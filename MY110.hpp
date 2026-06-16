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
        uint32_t mask_pins_new = 0;
        uint32_t mask_pins_default = 0;
        
        //данные по одному компонентк(актуатор или клапан)
        int8_t comp_id_curr = -1;//-1 начало, застревает на компоненте требующем смены состояния(0-3 актуаторы, 4-7 клапаны)
        int8_t comp_id_prev = false;//нужно для того чтобы не застревать на одном компоненте, если он уже в процессе смены состояния
        state_Component comp_state_prev = state_Component::undef;// дас  возможность досрочно поменять на противоположное состояние(до завершения текузего процесса смены у того же компонента)

        struct struct_pointers_to_components{
            Actuator * actuator[kol_all_mag]; 
            Clapan * clapan[kol_all_mag]; 
            Ball_cran * ball_cran[kol_all_mag]; 
        }pointers_to_components;
    public:
        
        

        MY110(SoftwareSerial &rs485_, 
        Actuator *m1_act, Clapan *m1_clap, Ball_cran *m1_ball,
        Actuator *m2_act, Clapan *m2_clap, Ball_cran *m2_ball,
        Actuator *m3_act, Clapan *m3_clap, Ball_cran *m3_ball,
        Actuator *m4_act, Clapan *m4_clap, Ball_cran *m4_ball
        ):
        rs485(rs485_),
        pointers_to_components{
            .actuator{m1_act,m2_act,m3_act,m4_act},
            .clapan{m1_clap,m2_clap,m3_clap,m4_clap},
            .ball_cran{m1_ball,m2_ball,m3_ball,m4_ball}
        }
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
        // int8_t get_curr_id(){
        //     return comp_id_curr;
        // }
        int8_t get_curr_type(){//0 - актуатор, 1 - клапан
            if(comp_id_curr <0){
                d_println(F("ERROR!!! get_curr_type: comp_id_curr <0"));
                return 0;
            }else if(comp_id_curr < 4){
                return 0;//актуатор
            }else if(comp_id_curr < 8){
                return 1;//клапан
            }else{
                d_println(F("ERROR!!! get_curr_type: comp_id_curr >= 8"));
                return 0;
            }
        }
        int8_t next_id(){
            comp_id_curr++;
            if(comp_id_curr>7){
                comp_id_curr = 0;
            }
            return comp_id_curr;
        }
        int8_t prev_id(){
            comp_id_curr--;
            if(comp_id_curr<0){
                comp_id_curr = 7;
            }
            return comp_id_curr;
        }

    private:


        void logic(){
            if(comp_id_curr != comp_id_prev){
                if(get_curr_type() == 0){//актуатор
                    comp_state_prev = pointers_to_components.actuator[comp_id_curr]->getStatus();
                    turn_act(comp_id_curr, comp_state_prev);
                }else if(get_curr_type() == 1){//клапан
                    comp_state_prev = pointers_to_components.clapan[comp_id_curr-4]->getStatus();
                    turn_clap(comp_id_curr-4, comp_state_prev);
                }

            }else{

                if(get_curr_type() == 0){//актуато
                    if(comp_state_prev != pointers_to_components.actuator[comp_id_curr]->getStatus()){
                        comp_state_prev = pointers_to_components.actuator[comp_id_curr]->getStatus();
                        turn_act(comp_id_curr, comp_state_prev);
                    }else{
                        next_id();
                    }
                }else if(get_curr_type() == 1){//клапан
                    if(comp_state_prev != pointers_to_components.clapan[comp_id_curr-4]->getStatus()){
                        comp_state_prev = pointers_to_components.clapan[comp_id_curr-4]->getStatus();
                        turn_clap(comp_id_curr-4, comp_state_prev);
                    }else{
                        next_id();
                    }
                }

            }


            

        }

        void turn_clap(int8_t id_clap, state_Component state_prev){
            turn_to_default_act_and_clap();
            if(state_prev == state_Component::going_close){
                pointers_to_components.clapan[id_clap]->close(false);// с аргументом конечного времени
                int i = 0;
                while(pins_MY110_all_mags_act_clap_forward[id_clap][i] != -1){
                    mask_pins_new = set_bit_0(mask_pins_curr, pins_MY110_all_mags_act_clap_forward[id_clap+4][i]);
                    i++;
                }
            }
            if(state_prev == state_Component::going_open){
                pointers_to_components.clapan[id_clap]->open(false);// с аргументом конечного времени
                int i = 0;
                while(pins_MY110_all_mags_act_clap_revers[id_clap][i] != -1){
                    mask_pins_new = set_bit_0(mask_pins_curr, pins_MY110_all_mags_act_clap_revers[id_clap+4][i]);
                    i++;
                }
            }
            
            send_mask(mask_pins_new);
            comp_id_curr = id_clap;
        }

        void turn_act(int8_t id_act, state_Component state_prev){
            turn_to_default_act_and_clap();
            if(state_prev == state_Component::going_close){
                pointers_to_components.actuator[id_act]->close(false);// с аргументом конечного времени
                int i = 0;
                while(pins_MY110_all_mags_act_clap_forward[id_act][i] != -1){
                    mask_pins_new = set_bit_0(mask_pins_curr, pins_MY110_all_mags_act_clap_forward[id_act][i]);
                    i++;
                }
            }
            if(state_prev == state_Component::going_open){
                pointers_to_components.actuator[id_act]->open(false);// с аргументом конечного времени
                int i = 0;
                while(pins_MY110_all_mags_act_clap_revers[id_act][i] != -1){
                    mask_pins_new = set_bit_0(mask_pins_curr, pins_MY110_all_mags_act_clap_revers[id_act][i]);
                    i++;
                }
            }
            
            send_mask(mask_pins_new);
            comp_id_curr = id_act;
        }



        void turn_to_default_act_and_clap(){
            int i = 0;
            while(pins_MY110_clear_act_clap[i] != -1){
                mask_pins_new = set_bit_0(mask_pins_curr, pins_MY110_clear_act_clap[i]);
                i++;
            }
            send_mask(mask_pins_new);
        }

       void send_mask(uint16_t mask_){

            uint16_t reg97_mask = 0;//(Биты 1-16)
            uint16_t reg98_mask = 0;//(Биты 17-32)
            reg97_mask = mask_ & 0xFFFF; // Извлекаем младшие 16 бит
            reg98_mask = (mask_ >> 16) & 0xFFFF; // Извлекаем старшие 16 бит

            node.setTransmitBuffer(0, reg97_mask); // Значение для регистра 0x0061
            node.setTransmitBuffer(1, reg98_mask); // Значение для регистра 0x0062
            uint8_t result = node.writeMultipleRegisters(0x0061, 2);
            //можно добавить проверку result на успешность записи, если нужно
            mask_pins_curr = mask_;
        }

 

};

#endif // MY110_HPP
