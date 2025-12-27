// #include "Arduino.h"
#include "timer.hpp" 
#include "component.hpp" 


Timer timer1;
Timer timer2;
Component comp(state_Component::open);
unsigned long timer_delay_loop ;
void setup()
{
    #if(!mode_work)//arduino
        Serial.begin(9600);
    #endif
    println("start");

    timer1.setTime(1000);
    timer2.setTime(1000);
    comp.setTimeClosing(2000);
    comp.setTimeOpening(4000);
    comp.close();
    comp.setFuncStartOpen([](){println("StartOpen");});
    comp.setFuncEndOpen([](){println("EndOpen");comp.close();});
    comp.setFuncStartClose([](){println("StartClose");});
    comp.setFuncEndClose([](){println("EndClose");comp.open();});


    // timer1.setFuncEnd(
    //     []()
    //     {
    //         println("1: ");
    //         if(timer2.getStatus() == state_Timer::going){
    //             timer1.restart();
    //         }
    //     });
    timer2.setFuncEnd([](){ println("end2");});

    // timer1.restart();
    timer2.restart();
    timer_delay_loop = millis();
}

void loop()
{
    if(millis() - timer_delay_loop >= 100){

        timer_delay_loop = millis();

        comp.update();
        timer1.update();
        timer2.update();
    }
}




