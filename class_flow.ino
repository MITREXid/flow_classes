#include "declorations.hpp"
#include "timer.hpp" 
#include "component.hpp" 
#include "other_components.hpp"
#include "component.hpp"
#include "magistral.hpp"
#include "flow.hpp"
#include "display.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "settings.hpp"

// FIX: сторожевой таймер. Если Arduino зависнет, она перезагрузится, а MY110::init()
// выключит все выходы МУ. 0 - отключить (если старый загрузчик Mega зацикливается после сброса WDT)
#define USE_WATCHDOG 1
#if USE_WATCHDOG
    #include <avr/wdt.h>
#endif

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

// Команды из монитора порта: журнал (log, err, clear, help) и отладочные цифры 0-5
void serial_command(char c){
    log_cmd_source = 1;//USB
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

// cfg - все настройки по программам; cfg <ключ> <значение> - изменить; cfg default - по умолчанию; cfg times
void cfg_command(char *args){
    while(*args == ' '){ ++args; }
    if(*args == 0){
        settings_print();
        return;
    }
    char name[10] = {0};
    uint8_t n = 0;
    while(*args && *args != ' ' && n < sizeof(name) - 1){ name[n++] = *args++; }
    if(strcmp(name, "times") == 0){
        flow.print_timing();
        return;
    }
    FlowSettings s = g_set;
    if(strcmp(name, "default") == 0){
        settings_default(s);
    }else{
        int8_t idx = -1;
        for(uint8_t i = 0; i < SET_N; ++i){
            if(strcmp(name, SET_PARAMS[i].key) == 0){ idx = i; break; }
        }
        if(idx < 0){
            Serial.println(F("cfg: неизвестный ключ, список - cfg"));
            return;
        }
        float v = atof(args);
        if(SET_PARAMS[idx].dec){ v = v * 10 + 0.5; }
        set_ref(s, idx) = (uint8_t)(v > 255 ? 255 : (v < 0 ? 0 : v));
    }
    if(flow.change_settings(s)){
        settings_print();
    }else{
        Serial.println(F("Не принято: сначала остановите FLOW"));
    }
}

void serial_poll(){
    static char buf[32];
    static uint8_t len = 0;
    while(Ser_PC.available() > 0){
        char c = Ser_PC.read();
        //цифра без перевода строки - сразу команда (как было раньше)
        if(len == 0 && c >= '0' && c <= '5'){
            serial_command(c);
            continue;
        }
        if(c == '\r' || c == '\n'){
            buf[len] = 0;
            if(strncmp(buf, "cfg", 3) == 0){
                cfg_command(buf + 3);
            }else if(len > 4 && strncmp(buf, "mbr ", 4) == 0){
                //mbr <регистр hex> [кол-во] - читать регистры частотника
                char *end;
                uint16_t reg = (uint16_t)strtol(buf + 4, &end, 16);
                uint8_t cnt = (uint8_t)strtol(end, NULL, 10);
                flow.debug_read_vfd(reg, cnt ? cnt : 1);
            }else if(len > 0 && !log_command(buf)){
                Serial.println(F("Неизвестная команда. help - список команд"));
            }
            len = 0;
        }else if(len < sizeof(buf) - 1){
            buf[len++] = (c >= 'A' && c <= 'Z') ? c + 32 : c;
        }
    }
}

void setup()
{
    #if USE_WATCHDOG
        wdt_disable();
    #endif
        Serial.begin(1000000);
        d_println(F("start"));
        int val = freeRAM();
        d_println(freeRAM());
    
    log_begin();//запись "ЗАПУСК" в журнал (с причиной перезагрузки)
    display.init();
    flow.init();
    if(!settings_load(LOG_SETTINGS_BASE)){//загружает и значения по умолчанию
        d_println(F("Настройки не найдены в EEPROM - значения по умолчанию"));
    }
    flow.apply_settings();

        d_println(freeRAM());
        d_println(F("==="));
        d_println(val-freeRAM());
    timer_delay_loop = millis();
    timer_event = millis();
    state = 0;
    d_println(F("Staart"));
    #if USE_WATCHDOG
        wdt_enable(WDTO_2S);
    #endif
}

void loop()
{
    #if USE_WATCHDOG
        wdt_reset();
    #endif
    if(millis() - timer_delay_loop >= 0){
            serial_poll();
        timer_delay_loop = millis();
        display.update();
        flow.update();
        log_dump_allowed = flow.can_dump_log();
        log_write_step();//журнал: один байт EEPROM за проход

    }
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




