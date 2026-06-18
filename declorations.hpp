#ifndef DECLORATIONS_HPP
#define DECLORATIONS_HPP

#ifdef ARDUINO
  #define mode_work 0
#else
    
  #define mode_work 1
#endif

#if(mode_work)//c++
    #include <iostream>
    #include <chrono>
    std::chrono::time_point<std::chrono::system_clock> time_global_start;
    #define d_print(val) std::cout<<val
    #define d_println(val) std::cout<<val<<std::endl
    #define start_def ;
    unsigned long millis(){
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - time_global_start).count();}
    


    // #define pinMode(a,b)  d_print("pinMode(");\
    //                        d_print((int)a);\
    //                        d_print(", ");\
                          //  d_print(b==OUTPUT?"OUTPUT":"INPUT"));\
    //                        d_println(")");

    #define pinMode(a,b) static_assert((b==LOW||b==HIGH)?false:true, "Invalid pin mode")
    #define digitalWrite(a,b) d_print("digitalWrite(");\
                            d_print((int)a);\
                            d_print(", ");\
                            d_print(((int)b==HIGH?"HIGH":"LOW"));\
                            d_println(")")

    // #define OUTPUT "OUTPUT"
    // #define INPUT "INPUT"
    // #define HIGH "HIGH"
    // #define LOW "LOW"
    #define INPUT   0
    #define OUTPUT  1
    #define LOW     2
    #define HIGH    3
    #define A0 16
    #define A1 17
    #define A2 18
    #define A3 19
    #define A4 20
    #define A5 21
    #define A6 22
    #define A7 23
    // #define d_print(val) std::cout<</*"(F("<<millis()<<")"<<*/val
    // #define d_println(val) std::cout<</*"(F("<<millis()<<")"<<*/val<<std::endl
    // #define print(val) std::cout<<"("<<millis()<<")"<<val
    // #define println(val) std::cout<<"("<<millis()<<")"<<val<<std::endl
    #define F(val) val

    #define String std::string

    inline int freeRAM() {
        return -1;
        // extern int __heap_start, *__brkval;
        // int v;
        // return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
    }
    class Serial_{
      public:
      uint8_t peek(){ return 0;}
      void readBytes(char *buf, int size){}
      void begin(uint32_t bod){}
      char read(){return 0;}
      uint8_t available(){return 0;}
      void print(std::string val){d_print("(Serial.print) = ");d_print(val);}
      void write(char val[], uint8_t size){
        d_print("(Serial.write) = ");
        for(int i =0; i<size;++i){
          d_print((int)val[i]);
        }
      }
      void write(uint8_t val[], uint8_t size){
        d_print("(Serial.write) = ");
        for(int i =0; i<size;++i){
          d_print((int)val[i]);
        }
      }
      void println(std::string val){d_print("(Serial.println) = ");d_println(val);}
    };
    
    inline void delayMicroseconds(uint32_t val){}

    class SoftwareSerial :public Serial_{
      public:
      SoftwareSerial(uint8_t rx, uint8_t tx){}
    };

    class ModbusMaster{
      public:
       void begin(uint32_t addr, SoftwareSerial &bod){}
       void preTransmission( void (a)() ){}
       void postTransmission( void (a)() ){}
       void writeSingleRegister(uint32_t reg, uint32_t val){}
        void setTransmitBuffer(uint8_t index, uint16_t val){}
        uint8_t writeMultipleRegisters(uint16_t reg, uint16_t num_regs){return 0;}
    };

    Serial_ Serial;
    Serial_ Serial2;
    Serial_ Serial3;
#else//arduino
    #include <Arduino.h>
    #define start_def 
    #define d_print(val) Serial.print(val);//std::cout<<val
    #define d_println(val) Serial.println(val);//std::cout<<val
    #define Ser_PC Serial
    // #define print(val) Serial.print(val);//std::cout<<val
    // #define println(val) Serial.println(val);//std::cout<<val

    
inline int freeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

#endif

inline void time_break(uint32_t t){
    uint32_t time_init = millis();
    while(millis() - time_init < t){}
}
char* to_binary_string(uint32_t value, char* buffer);
// Установить бит в 1
uint32_t set_bit_1(uint32_t value, uint8_t bit_pos) {
  // char buffer[32];
  // d_print(F("set_bit_1: ")); 
  // d_println(to_binary_string(value, buffer));
    return value | (1U << bit_pos);
}

// Установить бит в 0 (сбросить)
uint32_t set_bit_0(uint32_t value, uint8_t bit_pos) {
  // char buffer[32];
  // d_print(F("set_bit_0, bit = "));
  // d_print((int)bit_pos);
  // d_print(F(" : "));
  // d_println(to_binary_string(value, buffer));
    return value & ~(1U << bit_pos);
}

// Инвертировать бит (0 -> 1, 1 -> 0)
uint32_t toggle_bit(uint32_t value, uint8_t bit_pos) {
  //  char buffer[32];
  // d_print(F("toggle_bit: "));
  // d_println(to_binary_string(value, buffer));
    return value ^ (1U << bit_pos);
}

char* to_binary_string(uint32_t value, char* buffer) {
    for (int i = 31; i >= 0; i--) {
        // Если бит равен 1, пишем символ '1', иначе '0'
        buffer[31 - i] = (value & (1U << i)) ? '1' : '0';
    }
    buffer[32] = '\0'; // Обязательный нуль-терминатор в конце строки
    return buffer;
}

#endif // DECLORATIONS_HPP
