// #include "Arduino.h"
#include "timer.hpp" 



Timer timer1;
Timer timer2;


void setup()
{
    
    println("start");

    timer1.setTime(1000);
    timer2.setTime(10000);

    timer1.setFuncEnd(
        []()
        {
            println("end1");
            if(timer2.getStatus() == state_Timer::going){
                timer1.restart();
            }
        });
    timer2.setFuncEnd([](){ println("end2");});

    timer1.restart();
    timer2.restart();
}

void loop()
{
    timer1.update();
    timer2.update();
}




