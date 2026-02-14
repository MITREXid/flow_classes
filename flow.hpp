#ifndef FLOW_HPP
#define FLOW_HPP
#include "universal_object.hpp"
#include "magistral.hpp"

struct state_Flow{
    enum class str_mode{//режим работы
        stop,
        solo,
        troica,
        produvka,
        clearing
    }mode = str_mode::stop;
    enum class str_staging{//стадия в этом режиме работы
        starting,
        go,
        stoping
    }staging = str_staging::go;
};

class Flow: public Universal_object<state_Flow>{
    private:
        Data_alg data_alg;
        Shared_power_5V PWM;
    public:
        Magistral group[kol_mag_];
        Magistral solo;
    Flow(uint8_t p_pwm,
        uint8_t p_act_1, uint8_t p_clap_1, uint8_t p_ball_1,
        uint8_t p_act_2, uint8_t p_clap_2, uint8_t p_ball_2,
        uint8_t p_act_3, uint8_t p_clap_3, uint8_t p_ball_3,
        uint8_t p_act_4, uint8_t p_clap_4, uint8_t p_ball_4 )
    : PWM(p_pwm), 
    solo(0x0, PWM, p_act_1, p_clap_1,p_ball_1, data_alg),
    group{
        Magistral(0x1, PWM, p_act_2, p_clap_2,p_ball_2, data_alg),
        Magistral(0x2, PWM, p_act_3, p_clap_3,p_ball_3, data_alg),
        Magistral(0x3, PWM, p_act_4, p_clap_4 ,p_ball_4, data_alg),
    }
    {}
    void init(){
        setup_alg_magistral(4, data_alg);
        // PWM = new Shared_power_5V(pins[0]);
        // solo = new Magistral();
        // group[0] = new Magistral(0x1, *PWM, pins[1],pins[2], pins[3], data_alg);
        // group[1]  = new Magistral(0x2, *PWM, pins[1],pins[2], pins[3], data_alg);
        // group[2]  = new Magistral(0x3, *PWM, pins[1],pins[2], pins[3], data_alg);
        
        solo.init();
         for(uint8_t i = 0; i<kol_mag_;++i){
            group[i].init();
         }
    }
    void update(){
         solo.update();
        //  for(uint8_t i = 0; i<kol_mag_;++i){
        //     group[i].update();
        //  }
    }
    void start_solo(){
         solo.start();
    }
    void stop_solo(){
         solo.stop();
    }

};

#endif // FLOW_HPP
