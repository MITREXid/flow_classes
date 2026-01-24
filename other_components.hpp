#ifndef OTHER_COMPONENTS_HPP
#define OTHER_COMPONENTS_HPP
#include "component.hpp"
#include "Shared_power_5V.hpp"

class Actuator: public Component{
private:
    Shared_power_5V& PWM;
    int control_pin = -1; 
public:
    Actuator(state_Component first_state, Shared_power_5V &PWM_, int control_pin_);
    void update();
};
 
Actuator::Actuator(state_Component first_state, Shared_power_5V &PWM_, int control_pin_)
  : PWM{PWM_}, control_pin{control_pin_}
{
    static int control_pin_local = control_pin;
    static Shared_power_5V &PWM_local = PWM;
    this->setTimeClosing(3000);
    this->setTimeOpening(3000);

    this->setFuncStartOpen([](){
        println("Start Open");  
        pinMode(control_pin_local, LOW);
        PWM_local.voltageON();
    });

    this->setFuncEndOpen([](){
        println("End Open");
       PWM_local.voltageOFF();
    });

    this->setFuncStartClose([](){
        println("Start Close"); 
        pinMode(control_pin_local, HIGH);
       PWM_local.voltageON();
    });
    this->setFuncEndClose([](){
        println("End Close"); 
       PWM_local.voltageOFF();
    });

    if(first_state == in_going){
        println("Not correct start state. set close");
        first_state = state_Component::close;
    }

    if(state_Component::close == first_state){
        setStatus(state_Component::open);
        close();
    }else if(state_Component::open == first_state){
        setStatus(state_Component::close);
        open();
    }
}

    void Actuator::update(){
        Component::update();
        PWM.update();
    }


#endif // OTHER_COMPONENTS_HPP
