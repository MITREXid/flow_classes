#ifndef DECLORATIONS_HPP
#define DECLORATIONS_HPP



#define mode_work 0 //1- c++, 0 - arduino


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
#else//arduino
    #define start_def 
    #define print(val) Serial.print(val);//std::cout<<val
    #define println(val) Serial.println(val);//std::cout<<val
#endif

#endif // DECLORATIONS_HPP
