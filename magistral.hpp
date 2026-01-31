#ifndef MAGISTRAL_HPP
#define MAGISTRAL_HPP
#include "universal_object.hpp"
#include "other_components.hpp"
#include "Shared_power_5V.hpp"

enum state_Magistral {
    // default_ = 0,//в нем стоим пока не запустимя
    going_to_gate = 5,//высыпаем зерно в шлюз
    // all_close = 1,//все закрто
    in_magistral = 2,//в этом стотоянии ждем пока всё высыыпется в трубу
    air_on = 4,//в этом состоянии мы открываем шаровый (закрываем всё остальное) и ждем пока зерно дойдет до конца магистали
    // full_open = 11//режим прочистки
};

// enum state_process_Magistral{//общий статус Магистрали
//     changing_state = 0,//какой-то компонент в состянии going
//     ready_turn = 1,//готов к пееходу в другое состояние
//     process_stoping = 2,//останавливается
//     stop = 3//остаовлена магистраль
// };

struct magistral_data{//у каждого состояния есть момент нахождения в этом стотоянии и когда можжно из него уходить
    state_Magistral state;
    // state_process_Magistral process;
    bool flag_stop = true;//если останавливаемся то надо довести дело до конца и остановиться
};

class Magistral{//}: public Universal_object<state_Magistral>{
private:
    Actuator *actuator;
    Clapan *clapan;
    Ball_cran *ball_cran;
    magistral_data data;
    uint32_t time_to_start = 0;//время старта
    state_Magistral default_state = state_Magistral::in_magistral;
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
        data.state = default_state;
    }
    void init();
    void turn_to(state_Magistral next_state);
    void update();
    void logic();//Для объединения логики поведения(используется в update())
    void start();
    void stop();
    // state_process_Magistral getStatusProcess();
    state_Magistral getStatus();
};


void Magistral::turn_to(state_Magistral next_state){
    switch(next_state){
        // case state_Magistral::default_://актуатор закрыт клапан открыт шаровой кран закрыт по умолчанию
        //         actuator->close();
        //         clapan->open();
        //         ball_cran->close();
        //         data.state = state_Magistral::default_;
        //     break;
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
    if(data.state != default_state){
        print("already started magistral");
        return;
    }
    println("=========flag_stop = false");
    data.flag_stop = false;
    turn_to(default_state);
}

void Magistral::stop(){
    data.flag_stop = true;
    println("=========flag_stop = true");
}



void Magistral::logic(){
    static bool last_flag_stop = true;
    if(
        clapan->getStatus() == state_Component::in_going ||
        actuator->getStatus() == state_Component::in_going ||
        ball_cran->getStatus() == state_Component::in_going
    ){
        return;
    }else{
    }
    if(data.state == default_state){
        if(data.flag_stop == true){
            last_flag_stop = data.flag_stop;
            return;
        }
        if(data.flag_stop != last_flag_stop && data.flag_stop == false){
            last_flag_stop = data.flag_stop;
            time_to_start = millis();
        }
    }
    uint32_t cur_time = millis() - time_to_start;
    // print("=========time_to_start =");
    // println(time_to_start);
    // print("=========state");
    // println(getStatus());
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
    if(data.state == state_Magistral::in_magistral && times_turn_to.in_magistral <cur_time){
        print("=========");
        print(cur_time);
        println("=========turn_to(state_Magistral::air_on);");
        turn_to(state_Magistral::air_on);
    }
    if(data.state == state_Magistral::air_on && times_turn_to.air_on<cur_time){
        print("=========");
        print(cur_time);
        println("=========turn_to(state_Magistral::default);");
        turn_to(default_state);//в данном слуячае это going_to_gate
    }
}


state_Magistral Magistral::getStatus(){
    return data.state;
}

// state_process_Magistral Magistral::getStatusProcess(){
//     return data.process;
// }

#endif // MAGISTRAL_HPP
