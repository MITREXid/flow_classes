#ifndef MAGISTRAL_HPP
#define MAGISTRAL_HPP
#include "universal_object.hpp"
#include "other_components.hpp"
#include "Shared_power_5V.hpp"
#include "math.h"
#include "alg_state_mag.hpp"
#include "types_for_magistral.hpp"






class Magistral{//}: public Universal_object<state_Magistral>{
private:

   
    uint8_t id;
    Actuator actuator;
    Clapan clapan;
    Ball_cran ball_cran;
    uint32_t time_to_start_new_state = 0;//время старта
    // bool was_reset_time = false;//для того чтоб в состоянии в котором надо обнулить вермя не обнулять его бесконечно

    one_state_Magistral* current_state ;
    Data_alg &data_alg;
    state_Alg_mag curr_mode;
    
public:
    Magistral(uint8_t id_, Shared_power_5V &PWM_, uint8_t control_pin_actuator, uint8_t control_pin_clapan, uint8_t control_pin_ball_cran, Data_alg& d_)
    :data_alg{d_},
    actuator(state_Component::close, PWM_, control_pin_actuator),
    clapan(state_Component::close, PWM_, control_pin_clapan),
    ball_cran(state_Component::close, control_pin_ball_cran),
    id(id_)
    {
        // actuator = new Actuator(state_Component::close, PWM_, control_pin_actuator);
        // clapan = new Clapan(state_Component::close, PWM_, control_pin_clapan);
        // ball_cran = new Ball_cran(state_Component::close, control_pin_ball_cran);
    }
    void init();
    void turn_to(state_Magistral next_state);
    void update();
    void logic();//Для объединения логики поведения(используется в update())
    void start(state_Alg_mag mod);
    void stop();
    void set_current_mode_alg(state_Alg_mag mod);
    state_Alg_mag get_current_mode_alg();
    state_Magistral getState();
    // state_Magistral getState(one_state_Magistral* st = nullptr);
    // one_state_Magistral* getNextState(one_state_Magistral* st = nullptr);
    // uint32_t getTimeInState(one_state_Magistral* st = nullptr);
    // void setPathFlag( bool val,one_state_Magistral* st = nullptr);
    // bool getPathFlag(one_state_Magistral* st = nullptr);
    // bool getFlagResetTimer(one_state_Magistral* st = nullptr);
};


state_Magistral Magistral::getState(){
    return current_state->get_curr_state();
}

state_Alg_mag Magistral::get_current_mode_alg(){
    return curr_mode;
}

void Magistral::set_current_mode_alg(state_Alg_mag mod){
switch(mod){
    case state_Alg_mag::stop:
        data_alg.start_state->set_choose_path(id,0);
        data_alg.in_mag->set_choose_path(id,0);
        data_alg.exit_main_cycle->set_choose_path(id,1);
        break;
    case state_Alg_mag::prepare:
        data_alg.start_state->set_choose_path(id,1);
        data_alg.in_mag->set_choose_path(id,1);
        data_alg.exit_main_cycle->set_choose_path(id,1);
        break;
    case state_Alg_mag::one_cycle:
        data_alg.start_state->set_choose_path(id,1);
        data_alg.in_mag->set_choose_path(id,0);
        data_alg.exit_main_cycle->set_choose_path(id,1);
        break;
    case state_Alg_mag::cycle:
        data_alg.start_state->set_choose_path(id,1);
        data_alg.in_mag->set_choose_path(id,0);
        data_alg.exit_main_cycle->set_choose_path(id,0);
        break;
    case state_Alg_mag::produvka:
        data_alg.start_state->set_choose_path(id,2);
        data_alg.in_mag->set_choose_path(id,0);
        data_alg.exit_main_cycle->set_choose_path(id,1);
        break;
    case state_Alg_mag::clearing:
        data_alg.start_state->set_choose_path(id,3);
        data_alg.in_mag->set_choose_path(id,0);
        data_alg.exit_main_cycle->set_choose_path(id,1);
        break;
    default:
        d_println("Error set_current_mode_alg HZ mod");
        break;
}
}


