#ifndef MAGISTRAL_HPP
#define MAGISTRAL_HPP
#include "universal_object.hpp"
#include "other_components.hpp"
#include "Shared_power_5V.hpp"

enum state_Magistral {
    default_ = 0,//в нем стоим пока не запустимя
    going_to_gate = 5,//высыпаем зерно в шлюз
    // all_close = 1,//все закрто
    in_magistral = 2,//в этом стотоянии ждем пока всё высыыпется в трубу
    air_on = 4,//в этом состоянии мы открываем шаровый (закрываем всё остальное) и ждем пока зерно дойдет до конца магистали
    // full_open = 11//режим прочистки
};

// enum status_Magistral{
//     waiting_new = 0,
//     process_doing = 1
// };

struct magistral_data{//у каждого состояния есть момент нахождения в этом стотоянии и когда можжно из него уходить
    state_Magistral state;
    // status_Magistral status;
    bool flag_stop = true;//усли останавливаемся то надо довести дело до конца и остановиться
};

class Magistral{//}: public Universal_object<state_Magistral>{
private:
    Actuator *actuator;
    Clapan *clapan;
    Ball_cran *ball_cran;
    magistral_data data;
    uint32_t time_to_start = 0;//время старта
    //количество времени со старта, спустя которое мы переходим из текущего(название времени) в следующее стотояние
    struct _times_turn_to{
        // uint32_t default_ = 0;//из него выходм только по стрту
        uint32_t going_to_gate = 700;
        uint32_t in_magistral = 2000;
        uint32_t air_on = 22000;
        // uint32_t full_open = 1300;//не известно нужно ли время
    }times_turn_to;

public:
    Magistral(Shared_power_5V &PWM_, int control_pin_actuator, int control_pin_clapan, int control_pin_ball_cran)
    {
        actuator = new Actuator(state_Component::close, PWM_, control_pin_actuator);
        clapan = new Clapan(state_Component::open, PWM_, control_pin_clapan);
        ball_cran = new Ball_cran(state_Component::close, control_pin_ball_cran);
        data.state = state_Magistral::default_;
    }
    void init();
    void turn_to(state_Magistral next_state);
    void update();
    void logic();//Для объединения логики поведения(используется в update())
    void start();
    void stop();
};


void Magistral::turn_to(state_Magistral next_state){
    switch(next_state){
        case state_Magistral::default_://актуатор закрыт клапан открыт шаровой кран закрыт по умолчанию
                actuator->close();
                clapan->open();
                ball_cran->close();
                data.state = state_Magistral::default_;
            break;
         case state_Magistral::going_to_gate://актуатор открыт клапан закрыт шаровой кран закрыт
                actuator->open();
                clapan->close();
                ball_cran->close();
            data.state = state_Magistral::going_to_gate;
            break;
        case state_Magistral::in_magistral://актуатор закрыт клапан открыт шаровой кран закрыт
                actuator->close();
                clapan->open();
                ball_cran->close();
                data.state = state_Magistral::in_magistral;
            break;
        case state_Magistral::air_on://актуатор закрыт клапан закрыт шаровой кран открыт
                actuator->close();
                clapan->open();
                // actuator->open(); //почему мы так не делаем не понятно
                // clapan->close();
                ball_cran->open();
            data.state = state_Magistral::air_on;
            break;
    };
}


void Magistral::init()
{
    clapan->init();
    actuator->init();
    ball_cran->init();
}


void Magistral::update()
{
    clapan->update();
    actuator->update();
    ball_cran->update();

    logic();
    
}


void Magistral::start(){
    if(data.state != state_Magistral::default_){
        print("already started magistral");
        return;
    }
    
    // data.status = status_Magistral::process_doing;
    data.flag_stop = false;
    turn_to(state_Magistral::default_);
    println("=========turn_to(state_Magistral::default_);");
}

void Magistral::stop(){
    data.flag_stop = true;
    println("=========flag_stop");
}


void Magistral::logic(){
    if(
        clapan->getStatus() == state_Component::in_going ||
        actuator->getStatus() == state_Component::in_going ||
        ball_cran->getStatus() == state_Component::in_going
    ){
        // data.status = status_Magistral::process_doing;
        return;
    }else{
        // data.status = status_Magistral::waiting_new;
    }
    if(data.state == state_Magistral::default_){
        if(data.flag_stop == true){
            // println("=========stooooooooim");
            return;
        }else{
            time_to_start = millis();
            println("=========turn_to(state_Magistral::going_to_gate);");
            turn_to(state_Magistral::going_to_gate);
        }
    }
    uint32_t cur_time = millis() - time_to_start;
    //сначала вызов turn_to происходит открытие или закрытие чего-то потом мы тут анализируем и переходим на что-то следующее
    if(data.state == state_Magistral::going_to_gate && times_turn_to.going_to_gate <cur_time){
        print("=========");
        print(cur_time);
        println("=====turn_to(state_Magistral::in_magistral);");
        turn_to(state_Magistral::in_magistral);
        // println("=====turn_to(state_Magistral::all_close);");
        // turn_to(state_Magistral::all_close);
    }
    // if(data.state == state_Magistral::all_close && times_turn_to.all_close <cur_time){
    //     print("=========");
    //     print(cur_time);
    //     println("=========turn_to(state_Magistral::in_magistral);");
    //     turn_to(state_Magistral::in_magistral);
    // }
    if(data.state == state_Magistral::in_magistral && times_turn_to.in_magistral<cur_time){
        print("=========");
        print(cur_time);
        println("=========turn_to(state_Magistral::air_on);");
        turn_to(state_Magistral::air_on);
    }
    if(data.state == state_Magistral::air_on && times_turn_to.air_on<cur_time){
        print("=========");
        print(cur_time);
        println("=========turn_to(state_Magistral::default_);");
        turn_to(state_Magistral::default_);
    }
}

#endif // MAGISTRAL_HPP
