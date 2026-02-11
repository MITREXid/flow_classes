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

#define kol_mag_ 3
class Flow: public Universal_object<state_Flow>{
    private:
        Data_alg data_alg;
        Shared_power_5V *PWM;
        uint8_t pins[4];
    public:
        Magistral *group[kol_mag_];
        Magistral *solo;
    Flow(uint8_t p_pwm,uint8_t p_act_1, uint8_t p_clap_1, uint8_t p_ball_1 ){
        pins[0] = p_pwm;
        pins[1] = p_act_1;
        pins[2] = p_clap_1;
        pins[3] = p_ball_1;
    }
    void init(){
        data_alg = setup_alg_magistral(4);
        PWM = new Shared_power_5V(pins[0]);
        solo = new Magistral(0x0, *PWM, pins[1],pins[2], pins[3], data_alg);
        
        solo->init();
    }
    void update(){
         solo->update();
    }
    void start_solo(){
         solo->start();
    }
    void stop_solo(){
         solo->stop();
    }

};

#endif // FLOW_HPP
