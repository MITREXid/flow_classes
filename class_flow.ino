#include "declorations.hpp"
#include "timer.hpp" 
#include "component.hpp" 
#include "other_components.hpp"
#include "component.hpp"
#include "magistral.hpp"
#include "flow.hpp"
#include "display.hpp"
#include "config.hpp"

Flow flow(
    RO_RS485,//пин RX(RO) для частоника
    DI_RS485//пин TX(DI) для частоника
    //пин RE/DE настраивается в файле dyvka.hpp (был D11)
);

// Data_alg dalg;
Display display(flow);



unsigned long timer_delay_loop ;
unsigned long timer_event;
unsigned long state;

// #define mag 0

void setup()
{
    #if(!mode_work)//arduino
        Serial.begin(9600);
        d_println(F("start"));
        int val = freeRAM();
        d_println(freeRAM());
    #endif
    
    display.init();
    flow.init();

    #if(!mode_work)//arduino
        d_println(freeRAM());
        d_println(F("==="));
        d_println(val-freeRAM());
    #endif
    timer_delay_loop = millis();
    timer_event = millis();
    state = 0;
    d_println(F("Staart"));
}

void loop()
{
    if(millis() - timer_delay_loop >= 10){
        #if(!mode_work)//g++
            if(Ser_PC.available() > 0){
                char c = Ser_PC.read();
                d_print("available: ");
                d_println(c);
                if(c == '1'){
                    flow.start(state_Flow::do_complex);
                }
                if(c == '2'){
                    flow.start(state_Flow::do_solo);
                }
                if(c == '3'){
                    flow.start(state_Flow::do_clearing);
                }
                if(c == '4'){
                    flow.start(state_Flow::do_triplet);
                }
                if(c == '5'){
                    flow.start(state_Flow::do_produvka);
                }
                if(c == '0'){
                    flow.stop();
                }
            }
        #endif
        timer_delay_loop = millis();
        display.update();
        flow.update();

    }
    #if(mode_work)//g++
        if(millis() - timer_event >= 18000 && state == 0) {
            timer_event = millis();
            // flow.start(state_Flow::do_solo);
            // flow.start(state_Flow::do_clearing);
            // flow.start(state_Flow::do_produvka);
            flow.start(state_Flow::do_complex);
            // flow.start(state_Flow::do_triplet);
            ++state;
        }
         if(millis() - timer_event >= 100000 && state == 1) {
            timer_event = millis();
            flow.stop(); 
            ++state;
        }
        // if(millis() - timer_event >= 15000 && state == 2) {
        // timer_event = millis();
        // flow.start(state_Flow::do_produvka);
        // ++state;
        // }
    #endif
    // if(millis() - timer_event >= 15000 && state == 0) {
    //     timer_event = millis();
    //     flow.start(state_Flow::do_solo);
    //     ++state;
    // }
    //  if(millis() - timer_event >= 100 && state == 1) {
    //     timer_event = millis();
    //     flow.stop(); 
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




