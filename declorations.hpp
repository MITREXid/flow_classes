#ifndef DECLORATIONS_HPP
#define DECLORATIONS_HPP



#define mode_work 1 //1- c++, 0 - arduino


#if(mode_work)//c++
    #include <iostream>
    #define print(val) std::cout<<val
    #define println(val) std::cout<<val<<std::endl
    #define start_def ;
    unsigned long millis(){return 1;}
#else//arduino
    #define start_def Serial.begin(9600);
    #define print(val) Serial.print(val);//std::cout<<val
    #define println(val) Serial.println(val);//std::cout<<val
#endif

#endif // DECLORATIONS_HPP
