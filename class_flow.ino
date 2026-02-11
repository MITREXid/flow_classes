#include "declorations.hpp"
#include "timer.hpp" 
#include "component.hpp" 
#include "other_components.hpp"
#include "component.hpp"
#include "magistral.hpp"
#include "flow.hpp"

// Timer timer1;
// Timer timer2;
// Component comp(state_Component::open);
Shared_power_5V PWM(A1);
// Actuator *act;
// Ball_cran act3(state_Component::close, 12);
// Magistral mag(PWM,10,11,12, &mag_start_state);
Flow flow(10,11,12,13);
// Magistral mag1(PWM,10,11,12);
// Magistral mag2(PWM,10,11,12);
// Magistral mag3(PWM,10,11,12);
// Signal<> sig_start;
// Signal<> sig_end;
// Timer t1(1000, &sig_start, &sig_end, nullptr);

unsigned long timer_delay_loop ;
unsigned long timer_event;
unsigned long state;

void setup()
{
    #if(!mode_work)//arduino
        Serial.begin(9600);
        Serial.println("start");
        int val = freeRAM();
        Serial.println(freeRAM());
    #endif
    
    // Flow flow1(10,11,12,13);
// Magistral mag1(PWM,10,11,12);
// act = new Actuator ( state_Component::close, PWM, 10);
// Clapan *act2 = new Clapan (state_Component::open, PWM, 11);
// Ball_cran *act3 = new Ball_cran (state_Component::close, 12);
// Component *comp = new Component{
//         0, //состояние по умолчанию
//         700, //время открытия
//         700 //время закрытия
//     };

    #if(!mode_work)//arduino
        Serial.println(freeRAM());
        Serial.println("===");
        Serial.println(val-freeRAM());
    #endif
    // act.init();
    // act2.init();
    // act3.init();
    flow.init();
    // mag.init();
    // mag1.init();
    // mag2.init();
    // mag3.init();
    timer_delay_loop = millis();
    timer_event = millis();
    state = 0;
    // t1.restart();
        d_print("Staart");
}

void loop()
{
    if(millis() - timer_delay_loop >= 20){

    // println(freeRAM());
    // t1.update();
    // if(sig_end.isTrueAndNotCheked()){
    //     d_print("Stoop");
    // }
        timer_delay_loop = millis();
        flow.update();
    // mag1.update();
    // mag2.update();
    // mag3.update();
        // act->update();
        // act2.update();
        // act3.update();
        // d_print("act3(");
        // d_print(millis());
        // d_print(") status: ");
        // d_println(act3.getStatus());
        // d_print("act_2(");
        // d_print(millis());
        // d_print(") status: ");
        // d_println(act2.getStatus());
    }
    if(millis() - timer_event >= 7000 && state == 0) {
        timer_event = millis();

    flow.start_solo();
        // mag.start();
        // act->open();
    // mag1.start();
    // mag2.start();
    // mag3.start();
        // act.open();
        // act2.open();
        // act3.open();
        ++state;
    }
    if(millis() - timer_event >= 60000 && state == 1) {
        timer_event = millis();
        flow.stop_solo();
        // mag.stop();
        // act->close();
        // // act.close();
        // act2.close();
        // act3.close();
        ++state;
    }
     if(millis() - timer_event >= 40000 && state == 2) {
        timer_event = millis();
        // mag.start();
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




