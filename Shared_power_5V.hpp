#ifndef SHARED_POWER_5V_HPP
#define SHARED_POWER_5V_HPP
#include "universal_object.hpp"

class Shared_power_5V{//здесь статус это число тех кому ещё нужно питание
private:
    int state=0;
    char control_pin = -1;
public:
    Shared_power_5V(char control_pin_);
    void voltageON();
    void voltageOFF();
    void update();
    void setStatus(int status);
    int getStatus();

};

Shared_power_5V::Shared_power_5V(char control_pin_){
    control_pin = control_pin_;
    setStatus(0);
    pinMode(control_pin, OUTPUT);
}

void Shared_power_5V::voltageON()
{
    pinMode(control_pin, HIGH);
    setStatus(state+1);
}

void Shared_power_5V::voltageOFF()
{
    setStatus(state-1);
}

void Shared_power_5V::update()
{
    static bool flag = 0;
    if(getStatus()==0 && flag){
        pinMode(control_pin, LOW);
        flag = 0;
    }
    if(getStatus()>0){
        flag = 1;
    }
}

void Shared_power_5V::setStatus(int status){
    if(getStatus()<0){
        d_println("Error Shared_power_5V status < 0");
        status = 0;
    }
    state = status;
}

int Shared_power_5V::getStatus(){
    return state;
}
#endif // SHARED_POWER_5V_HPP
