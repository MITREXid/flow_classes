#include "declorations.hpp"
#include "timer.hpp" 
#include "component.hpp" 
#include "other_components.hpp"
#include "component.hpp"

// Timer timer1;
// Timer timer2;
// Component comp(state_Component::open);
Shared_power_5V PWM(A1);
Actuator* act= nullptr; 
Clapan* act2= nullptr; 
Ball_cran* act3= nullptr; 

unsigned long timer_delay_loop ;
unsigned long timer_event;
unsigned long state;

void setup()
{
    #if(!mode_work)//arduino
        Serial.begin(9600);
    #endif
    act = new Actuator(state_Component::close, PWM, 10);
    act2 = new Clapan(state_Component::open, PWM, 11);
    act3 = new Ball_cran(state_Component::close, 12);
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
        act->update();
        act2->update();
        act3->update();
        // print("act3(");
        // print(millis());
        // print(") status: ");
        // println(act3->getStatus());
        // print("act_2(");
        // print(millis());
        // print(") status: ");
        // println(act2->getStatus());
    }
    if(millis() - timer_event >= 10000 && state == 0) {
        timer_event = millis();
        act3->open();
        ++state;
    }
    if(millis() - timer_event >= 7000 && state == 1) {
        timer_event = millis();
        act3->close();
        ++state;
    }
    // if(millis() - timer_event >= 5000 && state == 1){
    //     timer_event = millis();
    //     act2->open();
    //     ++state;
    // }
    // if(millis() - timer_event >= 5000 && state == 2){
    //     timer_event = millis();
    //     act3->open();
    //     ++state;
    // }
    // if(millis() - timer_event >= 500){
    //     act.close();
    // }
}




