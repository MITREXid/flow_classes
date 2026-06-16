#ifndef MY110_HPP
#define MY110_HPP
#include "universal_object.hpp"
#include "signal.h"
#include "flow.hpp"
#include "config.h"
#include "other_components.hpp"

#if !mode_work
    #include <SoftwareSerial.h>
    #include <ModbusMaster.h>
#endif

enum class state_MY110{
    working,
    stoped
};

class MY110 : public Universal_object<state_MY110>{
    private:
        SoftwareSerial &rs485;
        ModbusMaster node;
        uint32_t mask_pins_curr = 0;
        uint32_t mask_pins_last = 0;
        uint32_t mask_pins_default = 0;
        struct struct_in_work{
            int8_t id_clap = -1;//-1 свободен
            int8_t id_clap_prev = false;
            int8_t id_act = -1;//-1 свободен
            int8_t id_act_prev = false;
            int8_t id_ball = -1;//-1 свободен
            int8_t id_ball_prev = false;
        }in_work;
        struct struct_pointers_to_components{
            Actuator * actuator[kol_all_mag]; 
            Clapan * clapan[kol_all_mag]; 
            Ball_cran * ball_cran[kol_all_mag]; 
        }pointers_to_components;
    public:
        
        

        MY110(SoftwareSerial &rs485_):
        rs485(rs485_)
        {
            setStatus(state_MY110::working);
        }

        void init(){
            node.begin(16, rs485);
            node.preTransmission(preTransmission);
            node.postTransmission(postTransmission);

        }
        void update(){
            logic();
        }
        

    private:


        void logic(){
            in_work.id_act = get_curr_act();
            if(in_work.id_act != in_work.id_act_prev){
                turn_to_default_act();
                turn_act(in_work.id_act);
            }

            in_work.id_clap = get_curr_clap();
            if(in_work.id_clap != in_work.id_clap_prev){
                turn_to_default_clap();
                turn_clap(in_work.id_clap);
            }

            in_work.id_ball = get_curr_ball();
            if(in_work.id_ball != in_work.id_ball_prev){
                turn_to_default_ball();
                turn_ball(in_work.id_ball);
            }
            

        }


        void turn_to_default_act(){
            mask_pins_curr = 0;//тут обнуляем пины актуатора(не так)
        }

        void turn_to_default_clap(){
            mask_pins_curr = 0;//тут обнуляем пины актуатора(не так)
        }

        void turn_to_default_ball(){
            mask_pins_curr = 0;//тут обнуляем пины актуатора(не так)
        }

        void turn_act(int8_t id_act){
            if(pointers_to_components.actuator[id_act]->getStatus() == state_Component::going_close){
                pointers_to_components.actuator[id_act]->close();// с аргументом конечного времени
            }
            if(pointers_to_components.actuator[id_act]->getStatus() == state_Component::going_open){
                pointers_to_components.actuator[id_act]->open();// с аргументом конечного времени

            }
            in_work.id_act_prev = id_act;
        }

        void turn_clap(int8_t id_clap){
            if(pointers_to_components.actuator[id_clap]->getStatus() == state_Component::going_close){
                pointers_to_components.actuator[id_clap]->close();// с аргументом конечного времени
            }
            if(pointers_to_components.actuator[id_clap]->getStatus() == state_Component::going_open){
                pointers_to_components.actuator[id_clap]->open();// с аргументом конечного времени

            }
            in_work.id_clap_prev = id_clap;
        }

        void turn_ball(int8_t id_ball){
            if(pointers_to_components.actuator[id_ball]->getStatus() == state_Component::going_close){
                pointers_to_components.actuator[id_ball]->close();// с аргументом конечного времени
            }
            if(pointers_to_components.actuator[id_ball]->getStatus() == state_Component::going_open){
                pointers_to_components.actuator[id_ball]->open();// с аргументом конечного времени

            }
            in_work.id_ball_prev = id_ball;
        }


        int8_t get_curr_act(){
            // if( in_work.id_act != -1){ return in_work.id_act;}
            for(int i = 0; i < kol_all_mag; ++i){
                if(pointers_to_components.actuator[i]->getStatus() == state_Component::going_close ||
                pointers_to_components.actuator[i]->getStatus() == state_Component::going_open){
                    return i;
                }
            }
            return -1;
        }
        int8_t get_curr_clap(){
            // if( in_work.id_clap == -1) {return -1;}
            for(int i = 0; i < kol_all_mag; ++i){
                if(pointers_to_components.clapan[i]->getStatus() == state_Component::going_close ||
                pointers_to_components.clapan[i]->getStatus() == state_Component::going_open){
                    return i;
                }
            }
            return -1;
        }
        int8_t get_curr_ball(){
            // if( in_work.id_ball == -1) {return -1;}
            for(int i = 0; i < kol_all_mag; ++i){
                if(pointers_to_components.ball_cran[i]->getStatus() == state_Component::going_close ||
                pointers_to_components.ball_cran[i]->getStatus() == state_Component::going_open){
                    return i;
                }
            }
            return -1;
        }

};

#endif // MY110_HPP
