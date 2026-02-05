#ifndef TIMER_HPP
#define TIMER_HPP

#include "declorations.hpp"
#include "universal_object.hpp"



enum state_Timer {stoped = 0, going = 1};
template<
    typename type_Data_start,
    typename type_Data_end,
    typename StartFunc = void(*)(type_Data_start),
    typename EndFunc = void(*)(type_Data_end)
   >
class Timer : public Universal_object<state_Timer> {
private:
    StartFunc func_start = nullptr;
    EndFunc func_end = nullptr;
    type_Data_start start_Data;
    type_Data_end end_Data;
    unsigned long time = 0;
    unsigned long timeStart = 0;
    
public:
    Timer() = default;
    
    Timer(
            unsigned long in_time,
            type_Data_start *start_D = nullptr,
            type_Data_end *end_D = nullptr,
            StartFunc start = nullptr,
            EndFunc end = nullptr
        ) 
        : time(in_time), func_start(start), func_end(end), start_Data(start_D), end_Data(end_D)  {}
    
    void update() {
        if(getStatus() == state_Timer::stoped) return;
        if(millis() - timeStart >= time) {
            setStatus(state_Timer::stoped);
            if(func_end) func_end(end_Data);
        }
    }
    
    void restart() {
        setStatus(state_Timer::going);
        timeStart = millis();
        if(func_start) func_start(start_Data);
    }
    
    void setFuncStart(StartFunc f) { func_start = f; }
    void setFuncEnd(EndFunc f) { func_end = f; }
    void setDataStart(type_Data_start d) { start_Data = d; }
    void setDataEnd(type_Data_end d) { end_Data = d; }
      
    //останавливает таймер, не вызывает функцию EndFunc
    void stop() {
        setStatus(state_Timer::stoped);
    }

    unsigned long getTime() const { return time; }
    void setTime(unsigned long time_) { time = time_; }
    
};

#endif // TIMER_HPP