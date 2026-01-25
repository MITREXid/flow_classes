#ifndef OTHER_COMPONENTS_HPP
#define OTHER_COMPONENTS_HPP
#include "component.hpp"
#include "Shared_power_5V.hpp"


//Конструктор надо вызывать или в setup или в loop чтобы не было проблем с инициализацией
class Actuator: public Component{
protected:
    Shared_power_5V& PWM;
    int control_pin = -1; 
public:
    Actuator(state_Component first_state, Shared_power_5V &PWM_, int control_pin_);
    void update();
};
 
Actuator::Actuator(state_Component first_state, Shared_power_5V &PWM_, int control_pin_)
  : PWM{PWM_}, control_pin{control_pin_}
{
    this->setTimeClosing(3000);
    this->setTimeOpening(3000);

    this->setFuncStartOpen([&](){
        println("Start Open");  
        pinMode(control_pin, LOW);
        PWM.voltageON();
    });

    this->setFuncEndOpen([&](){
        println("End Open");
       PWM.voltageOFF();
    });

    this->setFuncStartClose([&](){
        println("Start Close"); 
        pinMode(control_pin, HIGH);
       PWM.voltageON();
    });
    this->setFuncEndClose([&](){
        println("End Close"); 
       PWM.voltageOFF();
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



    //Конструктор надо вызывать или в setup или в loop чтобы не было проблем с инициализацией
class Clapan: public Actuator{
public:
    
    Clapan(state_Component first_state, Shared_power_5V &PWM_, int control_pin_);
    void update();
};
 
Clapan::Clapan(state_Component first_state, Shared_power_5V &PWM_, int control_pin_)
    :Actuator(first_state, PWM_, control_pin_)
{
    this->setTimeClosing(500);
    this->setTimeOpening(500);
}

void Clapan::update(){
    Actuator::update();
    PWM.update();
}


#endif // OTHER_COMPONENTS_HPP
