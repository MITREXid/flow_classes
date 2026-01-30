#ifndef COMPONENT_HPP
#define COMPONENT_HPP


#include "declorations.hpp"
#include "universal_object.hpp"
#include "timer.hpp"

enum state_Component {close = 0, in_going = 1, open = 2};
class Component: public Universal_object<state_Component>{
    private:
        CallableBase* open_func_end = nullptr;
        CallableBase* close_func_end = nullptr;
        CallableBase* open_func_start = nullptr;
        CallableBase* close_func_start = nullptr;
        Timer timer_closing;
        Timer timer_opening;
        state_Component default_state = state_Component::close;
    public:
        
        //Конструктор
        Component(){
            //ни чего не делает
        };

        /// @brief конструктор задает начальные параметры
        /// @param curr_state состояние по умолчанию
        Component(state_Component curr_state){
            default_state = curr_state; 
            setStatus(default_state);
        };

        /// @brief конструктор задает начальные параметры
        /// @param curr_state состояние по умолчанию
        /// @param in_time_open время на открытие 
        /// @param in_time_close время на закрытие
        Component(state_Component curr_state, unsigned long in_time_open, unsigned long in_time_close){
            if(curr_state == in_going){
                println("Not correct start state. set close");
                curr_state = state_Component::close;
            }
            default_state = curr_state;
            setStatus(default_state);
            timer_opening.setTime(in_time_open);
            timer_closing.setTime(in_time_close);
        }
        
        //ф-ии управления

        //ф-ия инициализации
        virtual void init();
        //ф-ия обновления текущего состояния(и выполнения действий в связи с этим), её надо переодически вызывать(часто)
        virtual void update();
        //сигнал на открытия
        virtual void open();
        //сигнал на закрытие
        virtual void close();
        
        //ф-ии для проверки можем ли открыть/закрыть
        ///CHEKME может нужно переделать т.к. если мы открыты то как бы тоже можем открыться, или переписать обе на тему != going
        bool can_open(){ 
            return (getStatus() == state_Component::close); 
        }
        bool can_close(){ 
            return (getStatus() == state_Component::open); 
        }

        //ф-ии для привеления к состоянию поумолчанию(оно задается в конструкторе)
        virtual void to_default() {
            if(default_state == state_Component::close){
                setStatus(state_Component::open);
                close();
            }else if(default_state == state_Component::open){
                setStatus(state_Component::close);
                open();
            }
        }



        virtual ~Component(){
            deleteIfNotNull(open_func_end);
            deleteIfNotNull(close_func_end);
            deleteIfNotNull(open_func_start);
            deleteIfNotNull(close_func_start);
        }


        void setTimeOpening(unsigned long in_time)//установка времени открытия
            {timer_opening.setTime(in_time);}

        void setTimeClosing(unsigned long in_time)//установка времени закрытия
            {timer_closing.setTime(in_time);}


        //ф-ии для установки функции вызываемой при старте открытия 
        template<typename Func>
        void setFuncStartOpen(Func f) {
            open_func_start = new Callable<Func>(f);
            timer_opening.setFuncStart([&](){
                if(this !=nullptr) this->setStatus(state_Component::in_going);
                open_func_start->invoke();
            });
        }
        
        //ф-ии для установки функции вызываемой при старте закрытия
        template<typename Func>
        void setFuncStartClose(Func f) {
            close_func_start = new Callable<Func>(f);
            timer_closing.setFuncStart([&](){
                if(this !=nullptr) this->setStatus(state_Component::in_going);
                close_func_start->invoke();
            });
        }
        
        //ф-ии для установки функции вызываемой при окончании открытия
        template<typename Func>
        void setFuncEndOpen(Func f) {
            open_func_end = new Callable<Func>(f);
            timer_opening.setFuncEnd([&](){
                if(this !=nullptr) this->setStatus(state_Component::open);
                open_func_end->invoke();
            });
        }
        
        //ф-ии для установки функции вызываемой при окончании закрытия
        template<typename Func>
        void setFuncEndClose(Func f) {
            close_func_end = new Callable<Func>(f);
            timer_closing.setFuncEnd([&](){
                if(this !=nullptr) this->setStatus(state_Component::close);
                close_func_end->invoke();
            });
        }  

        
};

void Component::update(){
    timer_closing.update();
    timer_opening.update();
}
void Component::open(){
    if(can_open()){
        timer_opening.restart();
        timer_closing.stop();
    }else{
        println("already open or in going");
    }
}
void Component::close(){
    if(can_close()){
        timer_closing.restart();
        timer_opening.stop();
    }else{
        println("already close or in going");
    }
}

void Component::init(){
    to_default();
}

// void Component::stop_doing(){
//     timer_closing.stop();
//     timer_opening.stop();
// }

#endif // COMPONENT_HPP
