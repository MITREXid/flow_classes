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
    //                        d_print(b);\
    //                        d_println(")");
    #define pinMode(a,b) ;//чтоб не писало ни сего
    #define digitalWrite(a,b);

    #define OUTPUT "OUTPUT"
    #define INPUT "INPUT"
    #define HIGH "HIGH"
    #define LOW "LOW"
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
      uint8_t peek(){}
      void readBytes(char *buf, int size){}
      uint8_t available(){}
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
    } Serial;
    
    inline void delayMicroseconds(uint32_t val){}

    class SoftwareSerial :public Serial_{
      public:
      SoftwareSerial(uint8_t rx, uint8_t tx){}
      void begin(uint32_t bod){}
    };

    class ModbusMaster{
      public:
       void begin(uint32_t addr, SoftwareSerial &bod){}
       void preTransmission( void (a)() ){}
       void postTransmission( void (a)() ){}
       void writeSingleRegister(uint32_t reg, uint32_t val){}
    };
#else//arduino
    #include <Arduino.h>
    #define start_def 
    #define d_print(val) Serial.print(val);//std::cout<<val
    #define d_println(val) Serial.println(val);//std::cout<<val
    // #define print(val) Serial.print(val);//std::cout<<val
    // #define println(val) Serial.println(val);//std::cout<<val

    
inline int freeRAM() {
  extern int __heap_start, *__brkval;
  int v;
  return (int) &v - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
}

#endif

// // Базовый класс для любого вызываемого объекта
// struct CallableBase {
//     virtual ~CallableBase() = default;
//     virtual void invoke() = 0;
// };

// // Шаблонный класс для хранения конкретного вызываемого объекта
// template<typename T>
// struct Callable : CallableBase {
//     T func;
    
//     Callable(T f) : func(f) {}
//     void invoke() override { func(); }
// };
//  // Вспомогательный метод для безопасного удаления
// inline void deleteIfNotNull(CallableBase* &ptr) {
//     if (ptr != nullptr) {
//         delete ptr;
//         ptr = nullptr;
//     }
// }


// // Используйте этот макрос для большинства случаев
// #define DEFINE_ARDUINO_CALLBACK(name, ret, ...) \
//     typedef ret (*name##_t)(__VA_ARGS__); \
//     struct name { \
//         name##_t func; \
//         constexpr name() : func(nullptr) {} \
//         constexpr name(name##_t f) : func(f) {} \
//         template<typename F> \
//         constexpr name(F f) : func(f) {} \
//         constexpr ret operator()(__VA_ARGS__ args) const { \
//             return func ? func(args) : (ret)0; \
//         } \
//         constexpr operator bool() const { return func != nullptr; } \
//         constexpr static uint8_t memory() { return sizeof(name); } \
//     }


#endif // DECLORATIONS_HPP
