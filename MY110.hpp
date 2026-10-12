#ifndef MY110_HPP
#define MY110_HPP
#include "universal_object.hpp"
#include "signal.h"
#include "flow.hpp"
#include "config.hpp"
#include "other_components.hpp"
#include "logger.hpp"

#if !mode_work
    #include <SoftwareSerial.h>
    #include "ModbusMaster/ModbusMaster.h"
#endif

enum class state_MY110{
    working,
    stoped
};

class MY110 : public Universal_object<state_MY110>{
    private:
        SoftwareSerial &rs485;
        ModbusMaster node;
        uint32_t mask_pins_curr = 0;//ЦЕЛЕВАЯ маска (что хочет логика). В МУ напрямую не пишется
        uint32_t mask_pins_new = 0;

        //FIX: выходной каскад. Маска доходит до МУ поэтапно: снять питание -> пауза ->
        //переключить выбор без тока -> пауза -> подать питание
        uint32_t out_mask = 0;//маска, подтверждённо записанная в МУ
        bool out_valid = false;//false - состояние выходов МУ неизвестно (после старта/ошибок)
        bool last_write_failed = false;
        uint16_t err_count = 0;
        uint32_t t_last_write = 0;
        uint32_t t_power_off = 0;//когда сняли питание с мотора
        uint32_t t_power_on = 0;//когда подали питание на мотор
        uint32_t t_select_change = 0;//когда переключили реле выбора
        uint32_t t_ball_off = 0;//когда отпустили реле шарового крана
        bool forced_off = false;//сработал лимит времени под током
        uint32_t forced_motor_mask = 0;

        //FIX: компонент, которому ещё не подали питание. Его таймер хода запускается только
        //когда питание реально подано (раньше таймер шёл с момента команды)
        int8_t pending_id = -1;//0-3 актуаторы, 4-7 клапаны
        state_Component pending_state = state_Component::undef;

        //для журнала
        int8_t moving_id = -1;//компонент, на который сейчас подано питание
        bool moving_open = false;
        uint32_t last_unsafe_logged = 0;
        int8_t stuck_id = -1;//за каким компонентом следим на "слишком долго в движении"
        uint32_t t_stuck_start = 0;
        bool stuck_logged = false;
        
        //данные по одному компонентк(актуатор или клапан)
        int8_t comp_id_curr = 0;//-1 начало, застревает на компоненте требующем смены состояния(0-3 актуаторы, 4-7 клапаны)
        int8_t comp_id_prev = -1;//нужно для того чтобы не застревать на одном компоненте, если он уже в процессе смены состояния
        state_Component comp_clap_act_state_prev = state_Component::undef;// дас  возможность досрочно поменять на противоположное состояние(до завершения текузего процесса смены у того же компонента)

        struct struct_pointers_to_components{
            Actuator * actuator[kol_all_mag]; 
            Clapan * clapan[kol_all_mag]; 
            Ball_cran * ball_cran[kol_all_mag]; 
        }pointers_to_components;

        state_Component comp_ball_state_prev[kol_all_mag] = {state_Component::undef, state_Component::undef, state_Component::undef, state_Component::undef};//чтоб не повторять сигнал для шарового крана
    public:
        
        

        MY110(SoftwareSerial &rs485_):
        rs485(rs485_)
        
        {
            setStatus(state_MY110::working);
        }

        void init(){

            node.begin(16, rs485);
            node.preTransmission(preTransmission);
            node.postTransmission(postTransmission);

            
            //FIX: при старте гарантированно выключаем все выходы МУ (с повторами)
            mask_pins_curr = 0;
            uint32_t t_init = millis();
            while(!out_valid && millis() - t_init < T_MY110_INIT_WAIT){
                if(write_mask(0, err_count == 0)){
                    out_mask = 0;
                    out_valid = true;
                    t_power_off = millis();
                }else{
                    time_break(100);
                }
            }
            if(!out_valid){
                log_event(LOG_ERR_INIT);
            }
            
            d_println(F("MY110 init done"));
        }

