#include "declorations.hpp"
#include "timer.hpp" 
#include "component.hpp" 
#include "other_components.hpp"
#include "component.hpp"
#include "magistral.hpp"

// Timer timer1;
// Timer timer2;
// Component comp(state_Component::open);
Shared_power_5V PWM(A1);
Actuator act(state_Component::close, PWM, 10);
Clapan act2(state_Component::open, PWM, 11);
Ball_cran act3(state_Component::close, 12);

Magistral mag(PWM,10,11,12);

unsigned long timer_delay_loop ;
unsigned long timer_event;
unsigned long state;

void setup()
{
    #if(!mode_work)//arduino
        Serial.begin(9600);
    #endif
    println("start");
    // act.init();
    // act2.init();
    // act3.init();
    mag.init();
    timer_delay_loop = millis();
    timer_event = millis();
    state = 0;
}

void loop()
{
    if(millis() - timer_delay_loop >= 20){

        timer_delay_loop = millis();
        mag.update();
        // act.update();
        // act2.update();
        // act3.update();
        // print("act3(");
        // print(millis());
        // print(") status: ");
        // println(act3.getStatus());
        // print("act_2(");
        // print(millis());
        // print(") status: ");
        // println(act2.getStatus());
    }
    if(millis() - timer_event >= 7000 && state == 0) {
        timer_event = millis();
        mag.start();
        // act.open();
        // act2.open();
        // act3.open();
        ++state;
    }
    if(millis() - timer_event >= 40000 && state == 1) {
        timer_event = millis();
        mag.stop();
        // // act.close();
        // act2.close();
        // act3.close();
        ++state;
    }
     if(millis() - timer_event >= 10000 && state == 2) {
        timer_event = millis();
        mag.start();
        // // act.close();
        // act2.close();
        // act3.close();
        ++state;
    }
    // if(millis() - timer_event >= 5000 && state == 1){
    //     timer_event = millis();
    //     act2.open();
    //     ++state;
    // }
    // if(millis() - timer_event >= 5000 && state == 2){
    //     timer_event = millis();
    //     act3.open();
    //     ++state;
    // }
    // if(millis() - timer_event >= 500){
    //     act.close();
    // }
}




