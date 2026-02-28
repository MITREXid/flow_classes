#include "declorations.hpp"
#include "timer.hpp" 
#include "component.hpp" 
#include "other_components.hpp"
#include "component.hpp"
#include "magistral.hpp"
#include "flow.hpp"
#include "display.hpp"

// Timer timer1;
// Timer timer2;
// Component comp(state_Component::open);
Shared_power_5V PWM(A1);
// Actuator *act;
// Ball_cran act3(state_Component::close, 12);
// Magistral mag(PWM,10,11,12, &mag_start_state);
Data_alg dalg;
Flow flow(10,11,12,13,
        11,12,13,
        11,12,13,
        11,12,13,
        11,12,13);
Display display(flow);
// Flow * flow2;
// Magistral mag1(PWM,10,11,12);
// Magistral mag2(PWM,10,11,12);
// Magistral mag3(PWM,10,11,12);
// Signal<> sig_start;
// Signal<> sig_end;
// Timer t1(1000, &sig_start, &sig_end, nullptr);
// Data_alg dalg;
unsigned long timer_delay_loop ;
unsigned long timer_event;
unsigned long state;

void setup()
{
    #if(!mode_work)//arduino
        Serial.begin(9600);
        Serial.println(F("start"));
        int val = freeRAM();
        Serial.println(freeRAM());
    #endif
    
// flow2 = new Flow(10,11,12,13);
    // flow2->init();
    // Flow flow1(10,11,12,13);
// Magistral mag1(0, PWM,10,11,12, dalg);//149
// dalg = setup_alg_magistral(4);//168
    flow.init();
    display.init();
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
        Serial.println(F("==="));
        Serial.println(val-freeRAM());
    #endif
    // act.init();
    // act2.init();
    // act3.init();
    // mag.init();
    // mag1.init();
    // mag2.init();
    // mag3.init();
    timer_delay_loop = millis();
    timer_event = millis();
    state = 0;
    // t1.restart();
        d_print(F("Staart"));
}

void loop()
{
    if(millis() - timer_delay_loop >= 100){

    // println(freeRAM());
    // t1.update();
    // if(sig_end.isTrueAndNotCheked()){
    //     d_print(F("Stoop");
    // }
        timer_delay_loop = millis();
    // flow2->update();
    display.update();
        flow.update();
    // mag1.update();
    // mag2.update();
    // mag3.update();
        // act->update();
        // act2.update();
        // act3.update();
        // d_print(F("act3(F(");
        // d_print(millis());
        // d_print(F(") status: ");
        // d_println(act3.getStatus());
        // d_print(F("act_2(F(");
        // d_print(millis());
        // d_print(F(") status: ");
        // d_println(act2.getStatus());
    }
    // if(millis() - timer_event >= 7000 && state == 0) {
    //     timer_event = millis();

    // // flow.start_solo();
    // flow.start(state_Flow::do_trio);
    //     // mag.start();
    //     // act->open();
    // // mag1.start();
    // // mag2.start();
    // // mag3.start();
    //     // act.open();
    //     // act2.open();
    //     // act3.open();
    //     ++state;
    // }
    // if(millis() - timer_event >= 10000 && state == 1) {
    //     timer_event = millis();
    //     flow.stop();
    //     // mag.stop();
    //     // act->close();
    //     // // act.close();
    //     // act2.close();
    //     // act3.close();
    //     ++state;
    // }
    //  if(millis() - timer_event >= 50000 && state == 2) {
    //     timer_event = millis();
    // flow.start(state_Flow::do_trio);
    //     // mag.start();
    //     // // act.close();
    //     // act2.close();
    //     // act3.close();
    //     ++state;
    // }
    // // if(millis() - timer_event >= 5000 && state == 1){
    // //     timer_event = millis();
    // //     act2.open();
    // //     ++state;
    // // }
    // // if(millis() - timer_event >= 5000 && state == 2){
    // //     timer_event = millis();
    // //     act3.open();
    // //     ++state;
    // // }
    // // if(millis() - timer_event >= 500){
    // //     act.close();
    // // }
}