        void set_component(Actuator *m1_act, Clapan *m1_clap, Ball_cran *m1_ball,
        Actuator *m2_act, Clapan *m2_clap, Ball_cran *m2_ball,
        Actuator *m3_act, Clapan *m3_clap, Ball_cran *m3_ball,
        Actuator *m4_act, Clapan *m4_clap, Ball_cran *m4_ball
        ){
            // Заполняем массив actuator
            pointers_to_components.actuator[0] = m1_act;
            pointers_to_components.actuator[1] = m2_act;
            pointers_to_components.actuator[2] = m3_act;
            pointers_to_components.actuator[3] = m4_act;

            // Заполняем массив clapan
            pointers_to_components.clapan[0] = m1_clap;
            pointers_to_components.clapan[1] = m2_clap;
            pointers_to_components.clapan[2] = m3_clap;
            pointers_to_components.clapan[3] = m4_clap;

            // Заполняем массив ball_cran
            pointers_to_components.ball_cran[0] = m1_ball;
            pointers_to_components.ball_cran[1] = m2_ball;
            pointers_to_components.ball_cran[2] = m3_ball;
            pointers_to_components.ball_cran[3] = m4_ball;

        }

        void update(){
            logic();
            output_stage();
        }

        //на моторы (DO25..DO32) сейчас не подано питание
        bool is_motor_off(){
            //out_mask - то, что мы последний раз успешно записали; при старте 0.
            //Если связи нет вообще, питание на моторы нами не подавалось
            return !(out_mask & MY110_POWER_BITS);
        }
        // int8_t get_curr_id(){
        //     return comp_id_curr;
        // }
        int8_t get_curr_type(){//0 - актуатор, 1 - клапан
            if(comp_id_curr <0){
                d_println(F("ERROR!!! get_curr_type: comp_id_curr <0"));
                return 0;
            }else if(comp_id_curr < 4){
                return 0;//актуатор
            }else if(comp_id_curr < 8){
                return 1;//клапан
            }else{
                d_println(F("ERROR!!! get_curr_type: comp_id_curr >= 8"));
                return 0;
            }
        }
        int8_t next_id(){
            comp_id_curr++;
            if(comp_id_curr>7){
                comp_id_curr = 0;
            }
            return comp_id_curr;
        }
        int8_t prev_id(){
            comp_id_curr--;
            if(comp_id_curr<0){
                comp_id_curr = 7;
            }
            return comp_id_curr;
        }

    private:


        void logic(){

            for(int j = 0; j<kol_all_mag;++j){
                if(pointers_to_components.ball_cran[j]->is_going() && comp_ball_state_prev[j] != pointers_to_components.ball_cran[j]->getStatus()){
                    comp_ball_state_prev[j] = pointers_to_components.ball_cran[j]->getStatus();
                    turn_ball(j, pointers_to_components.ball_cran[j]->getStatus());
                }
            }

            bool flag = false;
            if(get_curr_type() == 0){
                if(pointers_to_components.actuator[comp_id_curr]->is_going()){
                    flag = true;
                }
            }else if(get_curr_type() == 1){
                if(pointers_to_components.clapan[comp_id_curr-4]->is_going()){
                    flag = true;
                }
            }
            check_stuck(flag);
            if(flag){//компонент в процессе смены состояния
                if(comp_id_curr != comp_id_prev){
                    if(get_curr_type() == 0){//актуатор
                        comp_clap_act_state_prev = pointers_to_components.actuator[comp_id_curr]->getStatus();
                        turn_act(comp_id_curr, comp_clap_act_state_prev);
                    }else if(get_curr_type() == 1){//клапан
                        comp_clap_act_state_prev = pointers_to_components.clapan[comp_id_curr-4]->getStatus();
                        turn_clap(comp_id_curr-4, comp_clap_act_state_prev);
                    }

                }else{

                    if(get_curr_type() == 0){//актуато
                        if(comp_clap_act_state_prev != pointers_to_components.actuator[comp_id_curr]->getStatus()){
                            comp_clap_act_state_prev = pointers_to_components.actuator[comp_id_curr]->getStatus();
                            turn_act(comp_id_curr, comp_clap_act_state_prev);
                        }else{
                            //ждем переход
                        }
                    }else if(get_curr_type() == 1){//клапан
                        if(comp_clap_act_state_prev != pointers_to_components.clapan[comp_id_curr-4]->getStatus()){
                            comp_clap_act_state_prev = pointers_to_components.clapan[comp_id_curr-4]->getStatus();
                            turn_clap(comp_id_curr-4, comp_clap_act_state_prev);
                        }else{
                           //ждем переход 
                        }
                    }

                }
            }else{
                // d_print(F("next_id = "));
                // d_print((int)comp_id_curr);
                // d_print(F("->"));
                next_id();
                // d_println((int)comp_id_curr);
                turn_to_default_act_and_clap();
            
            }

        }

