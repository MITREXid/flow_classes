#ifndef FLOW_HPP
#define FLOW_HPP
#include "universal_object.hpp"
#include "magistral.hpp"
// #include "config.hpp"
#include "dyvka.hpp"
#include "MY110.hpp"
#include "logger.hpp"
#include "settings.hpp"

enum class state_Flow{
        do_stop,
        do_solo,
        do_complex,
        do_produvka,
        do_clearing,
        do_triplet
};

class Flow: public Universal_object<state_Flow>{
    private:
        Data_alg data_alg;
        uint8_t num_curr_mag = 0;
        Signal<> sig_ready_mag[kol_all_mag];
        Signal<> sig_ball_not_close[kol_all_mag];
        Dyvka dyvka;
        bool activated_stop_on_this_mag = false;//нужно чоб при остановке не отсылвть много раз stop()
        bool flag_already_started_solo_in_complex = false;//нужно чоб при остановке не отсылвть много раз stop()
        MY110 my110;
        
        SoftwareSerial rs485;
    public:
        Magistral group[kol_all_mag];//+1 это соло
        // Magistral solo;
    Flow(uint8_t pin_RX_rs485, uint8_t pin_TX_rs485): 
    group{
        Magistral(0x0, data_alg, sig_ready_mag[0], sig_ball_not_close[0]),
        Magistral(0x1, data_alg, sig_ready_mag[1], sig_ball_not_close[1]),
        Magistral(0x2, data_alg, sig_ready_mag[2], sig_ball_not_close[2]),
        Magistral(0x3, data_alg, sig_ready_mag[3], sig_ball_not_close[3]),//solo
    },
    rs485{pin_RX_rs485, pin_TX_rs485},
    dyvka(rs485, sig_ball_not_close),
    my110(rs485)
    {}

    void init(){
        //инициализвция SoftwareSerial
        rs485.begin(9600);
        pinMode(pin_DE_RE, OUTPUT);
        digitalWrite(pin_DE_RE, LOW);
        setup_alg_magistral(kol_all_mag, data_alg);

        my110.init();
        
        for(uint8_t i = 0; i<kol_all_mag;++i){
            get_mag(i)->set_time_actuator(TIME_ACTUATOR_FULL_OPEN_CLOSE[i], TIME_ACTUATOR_FULL_OPEN_CLOSE[i]);
            get_mag(i)->init();
        }

       

        dyvka.init();
        
        
        my110.set_component(get_mag(0)->actuator.getPointer(), get_mag(0)->clapan.getPointer(), get_mag(0)->ball_cran.getPointer(),
        get_mag(1)->actuator.getPointer(), get_mag(1)->clapan.getPointer(), get_mag(1)->ball_cran.getPointer(),
        get_mag(2)->actuator.getPointer(), get_mag(2)->clapan.getPointer(), get_mag(2)->ball_cran.getPointer(),
        get_mag(3)->actuator.getPointer(), get_mag(3)->clapan.getPointer(), get_mag(3)->ball_cran.getPointer()
        );
    }

    void update(){
        //  pwm.update();
         for(uint8_t i = 0; i<kol_all_mag;++i){
            get_mag(i)->update();
         }
        dyvka.update();
        my110.update();
        logic();
    }

     void start(state_Flow st, uint8_t mag = solo_mag){
        if(!num_mag_in_range(mag)){
            return;
        }
        if(!isStartState()){
            d_println(F("Error start, not in Start State"));
            log_event(LOG_CMD, (uint8_t)st, log_cmd_source | 0x100);
            return;
        }
        if(st == state_Flow::do_stop){
            d_println(F("AAAAA for stop only function stop()!!!!!!"));
            return;
        }
        
        log_event(LOG_CMD, (uint8_t)st, log_cmd_source);
        dyvka.start_if_open();
        setStatus(st);
        logic_start(st, mag);
    }

     void stop(int8_t mag = -1){//-1 это проверка всех, если != -1 выбор номера
        d_println(F("stop flow"));
        if(mag != -1){
            if( num_mag_in_range(mag)){
               get_mag(mag)->stop();
            }
            return;
        }
        if(getStatus() != state_Flow::do_stop){
            log_event(LOG_CMD, (uint8_t)state_Flow::do_stop, log_cmd_source);
        }
        setStatus(state_Flow::do_stop);
    }

    //для экрана и журнала: все магистрали в стартовом состоянии и ничего не движется
    bool is_idle(){
        return isStartState(-1, false);
    }

