#ifndef FLOW_HPP
#define FLOW_HPP
#include "universal_object.hpp"
#include "magistral.hpp"



#define kol_mag_group 3
#define kol_all_mag 4
#define solo_mag 3//nomer

enum class state_Flow{
        do_stop,
        do_solo,
        do_trio,
        do_produvka,
        do_clearing
};

class Flow: public Universal_object<state_Flow>{
    private:
        Data_alg data_alg;
        Shared_power_5V PWM;
        uint8_t num_in_group = 0;
    public:
        Magistral group[kol_all_mag];//+1 это соло
        // Magistral solo;
    Flow(uint8_t p_pwm,
        uint8_t p_act_1, uint8_t p_clap_1, uint8_t p_ball_1,
        uint8_t p_act_2, uint8_t p_clap_2, uint8_t p_ball_2,
        uint8_t p_act_3, uint8_t p_clap_3, uint8_t p_ball_3,
        uint8_t p_act_4, uint8_t p_clap_4, uint8_t p_ball_4 )
    : PWM(p_pwm), 
    group{
        Magistral(0x1, PWM, p_act_2, p_clap_2,p_ball_2, data_alg),
        Magistral(0x2, PWM, p_act_3, p_clap_3,p_ball_3, data_alg),
        Magistral(0x3, PWM, p_act_4, p_clap_4 ,p_ball_4, data_alg),
        Magistral(0x0, PWM, p_act_1, p_clap_1,p_ball_1, data_alg),//solo
    }
    {}

    uint8_t get_next_num_in_group(){
        if(num_in_group == kol_mag_group-1){
            return 0;
        }
        return num_in_group+1;
    }

    uint8_t get_prev_num_in_group(){
        if(num_in_group == 0){
            return kol_mag_group-1;
        }
        return num_in_group-1;
    }

    void init(){
        setup_alg_magistral(kol_all_mag, data_alg);
        for(uint8_t i = 0; i<kol_all_mag;++i){
            group[i].init();
        }
    }

    void logic(){
        
        //  switch(getStatus()){
        //     case state_Flow::stop:
        //         break;
        //     case state_Flow::solo:
        //         break;
        //     case state_Flow::trio:

        //         break;
        //     case state_Flow::produvka:
        //     if( group[num_in_group].getState() == state_Magistral::start_state){
        //         if(num_in_group == kol_mag_group-1){// четые подряд,поэтому отдельно для последгнего
        //             // num_in_group = get_next_num_in_group();
        //             group[kol_mag_group].start(state_Alg_mag::produvka);     
        //         }else{
        //             num_in_group = get_next_num_in_group();
        //             group[num_in_group].start(state_Alg_mag::produvka);   
        //         }     
        //     }
        //         break;
        //     case state_Flow::stop:
        //         break;
        //  }
    }


    void update(){
         for(uint8_t i = 0; i<kol_all_mag;++i){
            group[i].update();
         }

    }

    bool isStartState(){
        for(uint8_t i = 0; i<kol_all_mag;++i){
            if(group[i].get_current_mode_alg() != state_Alg_mag::stop && group[i].getState() != state_Magistral::start_state){
                return false;

            }
         }
         return true;
    }

    void start_solo(){
        if(!isStartState()){
            d_println("Error start_solo, not in Start State");
            return;
        }
        setStatus(state_Flow::do_solo);
        group[solo_mag].start(state_Alg_mag::cycle);
    }
    void start_trio(){
        if(!isStartState()){
            d_println("Error start_trio, not in Start State");
            return;
        }
        setStatus(state_Flow::do_trio);
        num_in_group = 0;
        group[num_in_group].start(state_Alg_mag::cycle);
        group[get_next_num_in_group()].start(state_Alg_mag::prepare);
    }

    void start_produv(){
        if(!isStartState()){
            d_println("Error start_produv, not in Start State");
            return;
        }
        setStatus(state_Flow::do_produvka);
        num_in_group = 0;
        group[num_in_group].start(state_Alg_mag::produvka);
    }



    void stop(){
        setStatus(state_Flow::do_stop);
         for(uint8_t i = 0; i<kol_all_mag;++i){
            group[i].stop();
         }
    }

};

#endif // FLOW_HPP
