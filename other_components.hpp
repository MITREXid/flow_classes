#ifndef OTHER_COMPONENTS_HPP
#define OTHER_COMPONENTS_HPP
#include "component.hpp"
#include "Shared_power_5V.hpp"
#include "config.hpp"

// using MyBase = Component<
//     data_for_func, data_for_func, data_for_func, data_for_func,
//     type_func, type_func, type_func, type_func
// >;
class Actuator: public Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>
{
protected:
    data_for_func data;
public:
   Actuator(state_Component first_state, Shared_power_5V &PWM_, char control_pin_)
    : Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>(
        first_state, //состояние по умолчанию
        1600, //время открытия
        1600 //время закрытия
    ){
        
        data.control_pin = control_pin_;
        data.PWM = &PWM_;
        data.type = data_for_func::type_str::act;
        data_for_func *ptr = &data;

        this->setFuncStartOpen(funcOpenStart);
        this->setDataStartOpen(ptr);

        this->setFuncEndOpen(funcOpenEnd);
        this->setDataEndOpen(ptr);

        this->setFuncStartClose(funcCloseStart);
        this->setDataStartClose(ptr);

        this->setFuncEndClose(funcCloseEnd);
        this->setDataEndClose(ptr);
    }

    void update();

};
 
   

void Actuator::update(){
    Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>::update();
    data.PWM->update();
}


class Clapan: public Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>
{
protected:
    data_for_func data;
public:
   Clapan(state_Component first_state, Shared_power_5V &PWM_, char control_pin_)
    : Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>(
        first_state, //состояние по умолчанию
        500, //время открытия
        500 //время закрытия
    ){
        
        data.control_pin = control_pin_;
        data.PWM = &PWM_;
        data.type = data_for_func::type_str::clapan;
        data_for_func *ptr = &data;

        this->setFuncStartOpen(funcOpenStart);
        this->setDataStartOpen(ptr);

        this->setFuncEndOpen(funcOpenEnd);
        this->setDataEndOpen(ptr);

        this->setFuncStartClose(funcCloseStart);
        this->setDataStartClose(ptr);

        this->setFuncEndClose(funcCloseEnd);
        this->setDataEndClose(ptr);
    }

    void update();

};
 
   

void Clapan::update(){
    Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>::update();
    data.PWM->update();
}


class Ball_cran: public Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>
{
protected:
    data_for_func data;
public:
   Ball_cran(state_Component first_state, char control_pin_)
    : Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>(
        first_state, //состояние по умолчанию
        5000, //время открытия
        5000 //время закрытия
    ){
        
        data.control_pin = control_pin_;
        data.PWM = nullptr;
        data.type = data_for_func::type_str::ball;
        data_for_func *ptr = &data;

        this->setFuncStartOpen(funcOpenStart);
        this->setDataStartOpen(ptr);

        this->setFuncEndOpen(funcOpenEnd);
        this->setDataEndOpen(ptr);

        this->setFuncStartClose(funcCloseStart);
        this->setDataStartClose(ptr);

        this->setFuncEndClose(funcCloseEnd);
        this->setDataEndClose(ptr);
    }

    void update();

};
 

void Ball_cran::update(){
    Component<
    data_for_func*, data_for_func*, data_for_func*, data_for_func*
>::update();
}


//     //Конструктор надо вызывать или в setup или в loop чтобы не было проблем с инициализацией


// class Clapan: public Component<


// >{
// private:
//     Shared_power_5V& PWM;
//     char control_pin; 
// public:
    
//     Clapan(state_Component first_state, Shared_power_5V &PWM_, char control_pin_);
//     void update();
// };
 
// Clapan::Clapan(state_Component first_state, Shared_power_5V &PWM_, char control_pin_)
//     : Component{
//         first_state, //состояние по умолчанию
//         500, //время открытия
//         500 //время закрытия
//     },
//     PWM{PWM_},
//     control_pin{control_pin_}
// {

    
//     this->setFuncStartOpen([&](){
//         d_println("Start Open Clapan");  
//         pinMode(control_pin, LOW);
//         PWM.voltageON();
//     });

//     this->setFuncEndOpen([&](){
//         d_println("End Open Clapan");
//        PWM.voltageOFF();
//     });

//     this->setFuncStartClose([&](){
//         d_println("Start Close Clapan"); 
//         pinMode(control_pin, HIGH);
//        PWM.voltageON();
//     });
//     this->setFuncEndClose([&](){
//         d_println("End Close Clapan"); 
//        PWM.voltageOFF();
//     });


// }
// void Clapan::update(){
//     Component::update();
//     PWM.update();
// }




// class Ball_cran: public Component{
// protected:
//     char control_pin = -1; 
// public:
//     Ball_cran(state_Component first_state, char control_pin_);
//     void update();
// };

// Ball_cran::Ball_cran(state_Component first_state, char control_pin_)
// : Component{
//         first_state, //состояние по умолчанию
//         5000, //время открытия
//         5000 //время закрытия
//     } ,
//   control_pin{control_pin_}
// {
   
//     this->setFuncStartOpen([&](){
//         d_println("Start Open Ball_cran");  
//         pinMode(control_pin, LOW);
//     });

//     this->setFuncEndOpen([&](){
//         d_println("End Open Ball_cran");
//     });

//     this->setFuncStartClose([&](){
//         d_println("Start Close Ball_cran"); 
//         pinMode(control_pin, HIGH);
//     });
//     this->setFuncEndClose([&](){
//         d_println("End Close Ball_cran"); 
//     });
// }

// void Ball_cran::update(){
//     Component::update();
// }

#endif // OTHER_COMPONENTS_HPP