    //Применить настройки программ. Частоты берутся из g_set при старте режима.
    //Дозирование медленным актуатором: порция = от начала открытия до команды "закрыть".
    //Порция меньше хода - актуатор только приоткрывается (питание на время порции), больше - открыт
    //полностью и стоит остаток. Закрытие всегда полным ходом до концевика.
    //+T_DOSE_START к шлюзу: питание на актуатор приходит ~0.3 с после команды (переключение реле полярности)
    void apply_settings(){
        const uint32_t act_ms = (uint32_t)g_set.act_ds * 100;
        const uint32_t extra = act_ms > 1000 ? act_ms - 1000 : 0;
        const uint32_t por_cx = (uint32_t)g_set.cx_portion * 100;
        const uint32_t por_tr = (uint32_t)g_set.tr_portion * 100;
        const uint32_t por_so = (uint32_t)g_set.so_portion * 100;
        for(uint8_t i = 0; i < kol_all_mag; ++i){
            get_mag(i)->set_time_actuator(act_ms, act_ms);
            get_mag(i)->set_act_times(act_ms, por_cx < act_ms ? por_cx : act_ms, por_tr < act_ms ? por_tr : act_ms,
                                      por_so < act_ms ? por_so : act_ms);
        }
        apply_act_time(data_alg, act_ms);
        uint32_t air[4];
        for(uint8_t m = 0; m < 4; ++m){ air[m] = (uint32_t)g_set.cx_air[m] * 1000; }
        apply_complex(data_alg, por_cx + T_DOSE_START, extra, air);
        for(uint8_t m = 0; m < 3; ++m){ air[m] = (uint32_t)g_set.tr_air[m] * 1000; }
        apply_triplet(data_alg, por_tr + T_DOSE_START, extra, air);
        apply_solo(data_alg, por_so + T_DOSE_START, extra, (uint32_t)g_set.so_air * 1000);
        apply_produv(data_alg, (uint32_t)g_set.pv_s * 1000);
        dyvka.set_ramp(g_set.ramp_s);
        dyvka.set_stop_ramp(g_set.stop_s);
    }

    //cfg times: рассчитанные времена шагов, зависящих от порции (для проверки на объекте)
    void print_timing(){
        #if !mode_work
        Serial.println(F("=== ВРЕМЕНА ШАГОВ, мс (магистраль 1/2/3/4) ==="));
        one_state_Magistral * st[12] = {data_alg.skip_gate_complex, data_alg.starting_complex, data_alg.in_magistral_complex,
                                        data_alg.air_until_solo_complex, data_alg.air_complex,
                                        data_alg.skip_gate_triplet, data_alg.starting_triplet, data_alg.air_triplet,
                                        data_alg.skip_gate_solo, data_alg.wait_with_coffe_in_mag_solo, data_alg.air_solo, data_alg.produv};
        const char * const names[12] = {"комплекс: шлюз", "комплекс: старт загрузки", "комплекс: после шлюза",
                                         "комплекс: воздух до соло", "комплекс: воздух", "триплет: шлюз", "триплет: старт загрузки",
                                         "триплет: воздух", "соло: шлюз", "соло: после шлюза", "соло: воздух", "продувка"};
        for(uint8_t i = 0; i < 12; ++i){
            Serial.print(names[i]); Serial.print(F(": "));
            for(uint8_t m = 0; m < kol_all_mag; ++m){
                Serial.print(st[i] ? st[i]->get_time_in_this(m) : 0);
                if(m < kol_all_mag - 1){ Serial.print('/'); }else{ Serial.println(); }
            }
        }
        #endif
    }

    //Изменить настройки (экран или USB). Нельзя только пока идёт режим
    //(при обрыве связи с МУ магистрали "висят", но режим не запущен - менять можно).
    bool change_settings(const FlowSettings &s){
        if(!is_idle() && getStatus() != state_Flow::do_stop){
            log_event(LOG_CFG_REJECT);
            return false;
        }
        g_set = s;
        settings_clamp(g_set);
        settings_save(LOG_SETTINGS_BASE);
        apply_settings();
        log_event(LOG_CFG, g_set.cx_portion, settings_pack(g_set));
        return true;
    }

    //давление, кПа x10; INT16_MIN - нет данных
    int16_t get_pressure_x10(){
        return dyvka.get_pressure_x10();
    }

    //отладка: прочитать регистры частотника (команда mbr в мониторе порта)
    void debug_read_vfd(uint16_t reg, uint8_t count){
        dyvka.debug_read(reg, count);
    }

    //журнал можно выводить: машина стоит или хотя бы моторы обесточены
    //(при обрыве связи с МУ компоненты "висят" в движении, а журнал нужнее всего)
    bool can_dump_log(){
        return is_idle() || my110.is_motor_off();
    }
private:
    uint8_t get_num_curr_mag_next(bool is_group = false){
        uint8_t max = kol_all_mag - 1;
        // uint8_t min = 0;
        uint8_t curr = num_curr_mag;
        if(is_group){
            max = kol_mag_group - 1;
        }
        if(curr == max){
            curr = 0;//min;
        }else{
            curr += 1;
        }
        if(!num_mag_in_range(curr)){return 0;}
        return curr;
    }

