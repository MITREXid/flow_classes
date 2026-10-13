#ifndef DYVKA_HPP
#define DYVKA_HPP
#include "universal_object.hpp"
#include "flow.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "settings.hpp"

    #include <SoftwareSerial.h>
    #include "ModbusMaster/ModbusMaster.h"

enum class state_Dyvka{
    do_if_open,
    do_anyway,
    no_air
};


class Dyvka : public Universal_object<state_Dyvka>{
    private:
        SoftwareSerial &rs485;
        ModbusMaster node;
        Signal<> (&sig_ball_not_close)[kol_all_mag];
        uint16_t goal_freq_chastot = 3500;//целевая частота для чатотника(умноженная 100) 
        // uint16_t curr_freq_chastot = 0;//текущая частота для чатотника(умноженная 100) 
        enum class State_air{on, off} state_air;
        //FIX: частотник часто не отвечает на "пуск" сразу после "стоп" (журнал Караганды:
        //продувка магистралей 3,2,1 шла без воздуха). Команду повторяем, пока не ответит.
        bool air_confirmed = true;//частотник подтвердил текущую команду
        uint8_t air_retries = 0;
        uint32_t t_air_cmd = 0;
        //плавный разгон: частоту поднимаем ступенями каждые T_RAMP_STEP мс
        uint8_t ramp_s = 0;//время разгона от 0 до заданной частоты, с (0 - сразу заданная)
        uint16_t ramp_cur = 0;//текущее задание частоты x100 во время разгона
        uint32_t t_ramp = 0;
        static constexpr uint16_t RAMP_START = 1000;//с какой частоты начинаем, x100 (10 Гц)
        static constexpr uint32_t T_RAMP_STEP = 200;
        //плавная остановка: частоту снижаем ступенями до RAMP_START, потом "стоп"
        uint8_t stop_s = 0;//время торможения от заданной частоты до 10 Гц, с (0 - сразу стоп)
        bool stopping = false;
        //давление
        int16_t pressure_x10 = INT16_MIN;
        uint32_t t_pressure = 0;
    public:
        Dyvka(SoftwareSerial &rs485_, Signal<> (&sig)[kol_all_mag]):
        sig_ball_not_close{sig},
        rs485(rs485_)
        {
            setStatus(state_Dyvka::no_air);
        }

        void init(){
            node.begin(1, rs485);
            node.preTransmission(preTransmission);
            node.postTransmission(postTransmission);
            air_off();
        }
        void update(){
            logic();
            if(!air_confirmed && millis() - t_air_cmd >= T_DYVKA_RETRY){
                send_air();
            }
            ramp_step();
            stop_step();
            #if PRESSURE_SOURCE == 1
            if(millis() - t_pressure >= T_PRESSURE_READ){
                t_pressure = millis();
                uint8_t r = node.readHoldingRegisters(PRESSURE_REG, 1);
                if(r == node.ku8MBSuccess){
                    float kpa = ((int16_t)node.getResponseBuffer(0) + PRESSURE_OFFSET) * PRESSURE_SCALE;
                    pressure_x10 = (int16_t)(kpa * 10);
                }else{
                    pressure_x10 = INT16_MIN;
                }
            }
            #endif
        }

        int16_t get_pressure_x10(){
            return pressure_x10;
        }

        //отладка: вывести регистры частотника в монитор порта
        void debug_read(uint16_t reg, uint8_t count){
            if(count < 1){ count = 1; }
            if(count > 16){ count = 16; }
            uint8_t r = node.readHoldingRegisters(reg, count);
            if(r != node.ku8MBSuccess){
                Serial.print(F("частотник не ответил, код "));
                Serial.println(r);
                return;
            }
            for(uint8_t i = 0; i < count; ++i){
                Serial.print(F("h"));
                Serial.print(reg + i, HEX);
                Serial.print(F(" = "));
                Serial.println((int16_t)node.getResponseBuffer(i));
            }
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
            if(frec > (uint16_t)SET_HZ_MAX * 100){
                frec = (uint16_t)SET_HZ_MAX * 100;
            }
            goal_freq_chastot = frec;
            //если воздух уже идёт (комплекс: триплет -> соло) - к новой частоте тоже плавно
            if(state_air == State_air::on && air_confirmed && ramp_cur > goal_freq_chastot){
                ramp_cur = goal_freq_chastot;
                node.writeSingleRegister(0x3000, ramp_cur);
            }
        }

