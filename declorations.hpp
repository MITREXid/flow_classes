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
    #define print(val) std::cout<<val
    #define println(val) std::cout<<val<<std::endl
    #define start_def ;
    unsigned long millis(){
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - time_global_start).count();}


    #define pinMode(a,b)  print("pinMode(");\
                           print(a);\
                           print(", ");\
                           print(b);\
                           println(")");
    #define OUTPUT "OUTPUT"
    #define INPUT "INPUT"
    #define HIGH "HIGH"
    #define LOW "LOW"
    #define A1 15
#else//arduino
    #define start_def 
    #define print(val) Serial.print(val);//std::cout<<val
    #define println(val) Serial.println(val);//std::cout<<val
#endif

#endif // DECLORATIONS_HPP
