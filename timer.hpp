#ifndef TIMER_HPP
#define TIMER_HPP

#include "declorations.hpp"
#include "universal_object.hpp"



enum state_Timer {stoped = 0, going = 1};

class Timer : public Universal_object<state_Timer> {
private:
    CallableBase* func_end = nullptr;
    CallableBase* func_start = nullptr;
    unsigned long time = 0;
    unsigned long timeStart = 0;
    
public:
    Timer() = default;
    
    Timer(unsigned long in_time) : time(in_time) {}
    
    template<typename FuncStart, typename FuncEnd>
    Timer(unsigned long in_time, FuncStart start_func, FuncEnd end_func) : 
          time(in_time) {
        setFuncStart(start_func);
        setFuncEnd(end_func);
    }
    
    ~Timer() {
        deleteIfNotNull(func_end);
        deleteIfNotNull(func_start);
    }
    
    // Запрещаем копирование, чтобы избежать проблем с владением указателей
    Timer(const Timer&) = delete;
    Timer& operator=(const Timer&) = delete;
    
    // Разрешаем перемещение
    Timer(Timer&& other) noexcept 
        : func_end(other.func_end), func_start(other.func_start),
          time(other.time), timeStart(other.timeStart) {
        other.func_end = nullptr;
        other.func_start = nullptr;
    }
    
    Timer& operator=(Timer&& other) noexcept {
        if (this != &other) {
            deleteIfNotNull(func_end);
            deleteIfNotNull(func_start);
            
            func_end = other.func_end;
            func_start = other.func_start;
            time = other.time;
            timeStart = other.timeStart;
            
            other.func_end = nullptr;
            other.func_start = nullptr;
        }
        return *this;
    }
    
    void update() {
        if(getStatus() == state_Timer::stoped){
            return;
        }
        if(millis() - timeStart >= time){
            setStatus(state_Timer::stoped);
            if(func_end != nullptr){
                func_end->invoke();
            }
        }
    }
    
    void restart() {
        setStatus(state_Timer::going);
        timeStart = millis();
        if(func_start != nullptr){
            func_start->invoke();
        }
    }
    
    void stop() {
        setStatus(state_Timer::stoped);
    }
    
    unsigned long getTime() const { return time; }
    void setTime(unsigned long time_) { time = time_; }
    
    template<typename Func>
    void setFuncEnd(Func f) {
        deleteIfNotNull(func_end);
        func_end = new Callable<Func>(f);
    }
    
    template<typename Func>
    void setFuncStart(Func f) {
        deleteIfNotNull(func_start);
        func_start = new Callable<Func>(f);
    }
};

#endif // TIMER_HPP