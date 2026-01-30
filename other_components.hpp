#ifndef OTHER_COMPONENTS_HPP
#define OTHER_COMPONENTS_HPP
#include "component.hpp"
#include "Shared_power_5V.hpp"


class Actuator: public Component{
protected:
    Shared_power_5V& PWM;
    int control_pin; 
public:
    Actuator(state_Component first_state, Shared_power_5V &PWM_, int control_pin_);
    void update();
};
 
Actuator::Actuator(state_Component first_state, Shared_power_5V &PWM_, int control_pin_)
    : Component{
        first_state, //состояние по умолчанию
        1300, //время открытия
        1300 //время закрытия
    },
    PWM{PWM_},
    control_pin{control_pin_}
{

    this->setFuncStartOpen([&](){
        println("Start Open Actuator");  
        pinMode(control_pin, LOW);
        PWM.voltageON();
    });

    this->setFuncEndOpen([&](){
        println("End Open Actuator");
       PWM.voltageOFF();
    });

    this->setFuncStartClose([&](){
        println("Start Close Actuator"); 
        pinMode(control_pin, HIGH);
       PWM.voltageON();
    });
    this->setFuncEndClose([&](){
        println("End Close Actuator"); 
       PWM.voltageOFF();
    });
}

void Actuator::update(){
    Component::update();
    PWM.update();
}



    //Конструктор надо вызывать или в setup или в loop чтобы не было проблем с инициализацией
class Clapan: public Component{
private:
    Shared_power_5V& PWM;
    int control_pin; 
public:
    
    Clapan(state_Component first_state, Shared_power_5V &PWM_, int control_pin_);
    void update();
};
 
Clapan::Clapan(state_Component first_state, Shared_power_5V &PWM_, int control_pin_)
    : Component{
        first_state, //состояние по умолчанию
        500, //время открытия
        500 //время закрытия
    },
    PWM{PWM_},
    control_pin{control_pin_}
{

    
    this->setFuncStartOpen([&](){
        println("Start Open Clapan");  
        pinMode(control_pin, LOW);
        PWM.voltageON();
    });

    this->setFuncEndOpen([&](){
        println("End Open Clapan");
       PWM.voltageOFF();
    });

    this->setFuncStartClose([&](){
        println("Start Close Clapan"); 
        pinMode(control_pin, HIGH);
       PWM.voltageON();
    });
    this->setFuncEndClose([&](){
        println("End Close Clapan"); 
       PWM.voltageOFF();
    });


}
void Clapan::update(){
    Component::update();
    PWM.update();
}




class Ball_cran: public Component{
protected:
    int control_pin = -1; 
public:
    Ball_cran(state_Component first_state, int control_pin_);
    void update();
};

Ball_cran::Ball_cran(state_Component first_state, int control_pin_)
: Component{
        first_state, //состояние по умолчанию
        5000, //время открытия
        5000 //время закрытия
    } ,
  control_pin{control_pin_}
{
   
    this->setFuncStartOpen([&](){
        println("Start Open Ball_cran");  
        pinMode(control_pin, LOW);
    });

    this->setFuncEndOpen([&](){
        println("End Open Ball_cran");
    });

    this->setFuncStartClose([&](){
        println("Start Close Ball_cran"); 
        pinMode(control_pin, HIGH);
    });
    this->setFuncEndClose([&](){
        println("End Close Ball_cran"); 
    });
}

void Ball_cran::update(){
    Component::update();
}

#endif // OTHER_COMPONENTS_HPP