        bool is_default_mask_clap_and_act(){
            int i = 0;
            uint32_t mask_temp = mask_pins_curr;
            while(pins_MY110_all_mags_clear[0][i] != -1){
                if(check_bit(mask_temp, pins_MY110_all_mags_clear[0][i]-1) == 1){
                    return false;
                }
                i++;
            }
            return true;
        }

        void turn_ball(int8_t id_ball, state_Component state_prev){
            d_print(F("MY110: turn_ball id = "));
            d_print((int)pointers_to_components.ball_cran[id_ball]->get_id());
            d_print(F(" state_prev = "));
            d_println((int)state_prev);
            if(pointers_to_components.ball_cran[id_ball]->getStatus() == state_Component::going_close){
                pointers_to_components.ball_cran[id_ball]->close(false);
                log_event(LOG_BALL, id_ball);
                mask_pins_new = set_mask(mask_pins_curr, id_ball,pins_MY110_all_mags_ball_revers,0);
                mask_pins_new = set_mask(mask_pins_new, id_ball,pins_MY110_all_mags_ball_forward,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_ball_forward[id_ball][i] != -1){
                //     mask_pins_new = set_bit_0(mask_pins_new, pins_MY110_all_mags_ball_revers[id_ball][i]);
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_ball_forward[id_ball][i]);
                //     i++;
                // }
            }else if(pointers_to_components.ball_cran[id_ball]->getStatus() == state_Component::going_open){
                pointers_to_components.ball_cran[id_ball]->open(false);
                log_event(LOG_BALL, id_ball | 8);
                mask_pins_new = set_mask(mask_pins_curr, id_ball,pins_MY110_all_mags_ball_revers,1);
                mask_pins_new = set_mask(mask_pins_new, id_ball,pins_MY110_all_mags_ball_forward,0);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_ball_forward[id_ball][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_ball_revers[id_ball][i]);
                //     mask_pins_new = set_bit_0(mask_pins_new, pins_MY110_all_mags_ball_forward[id_ball][i]);
                //     i++;
                // }
            }
            send_mask(mask_pins_new);
        }



