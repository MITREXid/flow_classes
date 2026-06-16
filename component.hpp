#ifndef COMPONENT_HPP
#define COMPONENT_HPP


#include "declorations.hpp"
#include "universal_object.hpp"
#include "timer.hpp"

enum state_Component {close = 0, going_close = 1, open = 2, going_open = 3, undef = 4};

// Базовый класс без виртуальных функций
class Component : public Universal_object<state_Component> {
protected:
    Timer timer_closing;
    Signal<> SigStartClose, SigEndClose;
    Timer timer_opening;
    Signal<> SigStartOpen, SigEndOpen;
    state_Component default_state = state_Component::close;
    
    bool flag_infinite_process = true; // если true, то процесс открытия/закрытия бесконечный, иначе ограничен временем таймера

    virtual void funcSigStartOpen(){};
    virtual void funcSigEndOpen(){};
    virtual void funcSigStartClose(){};
    virtual void funcSigEndClose(){};
public:
    Component(){}
    
    
        /// @brief конструктор задает начальные параметры
        /// @param curr_state состояние по умолчанию
    Component(state_Component curr_state) {
        Component();
        if(curr_state == going_close || curr_state == going_open) {
            d_println(F("Not correct start state. set close"));
            curr_state = state_Component::close;
        }
        default_state = curr_state;
        setStatus(default_state);
    }

        /// @brief конструктор задает начальные параметры
        /// @param curr_state состояние по умолчанию
        /// @param in_time_open время на открытие 
        /// @param in_time_close время на закрытие    
    Component(
        state_Component curr_state, 
        unsigned long in_time_open, 
        unsigned long in_time_close
    ) {
        Component();
        if(curr_state == going_close || curr_state == going_open) {
            d_println(F("Not correct start state. set close"));
            curr_state = state_Component::close;
        }
        default_state = curr_state;
        setStatus(default_state);
        timer_opening.setTime(in_time_open);
        timer_closing.setTime(in_time_close);
    }
    


    void update() {
        timer_closing.update();
        timer_opening.update();

         if(SigStartOpen.isTrueAndNotCheked()){  
           setStatus(state_Component::going_open);
            funcSigStartOpen();
        }
        if(SigStartClose.isTrueAndNotCheked()){  
           setStatus(state_Component::going_close);
            funcSigStartClose();
        }

        if(!flag_infinite_process){//отключили сигналы окончания
            if(SigEndClose.isTrueAndNotCheked()){  
            setStatus(state_Component::close);
            flag_infinite_process = true;
                funcSigEndClose();
            }
            if(SigEndOpen.isTrueAndNotCheked()){  
            setStatus(state_Component::open);
            flag_infinite_process = true;
                funcSigEndOpen();
            }
        }

    }
    
    bool open(bool is_infinite_process = true) {
        flag_infinite_process = is_infinite_process;
        // if(!is_going()) {
        //     d_println(F("cant open: in going"));
        //     return false;
        // }
        if(getStatus() == state_Component::open) {
            d_println(F("already open"));
            return false;
        }else{
            if(getStatus() == state_Component::going_close) {
                d_println(F("was going_close, let's go open"));
            }else if(getStatus() == state_Component::going_open) {
                d_println(F("yet going_open, restart"));
            }   
            timer_opening.restart();
            timer_closing.stop();
        }
        return true;
    }
    
    bool close(bool is_infinite_process = true) {
        flag_infinite_process = is_infinite_process;
        // if(!is_going()) {
        //     d_println(F("cant close: in going"));
        //     return false;
        // }
        if(getStatus() == state_Component::close) {
            d_println(F("already close"));
            return true;
        } else{
            if(getStatus() == state_Component::going_open) {
                d_println(F("was going_open, let's go close"));
            }else if(getStatus() == state_Component::going_close) {
                d_println(F("yet going_close, restart"));
            }
            timer_closing.restart();
            timer_opening.stop();
        }
        return true;
    }
    
    void to_default() {
        flag_infinite_process = true;
        if(default_state == state_Component::close) {
            setStatus(state_Component::open);
            close();
        } else if(default_state == state_Component::open) {
            setStatus(state_Component::close);
            open();
        }
    }
    
    //в конструкторе не работает setSignal, видимо меняются адреса
    void init() {
        timer_closing.setSignalStart(&SigStartClose);
        timer_closing.setSignalEnd(&SigEndClose);
        timer_opening.setSignalStart(&SigStartOpen);
        timer_opening.setSignalEnd(&SigEndOpen);
        to_default();
    }
    
    void setTimeOpening(unsigned long in_time) {
        timer_opening.setTime(in_time);
    }
    
    void setTimeClosing(unsigned long in_time) {
        timer_closing.setTime(in_time);
    }
    
    // Методы для проверки состояний остаются без изменений
    bool can_open() { return (getStatus() == state_Component::close); }
    bool can_close() { return (getStatus() == state_Component::open); }
    bool is_going(){if(getStatus() == going_close || getStatus() == going_open) {return true;}return false;}
};




#endif // COMPONENT_HPP
