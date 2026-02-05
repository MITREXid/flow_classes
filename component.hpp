#ifndef COMPONENT_HPP
#define COMPONENT_HPP


#include "declorations.hpp"
#include "universal_object.hpp"
#include "timer.hpp"

enum state_Component {close = 0, in_going = 1, open = 2};

// Базовый класс без виртуальных функций
template<
    typename OpenStartData,
    typename OpenEndData,
    typename CloseStartData,
    typename CloseEndData,
    typename OpenStartFunc = void(*)(OpenStartData),
    typename OpenEndFunc = void(*)(OpenEndData),
    typename CloseStartFunc  = void(*)(CloseStartData),
    typename CloseEndFunc  = void(*)(CloseEndData)
     >
class Component : public Universal_object<state_Component> {
protected:
    // Указываем шаблонные параметры для Timer
    Timer<CloseStartData, CloseEndData> timer_closing;
    Timer<OpenStartData, OpenEndData> timer_opening;
    state_Component default_state = state_Component::close;
    
public:
    Component() = default;
    

        /// @brief конструктор задает начальные параметры
        /// @param curr_state состояние по умолчанию
    Component(state_Component curr_state) {
        if(curr_state == in_going) {
            d_println("Not correct start state. set close");
            curr_state = state_Component::close;
        }
        default_state = curr_state;
        setStatus(default_state);
    }

        /// @brief конструктор задает начальные параметры
        /// @param curr_state состояние по умолчанию
        /// @param in_time_open время на открытие 
        /// @param in_time_close время на закрытие    
    Component(state_Component curr_state, unsigned long in_time_open, unsigned long in_time_close) {
        if(curr_state == in_going) {
            d_println("Not correct start state. set close");
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
    }
    
    bool open() {
        if(getStatus() == state_Component::in_going) {
            d_println("cant open: in going");
            return false;
        }
        if(getStatus() == state_Component::close) {
            timer_opening.restart();
            timer_closing.stop();
        } else {
            d_println("cant open: already open");
        }
        return true;
    }
    
    bool close() {
        if(getStatus() == state_Component::in_going) {
            d_println("cant close: in going");
            return false;
        }
        if(getStatus() == state_Component::open) {
            timer_closing.restart();
            timer_opening.stop();
        } else {
            d_println("cant close: already close");
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
    
    void init() {
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
    



//set func

    void setFuncEndOpen(OpenEndFunc f)
    {
        timer_opening.setFuncEnd(f);
    }
    void setFuncStartOpen(OpenStartFunc f)
    {
        timer_opening.setFuncStart(f);
    }

    void setFuncEndClose(CloseEndFunc f)
    {
        timer_closing.setFuncEnd(f);
    }
    void setFuncStartClose(CloseStartFunc f)
    {
        timer_closing.setFuncStart(f);
    }
//set data
    void setDataEndOpen(OpenEndData f)
    {
        timer_opening.setDataEnd(f);
    }
    void setDataStartOpen(OpenStartData&f)
    {
        timer_opening.setDataStart(f);
    }

    void setDataEndClose(CloseEndData f)
    {
        timer_closing.setDataEnd(f);
    }
    void setDataStartClose(CloseStartData f)
    {
        timer_closing.setDataStart(f);
    }
};




#endif // COMPONENT_HPP
