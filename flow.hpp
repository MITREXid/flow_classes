#ifndef FLOW_HPP
#define FLOW_HPP
#include "universal_object.hpp"
#include "magistral.hpp"
// #include "config.hpp"
#include "dyvka.hpp"
#include "MY110.hpp"

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
            return;
        }
        if(st == state_Flow::do_stop){
            d_println(F("AAAAA for stop only function stop()!!!!!!"));
            return;
        }
        
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
        setStatus(state_Flow::do_stop);
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
                if(get_mag(0)->inThisStateId(16, false) && get_mag(1)->inThisStateId(16, false) 
                && get_mag(2)->inThisStateId(16, false) && get_mag(3)->isStartedState(true)){
                    dyvka.set_goal_frec(4000);
                    get_mag(3)->start(state_Alg_mag::one_cycle_complex);
                }

                if(isStartState(0, true) && !flag_already_started_solo_in_complex){
                    flag_already_started_solo_in_complex = true;
                    dyvka.set_goal_frec(4000);
                    get_mag(3)->start(state_Alg_mag::one_cycle_complex);
                    // num_curr_mag = get_num_curr_mag_next(true);
                    // get_mag()->start(state_Alg_mag::one_cycle_complex);
                    // get_mag(get_num_curr_mag_next(true))->start(state_Alg_mag::prepare_complex);
                    // if(get_mag(0)->getState()==state_Magistral::start_state){

                    // }
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
                dyvka.set_goal_frec(FREQ_SOLO);
                get_mag()->start(state_Alg_mag::one_cycle_solo);
                break;
            case state_Flow::do_complex:
                    flag_already_started_solo_in_complex = false;
                num_curr_mag = 0;
                dyvka.set_goal_frec(FREQ_COMPLEX);
                for(uint8_t i = 0; i<kol_mag_group;++i){
                    get_mag(i)->start(state_Alg_mag::one_cycle_complex);
                }
                break;
            case state_Flow::do_produvka:
                num_curr_mag = 3;
                dyvka.set_goal_frec(FREQ_AIR);
                get_mag()->start(state_Alg_mag::produvka);
                break;
            case state_Flow::do_clearing:
                num_curr_mag = 0;
                dyvka.set_goal_frec(FREQ_CLEARING);
                for(uint8_t i = 0; i<kol_all_mag;++i){
                    get_mag(i)->start(state_Alg_mag::clearing);
                }
                break;
            case state_Flow::do_triplet:
                num_curr_mag = 0;
                dyvka.set_goal_frec(FREQ_TRIPLET);
                for(uint8_t i = 0; i<kol_mag_group;++i){
                    get_mag(i)->start(state_Alg_mag::one_cycle_triplet);
                }
                break;

        }
    }





   

};

#endif // FLOW_HPP