    uint8_t get_num_curr_mag_prev(bool is_group = false){
        uint8_t max = kol_all_mag - 1;
        // uint8_t min = 0;
        uint8_t curr = num_curr_mag;
        if(is_group){
            max = kol_mag_group - 1;
        }
        if(curr == 0){
            curr = max;
        }else{
            curr -= 1;
        }
        if(!num_mag_in_range(curr)){return 0;}
        return curr;
    }

    uint8_t num_mag_in_range(int8_t mag){
        if((mag>=0 && mag<kol_all_mag)){
            return true;
        }
        d_println(F("ERROR: num_mag_in_range num is no range"));
            return true;

    }

    Magistral* get_mag(int8_t mag = -1){//-1 - текущий или номер конкретного
        if(mag == -1){mag = num_curr_mag;}
        if(num_mag_in_range(mag)){
            return &group[mag];
        }
        return nullptr;
    }

   

    void logic(){
        
         switch(getStatus()){
            case state_Flow::do_stop:
                
                    if(!isStartState(-1, true)){//если все магистрали в стартовом состоянии(компоненты должны быть перекоючены)
                        if(!isStartState(num_curr_mag, true)){
                            if(activated_stop_on_this_mag == false){
                                activated_stop_on_this_mag = true;
                                d_print(F("Not START mag = "));
                                d_println((int)num_curr_mag);
                                get_mag(num_curr_mag)->stop();
                            }
                        }else{
                            num_curr_mag = get_num_curr_mag_next();
                            activated_stop_on_this_mag = false;
                        }
                    }else{
                        // d_println(F("FULL STOPED"));
                        activated_stop_on_this_mag = false;
                    }
                break;
            case state_Flow::do_solo:
                break;
            case state_Flow::do_complex:
                if(get_mag(2)->inThisStateId(17, true) && get_mag(3)->isStartedState(true)){
                    dyvka.set_goal_frec(g_set.cx_hz_s * 100);
                    get_mag(3)->start(state_Alg_mag::one_cycle_complex);
                }
                break;
            case state_Flow::do_produvka:
                if(isStartState(num_curr_mag, true)){
                        num_curr_mag = get_num_curr_mag_prev();
                        if(num_curr_mag != 3){//чтоб только по 1 разу кадждую
                            get_mag()->start(state_Alg_mag::produvka);
                        }else{
                            stop();//чтоб только по 1 разу кадждую
                        }
                }
                break;
            case state_Flow::do_clearing:
                if(get_mag(num_curr_mag)->inThisStateId(53, true)){
                    if(num_curr_mag != 0){//чтоб только по 1 разу кадждую
                        num_curr_mag = get_num_curr_mag_prev();
                        get_mag()->start(state_Alg_mag::clearing);
                    }
                }
                break;
            case state_Flow::do_triplet:
                break;
         }
    }



    bool isStartState(int8_t mag = -1, bool check_coponents_no_goin = false){//-1 это проверка всех, если != -1 выбор номера
        if(mag!=-1){
            if(num_mag_in_range(mag)){
                if(group[mag].isStartedState(check_coponents_no_goin)){
                    return true;
                }
            }
            return false;
        }
        for(uint8_t i = 0; i<kol_all_mag;++i){
            if(!(group[i].isStartedState(check_coponents_no_goin))){
            // if(group[i].get_current_mode_alg() != state_Alg_mag::stop && group[i].getState() != state_Magistral::start_state){
                return false;

            }
         }
         return true;
    }
   

    void logic_start(state_Flow st, uint8_t mag = solo_mag)
    {
        switch(st){ 
            case state_Flow::do_stop:
                break;
            case state_Flow::do_solo:
                num_curr_mag = mag;
                dyvka.set_goal_frec(g_set.so_hz * 100);
                get_mag()->start(state_Alg_mag::one_cycle_solo);
                break;
            case state_Flow::do_complex:
                    flag_already_started_solo_in_complex = false;
                num_curr_mag = 0;
                dyvka.set_goal_frec(g_set.cx_hz_t * 100);
                for(uint8_t i = 0; i<kol_mag_group;++i){
                    get_mag(i)->start(state_Alg_mag::one_cycle_complex);
                }
                break;
            case state_Flow::do_produvka:
                num_curr_mag = 3;
                dyvka.set_goal_frec(g_set.pv_hz * 100);
                get_mag()->start(state_Alg_mag::produvka);
                break;
            case state_Flow::do_clearing:
                num_curr_mag = 3;
                dyvka.set_goal_frec(g_set.cl_hz * 100);
                get_mag(num_curr_mag)->start(state_Alg_mag::clearing);
                break;
            case state_Flow::do_triplet:
                num_curr_mag = 0;
                dyvka.set_goal_frec(g_set.tr_hz * 100);
                for(uint8_t i = 0; i<kol_mag_group;++i){
                    get_mag(i)->start(state_Alg_mag::one_cycle_triplet);
                }
                break;

        }
    }





   

};

#endif // FLOW_HPP
