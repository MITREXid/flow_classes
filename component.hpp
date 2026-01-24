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
    public:
        void setTimeOpening(unsigned long in_time)
            {timer_opening.setTime(in_time);}

        void setTimeClosing(unsigned long in_time)
            {timer_closing.setTime(in_time);}
        Component(){setStatus(state_Component::close);};
        Component(state_Component curr_state){setStatus(curr_state);};
        Component(state_Component curr_state, unsigned long in_time_open, unsigned long in_time_close);

        // void setFuncEndOpen(void(* ptr_func_in)());
        // void setFuncStartOpen(void(* ptr_func_in)());
        // void setFuncEndClose(void(* ptr_func_in)());
        // void setFuncStartClose(void(* ptr_func_in)());

        template<typename Func>
        void setFuncStartOpen(Func f) {
            open_func_start = new Callable<Func>(f);
            timer_opening.setFuncStart([&](){
                if(this !=nullptr) this->setStatus(state_Component::in_going);
                open_func_start->invoke();
            });
        }
        
        template<typename Func>
        void setFuncStartClose(Func f) {
            close_func_start = new Callable<Func>(f);
            timer_closing.setFuncStart([&](){
                if(this !=nullptr) this->setStatus(state_Component::in_going);
                close_func_start->invoke();
            });
        }
        
        template<typename Func>
        void setFuncEndOpen(Func f) {
            open_func_end = new Callable<Func>(f);
            timer_opening.setFuncEnd([&](){
                if(this !=nullptr) this->setStatus(state_Component::open);
                open_func_end->invoke();
            });
        }
        
        template<typename Func>
        void setFuncEndClose(Func f) {
            close_func_end = new Callable<Func>(f);
            timer_closing.setFuncEnd([&](){
                if(this !=nullptr) this->setStatus(state_Component::close);
                close_func_end->invoke();
            });
        }  
        virtual void update();
        virtual void open();
        virtual void close();
};

void Component::update(){
    // if(getStatus() == state_Component::in_going){
    timer_closing.update();
    timer_opening.update();
    // }
}
void Component::open(){
    if(getStatus() == state_Component::close){
        // setStatus(state_Component::in_going);
        timer_opening.restart();
        timer_closing.stop();
    }else{
        println("already open or in going");
    }
}
void Component::close(){
    if(getStatus() == state_Component::open){
        // setStatus(state_Component::in_going);
        timer_closing.restart();
        timer_opening.stop();
    }else{
        println("already close or in going");
    }
}

Component::Component(state_Component curr_state, unsigned long in_time_open, unsigned long in_time_close){
    setStatus(curr_state);
    timer_opening.setTime(in_time_open);
    timer_closing.setTime(in_time_close);
}



#endif // COMPONENT_HPP