        void turn_clap(int8_t id_clap, state_Component state_prev){
            d_print(F("MY110: turn_clap id = "));
            d_print((int) pointers_to_components.clapan[id_clap]->get_id());
            d_print(F(" state_prev = "));
            d_println((int)state_prev);
            turn_to_default_act_and_clap();
            //клапаны с обратной полярностью мотора (CLAP_INVERT): меняем направление местами
            const int8_t * const * pins_close = CLAP_INVERT[id_clap] ? pins_MY110_all_mags_act_clap_revers : pins_MY110_all_mags_act_clap_forward;
            const int8_t * const * pins_open = CLAP_INVERT[id_clap] ? pins_MY110_all_mags_act_clap_forward : pins_MY110_all_mags_act_clap_revers;
            if(state_prev == state_Component::going_close){
                //FIX: close(false) вызывается в start_pending(), когда питание реально подано
                
                mask_pins_new = set_mask(mask_pins_curr, id_clap+4,pins_close,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_act_clap_forward[id_clap][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_act_clap_forward[id_clap+4][i]);
                //     i++;
                // }
            }else if(state_prev == state_Component::going_open){
                //FIX: open(false) вызывается в start_pending(), когда питание реально подано
                mask_pins_new = set_mask(mask_pins_curr, id_clap+4,pins_open,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_act_clap_revers[id_clap][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_act_clap_revers[id_clap+4][i]);
                //     i++;
                // }
            }
            
            send_mask(mask_pins_new);
            pending_id = id_clap+4;
            pending_state = state_prev;
            comp_id_prev = id_clap+4;
        }

        void turn_act(int8_t id_act, state_Component state_prev){
            d_print(F("MY110: turn_act id = "));
            d_print((int)pointers_to_components.actuator[id_act]->get_id());
            d_print(F(" state_prev = "));
            d_println((int)state_prev);
            turn_to_default_act_and_clap();
            if(state_prev == state_Component::going_close){
                //FIX: close(false) вызывается в start_pending(), когда питание реально подано
                mask_pins_new = set_mask(mask_pins_curr, id_act,pins_MY110_all_mags_act_clap_forward,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_act_clap_forward[id_act][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_act_clap_forward[id_act][i]);
                //     i++;
                // }
            }else if(state_prev == state_Component::going_open){
                //FIX: open(false) вызывается в start_pending(), когда питание реально подано
                mask_pins_new = set_mask(mask_pins_curr, id_act,pins_MY110_all_mags_act_clap_revers,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_act_clap_revers[id_act][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_act_clap_revers[id_act][i]);
                //     i++;
                // }
            }
            
            send_mask(mask_pins_new);
            pending_id = id_act;
            pending_state = state_prev;
            comp_id_prev = id_act;
        }



        void turn_to_default_act_and_clap(bool is_check_default_state = true){
            if(is_check_default_state && is_default_mask_clap_and_act()){
                return;
            }
            mask_pins_new = set_mask(mask_pins_curr, 0 ,pins_MY110_all_mags_clear,0);//id 0 - это клапана и актуаторы
            // int i = 0;
            // mask_pins_new = mask_pins_curr;
            // while(pins_MY110_clear_act_clap[i] != -1){
            //     mask_pins_new = set_bit_0(mask_pins_new, pins_MY110_clear_act_clap[i]);
            //     i++;
            // }
            send_mask(mask_pins_new);
        }

       //FIX: раньше при is_real_send маска сразу уходила в МУ одним кадром (реверс без паузы),
       //а mask_pins_last обновлялся даже при ошибке записи. Теперь здесь только целевая маска,
       //до МУ её доводит output_stage()
       void send_mask(uint32_t mask_, bool is_real_send = false){
            (void)is_real_send;
            mask_pins_curr = mask_;
        }

        static bool mask_is_safe(uint32_t m){
            for(uint8_t i = 0; i < sizeof(MY110_FORBIDDEN_PAIRS)/sizeof(MY110_FORBIDDEN_PAIRS[0]); ++i){
                if(check_bit(m, MY110_FORBIDDEN_PAIRS[i][0]-1) && check_bit(m, MY110_FORBIDDEN_PAIRS[i][1]-1)){
                    return false;
                }
            }
            //питание одновременно на шину актуаторов и на шину клапанов
            if((m & MY110_POWER_ACT) && (m & MY110_POWER_CLAP)){
                return false;
            }
            return true;
        }

        //Один шаг от текущего состояния выходов к целевому. Порядок:
        //моторы: снять питание -> T_MY110_MOTOR_BRAKE -> выбор без тока -> T_MY110_SELECT_SETTLE -> питание
        //шаровые: сначала отпустить -> T_MY110_BALL_GAP -> включить
        uint32_t next_step(uint32_t out, uint32_t target, uint32_t now){
            const uint32_t MOTOR = MY110_SELECT_BITS | MY110_POWER_BITS;
            uint32_t a_m = out & MOTOR;
            uint32_t t_m = target & MOTOR;
            uint32_t n_m = a_m;
            if(a_m != t_m){
                if(a_m & MY110_POWER_BITS){
                    n_m = a_m & MY110_SELECT_BITS;
                }else if(now - t_power_off < T_MY110_MOTOR_BRAKE){
                    //ждём остановки мотора
                }else if((a_m & MY110_SELECT_BITS) != (t_m & MY110_SELECT_BITS)){
                    n_m = t_m & MY110_SELECT_BITS;
                }else if(now - t_select_change >= T_MY110_SELECT_SETTLE){
                    n_m = t_m;
                }
            }

            uint32_t a_b = out & MY110_BALL_BITS;
            uint32_t t_b = target & MY110_BALL_BITS;
            uint32_t n_b = a_b;
            if(a_b != t_b){
                if(a_b & ~t_b){
                    n_b = a_b & t_b;
                }else if(now - t_ball_off >= T_MY110_BALL_GAP){
                    n_b = t_b;
                }
            }
            return n_m | n_b;
        }

        void output_stage(){
            const uint32_t MOTOR = MY110_SELECT_BITS | MY110_POWER_BITS;
            uint32_t now = millis();
            uint32_t target = mask_pins_curr;

            //лимит времени под током: снимаем питание, пока логика не выдаст новую команду
            if(forced_off && (target & MOTOR) != forced_motor_mask){
                forced_off = false;
            }
            if(!forced_off && (out_mask & MY110_POWER_BITS) && now - t_power_on >= T_MY110_MAX_MOTOR_ON){
                forced_off = true;
                forced_motor_mask = target & MOTOR;
                d_println(F("MY110 ERROR: motor ON too long -> power off"));
                log_event(LOG_ERR_LIMIT, moving_id < 0 ? 0xFF : moving_id, out_mask);
            }
            if(forced_off){
                target &= ~MOTOR;
            }

            if(!mask_is_safe(target)){
                char buffer[33];
                d_print(F("MY110 ERROR: unsafe mask blocked: "));
                d_println(to_binary_string(target, buffer));
                if(target != last_unsafe_logged){
                    last_unsafe_logged = target;
                    log_event(LOG_ERR_UNSAFE, 0, target);
                }
                target = out_mask & MY110_BALL_BITS;
            }

            uint32_t next = out_valid ? next_step(out_mask, target, now) : 0;

            bool need_write = !out_valid || next != out_mask || (now - t_last_write >= T_MY110_KEEPALIVE);
            if(need_write && !(last_write_failed && now - t_last_write < T_MY110_RETRY)){
                bool changed = !out_valid || next != out_mask;
                bool ok = write_mask(next, changed);
                t_last_write = now;
                last_write_failed = !ok;
                if(ok){
                    if(out_valid && (out_mask & MY110_POWER_BITS) && !(next & MY110_POWER_BITS)){
                        //ход закончен: пишем в журнал время под током
                        uint32_t on_ms = now - t_power_on;
                        if(on_ms > 0xFFFF){ on_ms = 0xFFFF; }
                        uint8_t arg = (moving_id < 0 ? 7 : moving_id) | (moving_open ? 8 : 0) | (forced_off ? 16 : 0);
                        log_event(LOG_MOVE, arg, on_ms | ((out_mask >> 24) << 16));
                        moving_id = -1;
                    }
                    if(!out_valid || ((out_mask & MY110_POWER_BITS) && !(next & MY110_POWER_BITS))){
                        t_power_off = now;
                    }
                    if(!(out_mask & MY110_POWER_BITS) && (next & MY110_POWER_BITS)){
                        t_power_on = now;
                    }
                    if((out_mask & MY110_SELECT_BITS) != (next & MY110_SELECT_BITS)){
                        t_select_change = now;
                    }
                    if(out_mask & MY110_BALL_BITS & ~next){
                        t_ball_off = now;
                    }
                    out_mask = next;
                    out_valid = true;
                }
            }

            start_pending();
        }

        //Компонент слишком долго в движении (раньше так проявлялось "залипание" логики):
        //пишем в журнал один раз за эпизод
        void check_stuck(bool is_going){
            if(!is_going || comp_id_curr != stuck_id){
                stuck_id = is_going ? comp_id_curr : -1;
                t_stuck_start = millis();
                stuck_logged = false;
                return;
            }
            if(!stuck_logged && millis() - t_stuck_start >= T_MY110_STUCK){
                stuck_logged = true;
                Component* comp = comp_id_curr < 4
                    ? static_cast<Component*>(pointers_to_components.actuator[comp_id_curr])
                    : static_cast<Component*>(pointers_to_components.clapan[comp_id_curr-4]);
                log_event(LOG_ERR_STUCK, comp_id_curr, (uint32_t)comp->getStatus());
            }
        }

        //запуск таймера хода компонента, когда на него реально подано питание
        void start_pending(){
            if(pending_id < 0 || forced_off){
                return;
            }
            const uint32_t MOTOR = MY110_SELECT_BITS | MY110_POWER_BITS;
            if((out_mask & MOTOR) != (mask_pins_curr & MOTOR) || !(out_mask & MY110_POWER_BITS)){
                return;
            }
            Component* comp = pending_id < 4
                ? static_cast<Component*>(pointers_to_components.actuator[pending_id])
                : static_cast<Component*>(pointers_to_components.clapan[pending_id-4]);
            moving_id = pending_id;
            moving_open = (pending_state == state_Component::going_open);
            if(pending_state == state_Component::going_close){
                comp->close(false);// с аргументом конечного времени
            }else if(pending_state == state_Component::going_open){
                comp->open(false);// с аргументом конечного времени
            }
            pending_id = -1;
        }

        bool write_mask(uint32_t mask_, bool is_log = true){
            uint16_t reg97_mask = mask_ & 0xFFFF; // (Биты 1-16)
            uint16_t reg98_mask = (mask_ >> 16) & 0xFFFF; // (Биты 17-32)

            node.setTransmitBuffer(0, reg98_mask); // Значение для регистра 0x0061
            node.setTransmitBuffer(1, reg97_mask); // Значение для регистра 0x0062
            uint8_t result = node.writeMultipleRegisters(0x0061, 2);
            bool ok = true;(result == node.ku8MBSuccess);
            if(ok){
                if(err_count > 0){
                    log_event(LOG_MB_OK, 0, err_count);
                }
                err_count = 0;
            }else{
                ++err_count;
                if(err_count == 1){//только первую ошибку серии, чтоб не забить журнал
                    log_event(LOG_ERR_MB, result, mask_);
                }
            }
            if(is_log || !ok){
                char buffer[33];
                d_print(F("RS485 MY110: send mask ("));
                d_print(to_binary_string(mask_, buffer));
                d_print(F(") : "));
                if(ok){
                    d_println(F("success"));
                }else{
                    d_print(F("error = "));
                    d_print((int)result);
                    d_print(F(" count = "));
                    d_println((int)err_count);
                }
            }
            return ok;
        }

        uint32_t set_mask(uint32_t mask, int8_t id_comp, const int8_t * const pins_mask[], int mode){//mode: 0 = set_bit_0, 1 = set_bit_1
            int i = 0;
            uint32_t mask_new = mask;
            while(pins_mask[id_comp][i] != -1){
                if(pins_mask[id_comp][i]<-1 || pins_mask[id_comp][i]>32){
                    d_println("ERROR!!!!! set_mask more 100 el in masss");
                    time_break(1000);
                }
                if(id_comp<0 || id_comp>7){
                    d_println("ERROR!!!!! set_mask more 100 el in masss");
                    time_break(1000);
                }
                if(i>32){
                    d_println("ERROR!!!!! set_mask more 100 el in masss");
                    time_break(1000);
                }

                if(mode == 0){
                    mask_new = set_bit_0(mask_new, pins_mask[id_comp][i]-1);
                }else if(mode == 1){
                    mask_new = set_bit_1(mask_new, pins_mask[id_comp][i]-1);
                }
                i++;


            }
            return mask_new;
        }

};

#endif // MY110_HPP
