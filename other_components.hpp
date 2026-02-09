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
      
    void funcSigStartOpen(){
        d_println("Start Open Actuator");  
        pinMode(control_pin, LOW);
        PWM.voltageON();
    }

    void funcSigEndOpen(){
        d_println("End Open Actuator");
       PWM.voltageOFF();
    }

    void funcSigStartClose(){
        d_println("Start Close Actuator"); 
        pinMode(control_pin, HIGH);
       PWM.voltageON();
    }
    void funcSigEndClose(){
        d_println("End Close Actuator"); 
       PWM.voltageOFF();
    }
};
 
Actuator::Actuator(state_Component first_state, Shared_power_5V &PWM_, int control_pin_)
    : Component(
        first_state, //состояние по умолчанию
        700, //время открытия
        700 //время закрытия
    ),
    PWM{PWM_},
    control_pin{control_pin_}
{}

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

        void funcSigStartOpen(){
        d_println("Start Open Clapan");  
        pinMode(control_pin, LOW);
        PWM.voltageON();
    }

    void funcSigEndOpen(){
        d_println("End Open Clapan");
       PWM.voltageOFF();
    }

    void funcSigStartClose(){
        d_println("Start Close Clapan"); 
        pinMode(control_pin, HIGH);
       PWM.voltageON();
    }
    void funcSigEndClose(){
        d_println("End Close Clapan"); 
       PWM.voltageOFF();
    }
};
 
Clapan::Clapan(state_Component first_state, Shared_power_5V &PWM_, int control_pin_)
    : Component(
        first_state, //состояние по умолчанию
        500, //время открытия
        500 //время закрытия
    ),
    PWM{PWM_},
    control_pin{control_pin_}
{}
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
     void funcSigStartOpen(){
        d_println("Start Open Ball_cran");  
        pinMode(control_pin, LOW);
    }
    void funcSigEndOpen(){
        d_println("End Open Ball_cran");
    }

    void funcSigStartClose(){
        d_println("Start Close Ball_cran"); 
        pinMode(control_pin, HIGH);
    }
    void funcSigEndClose(){
        d_println("End Close Ball_cran"); 
    }
};

Ball_cran::Ball_cran(state_Component first_state, int control_pin_)
: Component(
        first_state, //состояние по умолчанию
        5000, //время открытия
        5000 //время закрытия
 ) ,
  control_pin{control_pin_}
{}

void Ball_cran::update(){
    Component::update();
}

#endif // OTHER_COMPONENTS_HPP