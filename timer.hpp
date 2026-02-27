#ifndef TIMER_HPP
#define TIMER_HPP

#include "declorations.hpp"
#include "universal_object.hpp"



//становится в 1
template <typename Sta = bool> 
class Signal{
protected:
 Sta state;
 bool checked = false;

public:
    bool isChecked(){
        return checked;
    }
    void setState(Sta st){
        checked = false;
        state =st;
    }
    Sta getState(){
        checked = true;
        return state;
    }
    bool isTrueAndNotCheked(){
        if(!isChecked() && getState() == true){
            return true;
        }
        return false;
    } 
    Sta getStateWithoutSetChecked(){
        return state;
    }
};



enum state_Timer {stoped = 0, going = 1};

class Timer : public Universal_object<state_Timer> {
private:
    Signal<> *signal_start = nullptr;
    Signal<> *signal_end = nullptr;
    Signal<> *signal_stop = nullptr;
    unsigned long time = 0;
    unsigned long timeStart = 0;
    
public:
    Timer() = default;
    
    
    /// @brief конструктор
    /// @param in_signal_start ссылка насигнальнуб переменную старта
    /// @param in_signal_end ссылка насигнальнуб переменную конца работы
    /// @param in_signal_stop ссылка насигнальнуб переменную острановки
    /// если не нужен какоё-то сигнал задать nullptr
     Timer(
            unsigned long in_time, 
            Signal<> *in_signal_start,
            Signal<> *in_signal_end,
            Signal<> *in_signal_stop 
        ) 
        : time(in_time), 
        signal_start(in_signal_start), 
        signal_end(in_signal_end), 
        signal_stop(in_signal_stop)  
        {}

    void update() {
        if(getStatus() == state_Timer::stoped) return;
        if(millis() - timeStart >= time) {
            setStatus(state_Timer::stoped);
            if(signal_start !=nullptr){
                signal_start->setState(0);
            }
            if(signal_end !=nullptr){
                signal_end->setState(1);
            }
            if(signal_stop !=nullptr){
                signal_stop->setState(0);
            }
        }
    }
    
    void restart() {
        setStatus(state_Timer::going);
        timeStart = millis();
        if(signal_start !=nullptr){
            signal_start->setState(1);
        }
        if(signal_end !=nullptr){
            signal_end->setState(0);
        }
        if(signal_stop !=nullptr){
            signal_stop->setState(0);
        }
    }
    
    void setSignalStart(Signal<> *f) {  signal_start = f;}
    void setSignalEnd(Signal<> *f) { signal_end = f; }
    void setSignalStop(Signal<> *f) { signal_stop = f; }
      
    //останавливает таймер, не вызывает функцию EndFunc
    void stop() {
        setStatus(state_Timer::stoped);
        if(signal_start !=nullptr){
            signal_start->setState(0);
        }
        if(signal_end !=nullptr){
            signal_end->setState(0);
        }
        if(signal_stop !=nullptr){
            signal_stop->setState(1);
        }
    }

    unsigned long getTime() const { return time; }
    void setTime(unsigned long time_) { time = time_; }
    
};

#endif // TIMER_HPP