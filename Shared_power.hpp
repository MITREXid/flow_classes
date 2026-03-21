#ifndef Shared_power_HPP
#define Shared_power_HPP
#include "universal_object.hpp"

class Shared_power{//здесь статус это число тех кому ещё нужно питание
private:
    int state = 0;
    char control_pin = -1;
    enum class st_power_class{st_on = 0,st_off = 1} st_power; 
    void on_(){pinMode(control_pin, LOW);st_power = st_power_class::st_on;}
    void off_(){pinMode(control_pin, HIGH);st_power = st_power_class::st_off;}
    st_power_class get_st_power(){return st_power;}
public:
    Shared_power(char control_pin_);
    void voltageON();
    void voltageOFF();
    void update();
    void setStatus(int status);
    int getStatus();
    void init();

};

void Shared_power::init(){
    pinMode(control_pin, OUTPUT);
    off_();
}

Shared_power::Shared_power(char control_pin_){
    control_pin = control_pin_;
    setStatus(0);
}

void Shared_power::voltageON()
{
    setStatus(state+1);
}

void Shared_power::voltageOFF()
{
    setStatus(state-1);
}

void Shared_power::update()
{
    // d_print(F("PWM = "));
    // d_print(getStatus());
    // d_print(F(" : "));
    // d_println((int)get_st_power());
    if(getStatus()>0 && get_st_power() != st_power_class::st_on){
    d_println(F("PWM = on"));
        on_();
        return;
    }
    if(getStatus()<=0 && get_st_power() != st_power_class::st_off){
    d_println(F("PWM = off"));
        off_();
    }
}

void Shared_power::setStatus(int status){
    if(status<0){
        d_println(F("Error Shared_power status < 0"));
        status = 0;
    }
    state = status;
    d_print(state);d_println(F(" set PWM"));
}

int Shared_power::getStatus(){
    return state;
}
#endif // Shared_power_HPP
