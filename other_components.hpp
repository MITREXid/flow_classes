#ifndef OTHER_COMPONENTS_HPP
#define OTHER_COMPONENTS_HPP
#include "component.hpp"
#include "Shared_power.hpp"

class Actuator: public Component{
public:
    Actuator(state_Component first_state, int8_t id_);
    void update();
    void init(){
        Component::init();
        update();
    }
       bool close(bool is_infinite_process = true) {
        d_print(F("FLOW try Close Actuator("));
        d_print((int)get_id());
        d_print(F(")"));
        d_print(F(" , ("));  
        bool res = Component::close(is_infinite_process);
        d_print(F(")\n"));  
        return res;
    }
       bool open(bool is_infinite_process = true) {
        d_print(F("FLOW try Open Actuator("));
        d_print((int)get_id());
        d_print(F(")"));
        d_print(F(" , ("));  
        bool res = Component::open(is_infinite_process);
        d_print(F(")\n"));  
        return res;
       }

    void funcSigStartOpen(){
        d_print(F(" func SigStart Open Actuator("));  
        d_print((int)get_id());
        d_println(F(") "));
    }

    void funcSigEndOpen(){
        d_print(F(" func SigEnd Open Actuator("));
        d_print((int)get_id());
        d_println(F(") "));
    }

    void funcSigStartClose(){
        d_print(F(" func SigStart Close Actuator("));
        d_print((int)get_id());
        d_println(F(") "));
        
    }
    void funcSigEndClose(){
        d_print(F(" func SigEnd Close Actuator("));
        d_print((int)get_id());
        d_println(F(") "));
    }

    Actuator* getPointer() {
        return this;
    }

    uint8_t get_id() {
        return id;
    }
};
 
Actuator::Actuator(state_Component first_state, int8_t id_)
    : Component(
        id_, // идентификатор
        first_state, // состояние по умолчанию
        600, // время открытия
        600 // время закрытия
    )

{}

void Actuator::update(){
    Component::update();
}


class Clapan: public Component{
public:
    
    Clapan(state_Component first_state, int8_t id_);
    void update();

    void init(){
        Component::init();
        update();
    }
       bool close(bool is_infinite_process = true) {
        d_print(F("FLOW try Close Clapan("));
        d_print((int)get_id());
        d_print(F(")"));
        d_print(F(" , ("));  
        bool res = Component::close(is_infinite_process);
        d_print(F(")\n"));  
        return res;
    }
       bool open(bool is_infinite_process = true) {
        d_print(F("FLOW try Open Clapan("));
        d_print((int)get_id());
        d_print(F(")"));
        d_print(F(" , ("));  
        bool res = Component::open(is_infinite_process);
        d_print(F(")\n"));  
        return res;
       }

    void funcSigStartOpen(){
        d_print(F(" func SigStartOpen Clapan("));  
        d_print((int)get_id());
        d_println(F(") "));
    }

    void funcSigEndOpen(){
        d_print(F(" func SigEnd Open Clapan("));
        d_print((int)get_id());
        d_println(F(") "));
    }

    void funcSigStartClose(){
        d_print(F(" func SigStart Close Clapan("));
        d_print((int)get_id());
        d_println(F(") "));
        
    }
    void funcSigEndClose(){
        d_print(F(" func SigEnd Close Clapan("));
        d_print((int)get_id());
        d_println(F(") "));
    }

    Clapan* getPointer() {
        return this;
    }
};
 
Clapan::Clapan(state_Component first_state, int8_t id_)
    : Component(
        id_, // идентификатор
        first_state, // состояние по умолчанию
        500, // время открытия
        500 // время закрытия
    )
{}
void Clapan::update(){
    Component::update();
}




class Ball_cran: public Component{
public:
    Ball_cran(state_Component first_state, int8_t id_);
    void update();

    void init(){
        Component::init();
        update();
    }
           bool close(bool is_infinite_process = true) {
        d_print(F("FLOW try Close Ball_cran("));
        d_print((int)get_id());
        d_print(F(")"));
        d_print(F(" , ("));  
        bool res = Component::close(is_infinite_process);
        d_print(F(")\n"));  
        return res;
    }
       bool open(bool is_infinite_process = true) {
        d_print(F("FLOW try Open Ball_cran("));
        d_print((int)get_id());
        d_print(F(")"));
        d_print(F(" , ("));  
        bool res = Component::open(is_infinite_process);
        d_print(F(")\n"));  
        return res;
       }

    void funcSigStartOpen(){
        d_print(F(" func SigStartOpen Ball_cran("));  
        d_print((int)get_id());
        d_println(F(") "));
    }

    void funcSigEndOpen(){
        d_print(F(" func SigEnd Open Ball_cran("));
        d_print((int)get_id());
        d_println(F(") "));
    }

    void funcSigStartClose(){
        d_print(F(" func SigStart Close Ball_cran("));
        d_print((int)get_id());
        d_println(F(") "));
        
    }
    void funcSigEndClose(){
        d_print(F(" func SigEnd Close Ball_cran("));
        d_print((int)get_id());
        d_println(F(") "));
    }
    
      
    Ball_cran* getPointer() {
        return this;
    }
};

Ball_cran::Ball_cran(state_Component first_state, int8_t id_)
: Component(
        id_, // идентификатор
        first_state, //состояние по умолчанию
        15500, //время открытия
        15500 //время закрытия
 )
{}

void Ball_cran::update(){
    Component::update();
}

#endif // OTHER_COMPONENTS_HPP