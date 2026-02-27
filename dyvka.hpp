#ifndef DYVKA_HPP
#define DYVKA_HPP
#include "universal_object.hpp"
#include "signal.h"
#include "flow.hpp"

enum class state_Dyvka{
    do_if_open,
    do_anyway,
    no_air
};


class Dyvka : public Universal_object<state_Dyvka>{
    private:
        uint8_t pin_DE_RE = -1;
        uint8_t RX = -1;
        uint8_t TX = -1;
        Signal<> (&sig_ball_not_close)[kol_all_mag];
        uint16_t goal_freq_chastot = 5000;//целевая частота для чатотника(умноженная 100) 
        // uint16_t curr_freq_chastot = 0;//текущая частота для чатотника(умноженная 100) 
        enum class State_air{on, off} state_air;
    public:
        Dyvka(uint8_t pin_DE_RE_, uint8_t RX_, uint8_t TX_, Signal<> (&sig)[kol_all_mag]):
        pin_DE_RE{pin_DE_RE_},
        RX{RX_},
        TX{TX_},
        sig_ball_not_close{sig}
        {
            setStatus(state_Dyvka::no_air);
        }
        void init(){
            //инициализвция SoftwareSerial
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
        void set_goal_frec(uint16_t frec){
            if(frec>5000){
                goal_freq_chastot = 5000;
            }
            goal_freq_chastot = frec;
        }

    private:

        void air_on(){
            if(state_air == State_air::on){return;}
            d_println(F("Dyet"));
            state_air = State_air::on;
            //нужна реализация
        }
        void air_off(){
            if(state_air == State_air::off){return;}
            d_println(F("Ne Dyet"));
            state_air = State_air::off;
            //нужна реализация
        }

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
