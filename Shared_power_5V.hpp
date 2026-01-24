#ifndef SHARED_POWER_5V_HPP
#define SHARED_POWER_5V_HPP
#include "universal_object.hpp"

class Shared_power_5V: public Universal_object<int>{//здесь статус это число тех кому ещё нужно питание
private:
    int control_pin = -1;
public:
    Shared_power_5V(int control_pin_);
    void voltageON();
    void voltageOFF();
    void update();
    void setStatus(int status);

};

Shared_power_5V::Shared_power_5V(int control_pin_){
        control_pin = control_pin_;
        setStatus(0);
        pinMode(control_pin, OUTPUT);
    }

void Shared_power_5V::voltageON()
{
    pinMode(control_pin, HIGH);
    setStatus(getStatus()+1);
}

void Shared_power_5V::voltageOFF()
{
    setStatus(getStatus()-1);
}

void Shared_power_5V::update()
{
    if(getStatus()==0){
        pinMode(control_pin, LOW);
    }
}

void Shared_power_5V::setStatus(int status){
    if(getStatus()<0){
        println("Error Shared_power_5V status < 0");
        status = 0;
    }
    Universal_object::setStatus(status);
}

#endif // SHARED_POWER_5V_HPP