        void set_ramp(uint8_t seconds){
            ramp_s = seconds;
        }

        void set_stop_ramp(uint8_t seconds){
            stop_s = seconds;
        }

    private:

        void air_on(){
            if(stopping){//ещё тормозим - частотник крутится, снова разгоняемся к цели без "пуска"
                stopping = false;
                state_air = State_air::on;
                air_confirmed = true;
                t_ramp = millis();
                if(ramp_s == 0){
                    ramp_cur = goal_freq_chastot;
                    node.writeSingleRegister(0x3000, ramp_cur);
                }
                return;
            }
            if(state_air == State_air::on){return;}
            d_println(F("Dyet"));
            state_air = State_air::on;
            air_retries = 0;
            send_air();
        }
        void air_off(){
            if(state_air == State_air::off){return;}
            d_println(F("Ne Dyet"));
            state_air = State_air::off;
            air_retries = 0;
            if(stop_s > 0 && air_confirmed && ramp_cur > RAMP_START){
                stopping = true;//stop_step() снизит частоту и потом отправит "стоп"
                t_ramp = millis();
                return;
            }
            send_air();
        }

        //Отправить частотнику текущую команду (пуск с частотой или стоп).
        //Журнал: событие пишется, когда частотник ответил; ошибка - только первая в серии.
        void send_air(){
            uint8_t r;
            if(state_air == State_air::on){
                //с разгоном стартуем с RAMP_START, дальше ramp_step() поднимает до goal
                ramp_cur = (ramp_s > 0 && goal_freq_chastot > RAMP_START) ? RAMP_START : goal_freq_chastot;
                t_ramp = millis();
                r = node.writeSingleRegister(0x3000, ramp_cur);//настроили частоту
                if(r == 0){
                    time_break(10);
                    r = node.writeSingleRegister(0x2000, 1);    // Пуск
                }
            }else{
                r = node.writeSingleRegister(0x2000, 5);    // Стоп
            }
            t_air_cmd = millis();
            air_confirmed = (r == 0);
            if(air_confirmed){
                uint32_t data = (state_air == State_air::on ? goal_freq_chastot : 0) | ((uint32_t)air_retries << 24);
                log_event(LOG_AIR, state_air == State_air::on ? 1 : 0, data);
            }else{
                if(air_retries == 0){
                    log_event(LOG_ERR_AIR, r, 0x2000);
                }
                if(air_retries < 255){ ++air_retries; }
            }
        }

        //Ступень разгона: +goal/(ramp_s*1000/T_RAMP_STEP) каждые T_RAMP_STEP мс
        void ramp_step(){
            if(state_air != State_air::on || !air_confirmed || ramp_cur >= goal_freq_chastot){
                return;
            }
            if(millis() - t_ramp < T_RAMP_STEP){
                return;
            }
            uint32_t steps = (uint32_t)ramp_s * 1000 / T_RAMP_STEP;
            uint16_t inc = steps ? (uint16_t)(goal_freq_chastot / steps) : goal_freq_chastot;
            if(inc < 1){ inc = 1; }
            uint16_t next = ramp_cur + inc;
            if(next > goal_freq_chastot || next < ramp_cur){ next = goal_freq_chastot; }
            t_ramp = millis();
            if(node.writeSingleRegister(0x3000, next) == 0){
                ramp_cur = next;
                d_print(F("DYVKA ramp "));
                d_println(ramp_cur);
            }//не ответил - повторим ту же ступень через T_RAMP_STEP
        }

        //Ступень торможения: -goal/(stop_s*1000/T_RAMP_STEP) каждые T_RAMP_STEP мс, у 10 Гц - "стоп"
        void stop_step(){
            if(!stopping || millis() - t_ramp < T_RAMP_STEP){
                return;
            }
            t_ramp = millis();
            uint32_t steps = (uint32_t)stop_s * 1000 / T_RAMP_STEP;
            uint16_t dec = steps ? (uint16_t)(goal_freq_chastot / steps) : goal_freq_chastot;
            if(dec < 1){ dec = 1; }
            if(ramp_cur <= RAMP_START + dec){
                stopping = false;
                send_air();//стоп (state_air уже off)
                return;
            }
            if(node.writeSingleRegister(0x3000, ramp_cur - dec) == 0){
                ramp_cur -= dec;
                d_print(F("DYVKA stop ramp "));
                d_println(ramp_cur);
            }//не ответил - повторим ту же ступень
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
