#ifndef MAGISTRAL_HPP
#define MAGISTRAL_HPP
#include "universal_object.hpp"
#include "other_components.hpp"
#include "Shared_power_5V.hpp"
enum state_Magistral {not_defined = 0, in_magistral = 1, air_off = 2, air_on = 3, in_gate = 4, full_open = 11};
class Magistral{//}: public Universal_object<state_Magistral>{
private:
    Actuator *actuator;
    Clapan *clapan;
    Ball_cran *ball_cran;
    state_Magistral state;
public:
    Magistral(Shared_power_5V &PWM_, int control_pin_actuator, int control_pin_clapan, int control_pin_ball_cran)
    {
        actuator = new Actuator(state_Component::close, PWM_, control_pin_actuator);
        clapan = new Clapan(state_Component::open, PWM_, control_pin_clapan);
        ball_cran = new Ball_cran(state_Component::close, control_pin_ball_cran);
        state = state_Magistral::in_magistral;
    }
    void turn_to(state_Magistral next_state);
    void update();
};

void Magistral::update()
{
}

void Magistral::turn_to(state_Magistral next_state){
    // if(actuator->getStatus() == state_Component::in_going ||
    //    clapan->getStatus() == state_Component::in_going ||
    //    ball_cran->getStatus() == state_Component::in_going){
    //     return;
    // }
    switch(next_state){
        case state_Magistral::not_defined:
            // actuator->stop_doing();
            // clapan->stop_doing();
            // ball_cran->stop_doing();
            break;
        case state_Magistral::in_magistral://актуатор закрыт клапан открыт шаровой кран закрыт
            // if(clapan->can_open()){
                clapan->open();
            // }else{
            //     println("cannot turn to in_magistral: clapan not close");
            //     return;
            // }
            // if(ball_cran->can_close()){
                ball_cran->close();
            // }else{
            //     println("cannot turn to in_magistral: ball_cran not open");
            //     return;
            // }
            // if(actuator->can_close()){
                actuator->close();
            // }else{
            //     println("cannot turn to in_magistral: actuator not open");
            //     return;
            // }
            state = state_Magistral::in_magistral;
            break;
        case state_Magistral::air_off://актуатор открыт клапан закрыт шаровой кран закрыт
            // if(actuator->can_open()){
                actuator->open();
            // }else{
            //     println("cannot turn to air_off: actuator not open");
            //     return;
            // }
            // if(clapan->can_close()){
                clapan->close();
            // }else{
            //     println("cannot turn to air_off: clapan not open");
            //     return;
            // }
            // if(ball_cran->can_close()){
                ball_cran->close();
            // }else{
            //     println("cannot turn to air_off: ball_cran not open");
            //     return;
            // }
            state = state_Magistral::air_off;
            break;
        case state_Magistral::air_on://актуатор открыт клапан закрыт шаровой кран открыт
            // if(actuator->can_open()){
                actuator->open();
            // }else{
            //     println("cannot turn to air_on: actuator not open");
            //     return;
            // }
            // if(clapan->can_close()){
                clapan->close();
            // }else{
            //     println("cannot turn to air_on: clapan not open");
            //     return;
            // }
            // if(ball_cran->can_open()){
                ball_cran->open();
            // }else{
            //     println("cannot turn to air_on: ball_cran not open");
            //     return;
            // }
            state = state_Magistral::air_on;
            break;
        case state_Magistral::in_gate://актуатор открыт клапан закрыт шаровой кран закрыт
            // if(actuator->can_open()){
                actuator->open();
            // }else{
            //     println("cannot turn to in_gate: actuator not open");
            //     return;
            // }
            // if(clapan->can_close()){
                clapan->close();
            // }else{
            //     println("cannot turn to in_gate: clapan not open");
            //     return;
            // }
            // if(ball_cran->can_close()){
                ball_cran->close();
            // }else{
            //     println("cannot turn to in_gate: ball_cran not open");
            //     return;
            // }
            state = state_Magistral::in_gate;
            break;
        case state_Magistral::full_open:
            println("not implemented full_open");
            break;
    };
}

#endif // MAGISTRAL_HPP
