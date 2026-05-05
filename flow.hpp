#ifndef FLOW_HPP
#define FLOW_HPP
#include "universal_object.hpp"
#include "magistral.hpp"


#define kol_mag_group 3
#define kol_all_mag 4
#define solo_mag 3//nomer от 0


#include "dyvka.hpp"

enum class state_Flow{
        do_stop,
        do_solo,
        do_trio,
        do_produvka,
        do_clearing,
        do_triplet
};

class Flow: public Universal_object<state_Flow>{
    private:
        Data_alg data_alg;
        uint8_t num_curr_mag = 0;
        uint8_t pin_power_v12;
        Signal<> sig_ready_mag[kol_all_mag];
        Signal<> sig_ball_not_close[kol_all_mag];
        Dyvka dyvka;
        bool activated_stop_on_this_mag = false;//нужно чоб при остановке не отсылвть много раз stop()
    public:
        Shared_power pwm;
        Magistral group[kol_all_mag];//+1 это соло
        // Magistral solo;
    Flow(
        uint8_t p_act_1, uint8_t p_clap_1, uint8_t p_ball_1,
        uint8_t p_act_2, uint8_t p_clap_2, uint8_t p_ball_2,
        uint8_t p_act_3, uint8_t p_clap_3, uint8_t p_ball_3,
        uint8_t p_act_4, uint8_t p_clap_4, uint8_t p_ball_4,
        uint8_t pin_RX_dyvka, uint8_t pin_TX_dyvka, uint8_t pin_power_v12_, uint8_t pin_power_v12_clapan_)
    : 
    group{
        Magistral(0x0, p_act_1, p_clap_1, p_ball_1, pwm, data_alg, sig_ready_mag[0], sig_ball_not_close[0]),
        Magistral(0x1, p_act_2, p_clap_2, p_ball_2, pwm, data_alg, sig_ready_mag[1], sig_ball_not_close[1]),
        Magistral(0x2, p_act_3, p_clap_3, p_ball_3, pwm, data_alg, sig_ready_mag[2], sig_ball_not_close[2]),
        Magistral(0x3, p_act_4, p_clap_4, p_ball_4, pwm, data_alg, sig_ready_mag[3], sig_ball_not_close[3]),//solo
    },
    dyvka(pin_RX_dyvka, pin_TX_dyvka, sig_ball_not_close),
    pin_power_v12(pin_power_v12_),
    pwm{Shared_power(pin_power_v12_clapan_)}
    {}

    void init(){
        setup_alg_magistral(kol_all_mag, data_alg);
        pinMode(pin_power_v12,OUTPUT);
        d_println(F("power_12V: HIGH"));
        digitalWrite(pin_power_v12,HIGH);

        pwm.init();


        uint32_t time_mark = millis();
        while(millis()-time_mark<100){}

        dyvka.init();
        
        for(uint8_t i = 0; i<kol_all_mag;++i){
            get_mag(i)->init();
            
            d_println(F("power_12V: LOW"));
            digitalWrite(pin_power_v12,LOW);
            pwm.voltageON();
            pwm.update();

            time_mark = millis();
            while(millis()-time_mark<1000){}

            
            d_println(F("power_12V: HIGH"));
            digitalWrite(pin_power_v12,HIGH);
            pwm.voltageOFF();
            pwm.update();

            
            time_mark = millis();
            while(millis()-time_mark<1000){}
            //  get_mag(i)->update();
        }

        time_mark = millis();
        while(millis()-time_mark<100){}


        d_println(F("power_12V: LOW"));
        digitalWrite(pin_power_v12,LOW);
    }

    void update(){
         pwm.update();
         for(uint8_t i = 0; i<kol_all_mag;++i){
            get_mag(i)->update();
         }
        dyvka.update();
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
            case state_Flow::do_trio:
                if(isStartState(num_curr_mag, true)){
                    num_curr_mag = get_num_curr_mag_next(true);
                    get_mag()->start(state_Alg_mag::one_cycle_trio);
                    get_mag(get_num_curr_mag_next(true))->start(state_Alg_mag::prepare_trio);
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
                dyvka.set_goal_frec(4000);
                get_mag()->start(state_Alg_mag::cycle_solo);
                break;
            case state_Flow::do_trio:
                num_curr_mag = 0;
                dyvka.set_goal_frec(4000);
                get_mag()->start(state_Alg_mag::one_cycle_trio);
                get_mag(get_num_curr_mag_next(true))->start(state_Alg_mag::prepare_trio);
                break;
            case state_Flow::do_produvka:
                num_curr_mag = 3;
                dyvka.set_goal_frec(5000);
                get_mag()->start(state_Alg_mag::produvka);
                break;
            case state_Flow::do_clearing:
                num_curr_mag = 0;
                dyvka.set_goal_frec(5000);
                for(uint8_t i = 0; i<kol_mag_group;++i){
                    get_mag(i)->start(state_Alg_mag::clearing);
                }
                break;
            case state_Flow::do_triplet:
                num_curr_mag = 0;
                dyvka.set_goal_frec(5000);
                for(uint8_t i = 0; i<kol_mag_group;++i){
                    get_mag(i)->start(state_Alg_mag::one_cycle_triplet);
                }
                break;

        }
    }





   

};

#endif // FLOW_HPP
