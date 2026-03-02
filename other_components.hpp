#ifndef OTHER_COMPONENTS_HPP
#define OTHER_COMPONENTS_HPP
#include "component.hpp"
#include "Shared_power_5V.hpp"


class Actuator: public Component{
protected:
    uint8_t control_pin; 
public:
    Actuator(state_Component first_state, uint8_t control_pin_);
    void update();
    void init(){
        Component::init();
        pinMode(control_pin, OUTPUT);
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
        2000, //время открытия
        2000 //время закрытия
    ),
    control_pin{control_pin_}
{}

void Actuator::update(){
    Component::update();
}



class Clapan: public Component{
private:
    uint8_t control_pin_forward = -1; 
    uint8_t control_pin_backward = -1; 
public:
    
    Clapan(state_Component first_state,  uint8_t control_pin_forward_,  uint8_t control_pin_backward_);
    void update();
    void init(){
        Component::init();
        pinMode(control_pin_forward, OUTPUT);
        pinMode(control_pin_backward, OUTPUT);
    }

    void funcSigStartOpen(){
        d_println(F("Start Open Clapan"));  
        digitalWrite(control_pin_forward, HIGH);
        digitalWrite(control_pin_backward, LOW);
    }

    void funcSigEndOpen(){
        d_println(F("End Open Clapan"));
        digitalWrite(control_pin_forward, LOW);
        digitalWrite(control_pin_backward, LOW);
    }

    void funcSigStartClose(){
        d_println(F("Start Close Clapan")); 
        digitalWrite(control_pin_forward, LOW);
        digitalWrite(control_pin_backward, HIGH);
    }
    void funcSigEndClose(){
        d_println(F("End Close Clapan")); 
        digitalWrite(control_pin_forward, LOW);
        digitalWrite(control_pin_backward, LOW);
    }
};
 
Clapan::Clapan(state_Component first_state, uint8_t control_pin_forward_ ,  uint8_t control_pin_backward_)
    : Component(
        first_state, //состояние по умолчанию
        500, //время открытия
        500 //время закрытия
    ),
    control_pin_forward{control_pin_forward_},
    control_pin_backward{control_pin_backward_}
{}

void Clapan::update(){
    Component::update();
}




class Ball_cran: public Component{
protected:
    int control_pin = -1; 
public:
    Ball_cran(state_Component first_state, int control_pin_);
    void update();

    void init(){
        Component::init();
        pinMode(control_pin, OUTPUT);
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
    }
    void funcSigEndClose(){
        d_println(F("End Close Ball_cran")); 
    }
};

Ball_cran::Ball_cran(state_Component first_state, int control_pin_)
: Component(
        first_state, //состояние по умолчанию
        10000, //время открытия
        10000 //время закрытия
 ) ,
  control_pin{control_pin_}
{}

void Ball_cran::update(){
    Component::update();
}

#endif // OTHER_COMPONENTS_HPP