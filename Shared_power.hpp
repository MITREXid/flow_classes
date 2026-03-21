#ifndef Shared_power_HPP
#define Shared_power_HPP
#include "universal_object.hpp"

class Shared_power{//здесь статус это число тех кому ещё нужно питание
private:
    int state = 0;
    char control_pin = -1;
public:
    Shared_power(char control_pin_);
    void voltageON();
    void voltageOFF();
    void update();
    void setStatus(int status);
    int getStatus();

};

Shared_power::Shared_power(char control_pin_){
    control_pin = control_pin_;
    setStatus(0);
    pinMode(control_pin, OUTPUT);
    pinMode(control_pin, HIGH);
}

void Shared_power::voltageON()
{
    pinMode(control_pin, LOW);
    setStatus(state+1);
}

void Shared_power::voltageOFF()
{
    setStatus(state-1);
}

void Shared_power::update()
{
    static bool flag = 0;
    if(getStatus()==0 && flag){
        pinMode(control_pin, HIGH);
        flag = 0;
    }
    if(getStatus()>0){
        flag = 1;
    }
}

void Shared_power::setStatus(int status){
    if(getStatus()<0){
        d_println(F("Error Shared_power status < 0"));
        status = 0;
    }
    state = status;
}

int Shared_power::getStatus(){
    return state;
}
#endif // Shared_power_HPP
