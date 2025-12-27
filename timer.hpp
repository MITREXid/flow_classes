#ifndef TIMER_HPP
#define TIMER_HPP

#include "declorations.hpp"
#include "universal_object.hpp"

enum state_Timer {stoped = 0, going = 1};
class Timer: public Universal_object<state_Timer>{
    private:
    //указатель на ф-ию, запускается при прохождении нужного количества времени
        void (*ptr_func_do_when_end)() = nullptr;
        void (*ptr_func_do_when_start)() = nullptr;
        unsigned long time = 0;
        unsigned long timeStart = 0;
    public:
        Timer(){};
        Timer(unsigned long in_time):time(in_time){};
        Timer(unsigned long in_time,  void (*ptr_func_in_start)(), void (*ptr_func_in_end)()):
              time(in_time), ptr_func_do_when_end(ptr_func_in_start),
              ptr_func_do_when_start(ptr_func_in_end){};
        void update();
        void restart();
        unsigned long getTime() const { return time; }
        void setTime(unsigned long time_) { time = time_; }
        void setFuncEnd( void (*ptr_func_in)())
            {ptr_func_do_when_end = ptr_func_in;};
        void setFuncStart( void (*ptr_func_in)())
            {ptr_func_do_when_start = ptr_func_in;};
        void stop();
    
};

// template <typename ENUM_STATE>
void Timer::update()
{
    if(getStatus() == state_Timer::stoped){
        return;
    }
    if(millis() - timeStart >= time){
        setStatus(state_Timer::stoped);
        if(ptr_func_do_when_end != nullptr){
            (*ptr_func_do_when_end)();
        }
    }
}

void Timer::restart() {
    // if(getStatus() == state_Timer::stoped){
    //     return;
    // }
    setStatus(state_Timer::going);
    timeStart = millis();
    if(ptr_func_do_when_start != nullptr){
        (*ptr_func_do_when_start)();
    }
}

void Timer::stop() {
    setStatus(state_Timer::stoped);
}

#endif // TIMER_HPP
