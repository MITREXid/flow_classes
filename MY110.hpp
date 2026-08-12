#ifndef MY110_HPP
#define MY110_HPP
#include "universal_object.hpp"
#include "signal.h"
#include "flow.hpp"
#include "config.hpp"
#include "other_components.hpp"

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
        uint32_t mask_pins_curr = 0;
        uint32_t mask_pins_new = 0;
        uint32_t mask_pins_last = 0;
        
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

            
            turn_to_default_act_and_clap(false);
            send_mask(mask_pins_new, true);
            
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

            if(mask_pins_last != mask_pins_curr){
                send_mask(mask_pins_curr, true);
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
            if(state_prev == state_Component::going_close){
                pointers_to_components.clapan[id_clap]->close(false);// с аргументом конечного времени
                
                mask_pins_new = set_mask(mask_pins_curr, id_clap+4,pins_MY110_all_mags_act_clap_forward,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_act_clap_forward[id_clap][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_act_clap_forward[id_clap+4][i]);
                //     i++;
                // }
            }else if(state_prev == state_Component::going_open){
                pointers_to_components.clapan[id_clap]->open(false);// с аргументом конечного времени
                mask_pins_new = set_mask(mask_pins_curr, id_clap+4,pins_MY110_all_mags_act_clap_revers,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_act_clap_revers[id_clap][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_act_clap_revers[id_clap+4][i]);
                //     i++;
                // }
            }
            
            send_mask(mask_pins_new);
            comp_id_prev = id_clap+4;
        }

        void turn_act(int8_t id_act, state_Component state_prev){
            d_print(F("MY110: turn_act id = "));
            d_print((int)pointers_to_components.actuator[id_act]->get_id());
            d_print(F(" state_prev = "));
            d_println((int)state_prev);
            turn_to_default_act_and_clap();
            if(state_prev == state_Component::going_close){
                pointers_to_components.actuator[id_act]->close(false);// с аргументом конечного времени
                mask_pins_new = set_mask(mask_pins_curr, id_act,pins_MY110_all_mags_act_clap_forward,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_act_clap_forward[id_act][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_act_clap_forward[id_act][i]);
                //     i++;
                // }
            }else if(state_prev == state_Component::going_open){
                pointers_to_components.actuator[id_act]->open(false);// с аргументом конечного времени
                mask_pins_new = set_mask(mask_pins_curr, id_act,pins_MY110_all_mags_act_clap_revers,1);
                // int i = 0;
                // mask_pins_new = mask_pins_curr;
                // while(pins_MY110_all_mags_act_clap_revers[id_act][i] != -1){
                //     mask_pins_new = set_bit_1(mask_pins_new, pins_MY110_all_mags_act_clap_revers[id_act][i]);
                //     i++;
                // }
            }
            
            send_mask(mask_pins_new);
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

       void send_mask(uint32_t mask_, bool is_real_send = false){

            mask_pins_curr = mask_;
            
            if(is_real_send){
                uint16_t reg97_mask = 0;//(Биты 1-16)
                uint16_t reg98_mask = 0;//(Биты 17-32)
                reg97_mask = mask_ & 0xFFFF; // Извлекаем младшие 16 бит
                reg98_mask = (mask_ >> 16) & 0xFFFF; // Извлекаем старшие 16 бит

                node.setTransmitBuffer(0, reg98_mask); // Значение для регистра 0x0061
                node.setTransmitBuffer(1, reg97_mask); // Значение для регистра 0x0062
                uint8_t result = node.writeMultipleRegisters(0x0061, 2);
                d_print(F("RS485 MY110: REAL send mask ("));
                
                char buffer[32];
                d_print(to_binary_string(mask_, buffer));
                d_print(F(") : "));
                mask_pins_last = mask_pins_curr;
                if(result == node.ku8MBSuccess){
                    d_print(F("success"));
                }else{
                    d_print(F("error = "));
                    d_print((int)result);
                }
                    d_println("");
            }
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