void Magistral::turn_to(state_Magistral next_state){
    switch(next_state){
        case state_Magistral::undefine://актуатор закрыт клапан закрыт шаровой кран закрыт
                data_alg.start_state->set_choose_path(id, 0);
                turn_to(start_state);
                current_state = data_alg.start_state;
            break;
        case state_Magistral::start_state://актуатор закрыт клапан закрыт шаровой кран закрыт
                data_alg.start_state->set_choose_path(id, 0);
                actuator.close();
                clapan.close();
                ball_cran.close();
            break;
        case state_Magistral::going_to_gate://актуатор открыт клапан закрыт шаровой кран закрыт
                actuator.open();
                clapan.close();
                ball_cran.close();
            break;
         case state_Magistral::all_close://актуатор закрыт клапан закрыт шаровой кран закрыт
                actuator.close();
                clapan.close();
                ball_cran.close();
            break;
        case state_Magistral::in_magistral://актуатор закрыт клапан открыт шаровой кран закрыт
                actuator.close();
                clapan.open();
                ball_cran.close();
            break;
        case state_Magistral::air_on://актуатор закрыт клапан закрыт шаровой кран открыт
                actuator.close();
                clapan.close();
                ball_cran.open();
            break;
        case state_Magistral::full_open:
                actuator.open();
                clapan.open();
                ball_cran.open();
            break;
    };
    time_to_start_new_state = millis();
}

//Надо чтоб на этом этапе data_alg уже было заполнено
void Magistral::init()
{
    current_state = data_alg.start_state;
    clapan.init();
    actuator.init();
    ball_cran.init();
    
}


void Magistral::update()
{
    clapan.update();
    actuator.update();
    ball_cran.update();

    logic();
    
}


void Magistral::start(state_Alg_mag mod){
    d_println(F("========start"));
    set_current_mode_alg(mod);
}

void Magistral::stop(){
    d_println(F("========stop"));
    set_current_mode_alg(state_Alg_mag::stop);
}


void Magistral::logic(){
    

    if(
        clapan.getStatus() == state_Component::in_going ||
        actuator.getStatus() == state_Component::in_going ||
        ball_cran.getStatus() == state_Component::in_going
    ){
        return;
    }
    

    uint32_t curr_time = millis() - time_to_start_new_state;
    if(current_state->get_time_in_this() < curr_time && 
    current_state->get_curr_state() != current_state->get_next_state(id)->get_curr_state()){
        
        d_print(F("#"));
        d_print((int)id);
        d_print(F(" ========state: "));
        d_print(current_state->get_curr_state());
        d_print(F("->"));
        d_print(current_state->get_next_state(id)->get_curr_state());
        d_println(F("turn is:"));
        current_state = current_state->get_next_state(id);
        turn_to(current_state->get_curr_state());
    }

}



    // state_Magistral Magistral::getState(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println(F("AAAA nullptr getState");}
    //     return st->curr_state;
    // }
    // one_state_Magistral* Magistral::getNextState(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println(F("AAAA nullptr getNextState");}
    //     if(st->choose_conditional){
    //         if(st->conditional_path == nullptr){
    //             d_println(F("AAAA nullptr getNextState st->conditional_path");
    //         }
    //         return st->conditional_path;
    //     }else{
    //         if(st->next_state == nullptr){
    //             d_println(F("AAAA nullptr getNextState st->next_state");
    //         }
    //         return st->next_state;
    //     }
    // }
    // uint32_t Magistral::getTimeInState(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println(F("AAAA nullptr getTimeInState");}
    //     return st->time_in_this;
    // }
    // void Magistral::setPathFlag(bool val, one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println(F("AAAA nullptr setPathFlag");}
    //     st->choose_conditional = val;
    // }
    // bool Magistral::getPathFlag(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println(F("AAAA nullptr getPathFlag");}
    //     return st->choose_conditional;
    // }

    // bool Magistral::getFlagResetTimer(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println(F("AAAA nullptr getFlagResetTimer");}
    //     return st->flag_reset_timer;
    // }
#endif // MAGISTRAL_HPP
