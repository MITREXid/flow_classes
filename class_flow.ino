// #include "Arduino.h"
#include "timer.hpp" 
#include "component.hpp" 
#include "other_components.hpp"
#include "component.hpp"

// Timer timer1;
// Timer timer2;
// Component comp(state_Component::open);
Shared_power_5V PWM(A1);
Actuator act(state_Component::close, PWM, 10);
Actuator act2(state_Component::open, PWM, 11);

unsigned long timer_delay_loop ;
unsigned long timer_event;
unsigned long state;

void setup()
{
    #if(!mode_work)//arduino
        Serial.begin(9600);
    #endif
    println("start");

    // act2.close();
    timer_delay_loop = millis();
    timer_event = millis();
    state = 0;
}

void loop()
{
    if(millis() - timer_delay_loop >= 100){

        timer_delay_loop = millis();
        act.update();
        act2.update();
    }
    // if(millis() - timer_event >= 5000 && state == 0){
    //     act.open();
    //     ++state;
    // }
    // if(millis() - timer_event >= 1000 && state == 1){
    //     act2.close();
    //     ++state;
    // }
    // if(millis() - timer_event >= 500){
    //     act.close();
    // }
}




