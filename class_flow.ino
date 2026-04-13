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
// Shared_power_5V PWM(A1);
// Actuator *act;
// Ball_cran act3(state_Component::close, 12);
// Magistral mag(PWM,10,11,12, &mag_start_state);

#define rele_5 27//шаровые 27
#define rele_6 43//шаровые 43
#define rele_7 29//шаровые 29
#define rele_8 41//шаровые 41

#define rele_9 31//актуатор 31
#define rele_11 33//актуатор 33
#define rele_13 35//актуатор  35
#define rele_15 37//актуатор 37

//тут ошибка по схеме FLOW3_v2.2 другое(в физике пока норм) в комментах, то что должно быть по схеме
#define mod2_clap_rele_1_2  39//клапан 39
#define mod2_clap_rele_3_4 45//клапан 45
#define mod2_clap_rele_5_6 47//клапан 47
#define mod2_clap_rele_7_8 49//клапан 49

#define mod2_rele_15_power_clap 53// 53
#define rele_2_power_act 23// 23

#define pin_power_v12 rele_2_power_act
#define pin_power_v12_clapan mod2_rele_15_power_clap

Data_alg dalg;
Flow flow(
    //четвертая магистраль
    rele_15,//актуатор
    mod2_clap_rele_1_2,//клапан
    rele_8,//шаровый 
    //третья магистраль
    rele_13,//актуатор
    mod2_clap_rele_3_4,//клапан
    rele_7,//шаровый 
    //вторая магистраль
    rele_11,//актуатор
    mod2_clap_rele_5_6,//клапан
    rele_6,//шаровый 
    //первая магистраль(соло)
    rele_9,//актуатор
    mod2_clap_rele_7_8,//клапан
    rele_5,//шаровый 
    12,//пин RX(RO) для частоника
    10,//пин TX(DI) для частоника
    //пин RE/DE настраивается в файле dyvka.hpp (был D11)
    pin_power_v12,//пин открывающий реле на котором 12В
    pin_power_v12_clapan
);

Display display(flow);



unsigned long timer_delay_loop ;
unsigned long timer_event;
unsigned long state;

#define mag 0

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
    if(millis() - timer_delay_loop >= 100){
        timer_delay_loop = millis();
        display.update();
        flow.update();
    }
    #if(mode_work)//g++
        if(millis() - timer_event >= 15000 && state == 0) {
            timer_event = millis();
            // flow.start(state_Flow::do_solo);
            // flow.start(state_Flow::do_clearing);
            // flow.start(state_Flow::do_produvka);
            flow.start(state_Flow::do_trio);
            ++state;
        }
         if(millis() - timer_event >= 100 && state == 1) {
            timer_event = millis();
            flow.stop(); 
            ++state;
        }
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




