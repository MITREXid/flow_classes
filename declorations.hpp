#ifndef DECLORATIONS_HPP
#define DECLORATIONS_HPP

// Общие объявления прошивки. Только Arduino API: для сборки на ПК
// заглушки лежат отдельно, в host/include (подменяют Arduino.h и др.)
#include <Arduino.h>

#define start_def
#define d_print(val) Serial.print(val);
#define d_println(val) Serial.println(val);
#define Ser_PC Serial

// Свободная RAM (AVR): расстояние между вершиной кучи и стеком
inline int freeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  // intptr_t: на AVR это int (16 бит), на ПК - целое размером с указатель
  return (int)((intptr_t)&v - (__brkval == 0 ? (intptr_t)&__heap_start : (intptr_t)__brkval));
}

inline void time_break(uint32_t t){
    uint32_t time_init = millis();
    while(millis() - time_init < t){}
}
inline char* to_binary_string(uint32_t value, char* buffer);
// Установить бит в 1
inline uint32_t set_bit_1(uint32_t value, uint8_t bit_pos) {
  // char buffer[32];
  // d_print(F("set_bit_1: ")); 
  // d_println(to_binary_string(value, buffer));
    return value | (1UL << bit_pos);
}

// Установить бит в 0 (сбросить)
inline uint32_t set_bit_0(uint32_t value, uint8_t bit_pos) {
  // char buffer[32];
  // d_print(F("set_bit_0, bit = "));
  // d_print((int)bit_pos);
  // d_print(F(" : "));
  // d_println(to_binary_string(value, buffer));
    return value & ~(1UL << bit_pos);
}

// Инвертировать бит (0 -> 1, 1 -> 0)
inline uint32_t toggle_bit(uint32_t value, uint8_t bit_pos) {
  //  char buffer[32];
  // d_print(F("toggle_bit: "));
  // d_println(to_binary_string(value, buffer));
    return value ^ (1UL << bit_pos);
}

inline char* to_binary_string(uint32_t value, char* buffer) {
    for (int i = 31; i >= 0; i--) {
        // Если бит равен 1, пишем символ '1', иначе '0'
        buffer[31 - i] = (value & (1UL << i)) ? '1' : '0';
    }
    buffer[32] = '\0'; // Обязательный нуль-терминатор в конце строки
    return buffer;
}

//Проверяет бит
inline bool check_bit(uint32_t value, int8_t bit_index) {
    return value & (1UL << bit_index);
}

#endif // DECLORATIONS_HPP
