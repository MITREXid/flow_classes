#ifndef DYVKA_HPP
#define DYVKA_HPP
#include "universal_object.hpp"
#include "signal.h"
#include "flow.hpp"
#include "config.hpp"

#if !mode_work
    #include <SoftwareSerial.h>
    #include "ModbusMaster/ModbusMaster.h"
#endif

enum class state_Dyvka{
    do_if_open,
    do_anyway,
    no_air
};


class Dyvka : public Universal_object<state_Dyvka>{
    private:
        int8_t control_pin = DYVKA_CONTROL_PIN;
        Signal<> (&sig_ball_not_close)[kol_all_mag];
        uint16_t goal_freq_chastot = 0;//целевая частота для чатотника(умноженная 100) 
        // uint16_t curr_freq_chastot = 0;//текущая частота для чатотника(умноженная 100) 
        enum class State_air{on, off} state_air = State_air::on;//для того чтобы air_off стработало
    public:
        
        Dyvka(SoftwareSerial &rs485_, Signal<> (&sig)[kol_all_mag]):
        sig_ball_not_close{sig}
        {
            setStatus(state_Dyvka::no_air);
        }

        void init(){
            pinMode(control_pin, OUTPUT);
        //    analogWrite(control_pin, 0);
            air_off();
        }
        void update(){
            logic();
        }
        void start_anyway(){
            air_on();
            setStatus(state_Dyvka::do_anyway);
        }
        void start_if_open(){
            setStatus(state_Dyvka::do_if_open);
        }
        void stop(){
            air_off();
            setStatus(state_Dyvka::no_air);
        }
        void set_goal_frec(uint16_t frec, bool set_now = false){//нужно для случаев когда air_on или off не запускаются но надо поменять мощность(комплекс переход от триплета к соло)
            if(frec>10000){
                frec = 10000;
            }
            goal_freq_chastot = frec;
            if(set_now){
                analogWrite(control_pin, convert_to_255_range(goal_freq_chastot));
            }
            d_print(F("DYVKA change val ("));
            d_print(convert_to_255_range(goal_freq_chastot));
            d_println(F(")"));
        }

        uint16_t convert_to_255_range(uint16_t frec){
            float f_frec = (float)frec/100.0 * 255;
            return (uint16_t)f_frec;
        }
        uint16_t get_curr_goal_frec(){
            return goal_freq_chastot;
        }

         void air_on(){
            if(state_air == State_air::on){return;}
            d_print(F("DYVKA on ("));
            d_print(convert_to_255_range(goal_freq_chastot));
            d_println(F(")"));
            state_air = State_air::on;
           analogWrite(control_pin, convert_to_255_range(goal_freq_chastot));
            //нужна реализация
        }
        void air_off(){
            if(state_air == State_air::off){return;}
            d_println(F("DYVKA: off (0)"));
            state_air = State_air::off;
           analogWrite(control_pin, 0);
        }

    private:

       

        void logic(){
            switch(getStatus()){
                case state_Dyvka::do_anyway:
                    if(state_air == State_air::off){
                        air_on();
                    }
                    return;
                case state_Dyvka::do_if_open:
                    for(uint8_t i = 0; i <kol_all_mag;++i){
                        if(sig_ball_not_close[i].getState()){
                            air_on();
                            return;
                        }
                    }
                    air_off();//Если все закрыты
                    return;
                case state_Dyvka::no_air:
                    if(state_air == State_air::on){
                        air_off();
                    }
                    return;
            }
        }

};

#endif // DYVKA_HPP
