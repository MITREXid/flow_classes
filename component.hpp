#ifndef COMPONENT_HPP
#define COMPONENT_HPP


#include "declorations.hpp"
#include "universal_object.hpp"
#include "timer.hpp"

enum state_Component {close = 0, in_going = 1, open = 2};

// Базовый класс без виртуальных функций
class Component : public Universal_object<state_Component> {
protected:
    Timer timer_closing;
    Signal<> SigStartClose, SigEndClose;
    Timer timer_opening;
    Signal<> SigStartOpen, SigEndOpen;
    state_Component default_state = state_Component::close;
    
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
        if(curr_state == in_going) {
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
        if(curr_state == in_going) {
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

        if(SigEndClose.isTrueAndNotCheked()){  
           setStatus(state_Component::close);
            funcSigEndClose();
        }
        if(SigStartOpen.isTrueAndNotCheked()){  
           setStatus(state_Component::in_going);
            funcSigStartOpen();
        }
        if(SigStartClose.isTrueAndNotCheked()){  
           setStatus(state_Component::in_going);
            funcSigStartClose();
        }
        if(SigEndOpen.isTrueAndNotCheked()){  
           setStatus(state_Component::open);
            funcSigEndOpen();
        }


    }
    
    bool open() {
        if(getStatus() == state_Component::in_going) {
            d_println(F("cant open: in going"));
            return false;
        }
        if(getStatus() == state_Component::close) {
            timer_opening.restart();
            timer_closing.stop();
        } else {
            d_println(F("cant open: already open"));
        }
        return true;
    }
    
    bool close() {
        if(getStatus() == state_Component::in_going) {
            d_println(F("cant close: in going"));
            return false;
        }
        if(getStatus() == state_Component::open) {
            timer_closing.restart();
            timer_opening.stop();
        } else {
            d_println(F("cant close: already close"));
        }
        return true;
    }
    
    void to_default() {
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
    
};




#endif // COMPONENT_HPP
