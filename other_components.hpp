#ifndef OTHER_COMPONENTS_HPP
#define OTHER_COMPONENTS_HPP
#include "component.hpp"
#include "Shared_power.hpp"

class Actuator: public Component{
protected:
    uint8_t control_pin; 
public:
    Actuator(state_Component first_state, uint8_t control_pin_);
    void update();
    void init(){
        pinMode(control_pin, OUTPUT);
        digitalWrite(control_pin, HIGH);
        Component::init();
        update();
    }
      
    void funcSigStartOpen(){
        d_println(F("Start Open Actuator"));  
        digitalWrite(control_pin, LOW);
    }

    void funcSigEndOpen(){
        d_println(F("End Open Actuator"));
    }

    void funcSigStartClose(){
        d_println(F("Start Close Actuator")); 
        digitalWrite(control_pin, HIGH);
        
    }
    void funcSigEndClose(){
        d_println(F("End Close Actuator"));
    }
};
 
Actuator::Actuator(state_Component first_state, uint8_t control_pin_)
    : Component(
        first_state, //состояние по умолчанию
        1000, //время открытия
        1000 //время закрытия
    ),
    control_pin{control_pin_}
{}

void Actuator::update(){
    Component::update();
}


class Clapan: public Component{
private:
    Shared_power& PWM;
    int control_pin; 
public:
    
    Clapan(state_Component first_state, Shared_power &PWM_, int control_pin_);
    void update();

    void init(){
        pinMode(control_pin, OUTPUT);
        digitalWrite(control_pin, HIGH);
        Component::init();
        update();
    }


    void funcSigStartOpen(){
        d_println(F("Start Open Clapan"));  
        digitalWrite(control_pin, LOW);
        PWM.voltageON();
    }

    void funcSigEndOpen(){
        d_println(F("End Open Clapan"));
       PWM.voltageOFF();
    }

    void funcSigStartClose(){
        d_println(F("Start Close Clapan")); 
        digitalWrite(control_pin, HIGH);
       PWM.voltageON();
    }
    void funcSigEndClose(){
        d_println(F("End Close Clapan")); 
       PWM.voltageOFF();
    }
};
 
Clapan::Clapan(state_Component first_state, Shared_power &PWM_, int control_pin_)
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

    void init(){
        pinMode(control_pin, OUTPUT);
        digitalWrite(control_pin, HIGH);
        Component::init();
        update();
    }
    
     void funcSigStartOpen(){
        d_println(F("Start Open Ball_cran"));  
        digitalWrite(control_pin, LOW);
    }
    void funcSigEndOpen(){
        d_println(F("End Open Ball_cran"));
    }

    void funcSigStartClose(){
        d_println(F("Start Close Ball_cran")); 
        digitalWrite(control_pin, HIGH);
        //d_println(millis());
        
    }
    void funcSigEndClose(){
        d_println(F("End Close Ball_cran")); 
        //d_println(millis());
    }
};

Ball_cran::Ball_cran(state_Component first_state, int control_pin_)
: Component(
        first_state, //состояние по умолчанию
        15000, //время открытия
        15000 //время закрытия
 ) ,
  control_pin{control_pin_}
{}

void Ball_cran::update(){
    Component::update();
}

#endif // OTHER_COMPONENTS_HPP