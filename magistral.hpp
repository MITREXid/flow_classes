#ifndef MAGISTRAL_HPP
#define MAGISTRAL_HPP
#include "universal_object.hpp"
#include "other_components.hpp"
#include "Shared_power_5V.hpp"

enum state_Magistral {
    going_to_gate = 1,//высыпаем зерно в шлюз
    all_close = 2,//все закрто
    in_magistral = 3,//в этом стотоянии ждем пока всё высыыпется в трубу
    air_on = 4,//в этом состоянии мы открываем шаровый (закрываем всё остальное) и ждем пока зерно дойдет до конца магистали
    full_open = 11//режим прочистки
};

class one_state_Magistral{
    public:
        // one_state_Magistral(state_Magistral curr_state_, uint32_t time_in_this_, one_state_Magistral *next_state_, one_state_Magistral *conditional_path_)
        // :curr_state{curr_state_}, time_in_this{time_in_this_}, next_state{next_state_}, conditional_path{conditional_path_}{}
        
        one_state_Magistral(
            state_Magistral curr_state_,
            uint32_t time_in_this_,
            one_state_Magistral *next_state_,
            one_state_Magistral *conditional_path_,
            bool flag_reset_timer_ = false,
            int setup_this = 0//нужно для огочтб в конструкотре задатьуказатель на себя
        )
            :curr_state{curr_state_},
            time_in_this{time_in_this_},
            next_state{next_state_},
            conditional_path{conditional_path_},
            flag_reset_timer{flag_reset_timer_}
        {
            if(setup_this==1){
                next_state = this;
                return;
            }else if(setup_this==2){
                conditional_path = this;
                return;
            }
        }

        state_Magistral curr_state;
        one_state_Magistral *next_state = nullptr;
        uint32_t time_in_this = 0;
        one_state_Magistral *conditional_path = nullptr;//если choose_conditional = true
        bool choose_conditional = false;
        bool flag_reset_timer = false;

};


class Magistral{//}: public Universal_object<state_Magistral>{
private:

    one_state_Magistral* mag_state_5;
    one_state_Magistral* mag_state_4;
    one_state_Magistral* mag_state_3;
    one_state_Magistral* mag_state_2;
    one_state_Magistral* mag_start_state;


    Actuator *actuator;
    Clapan *clapan;
    Ball_cran *ball_cran;
    uint32_t time_to_start_new_state = 0;//время старта
    // bool was_reset_time = false;//для того чтоб в состоянии в котором надо обнулить вермя не обнулять его бесконечно

    one_state_Magistral* current_state = mag_start_state;

public:
    Magistral(Shared_power_5V &PWM_, int control_pin_actuator, int control_pin_clapan, int control_pin_ball_cran)
    {
        actuator = new Actuator(state_Component::close, PWM_, control_pin_actuator);
        clapan = new Clapan(state_Component::close, PWM_, control_pin_clapan);
        ball_cran = new Ball_cran(state_Component::close, control_pin_ball_cran);
        // data.state = state_Magistral::default_;

        //далее указываем алгоритм работы
        mag_state_5 = new one_state_Magistral(state_Magistral::air_on, 22000, nullptr/*mag_state_2*/, nullptr/*mag_start_state*/);
        mag_state_4 = new one_state_Magistral(state_Magistral::in_magistral, 2000, mag_state_5,nullptr);
        mag_state_3 = new one_state_Magistral(state_Magistral::all_close, 1000, mag_state_4,nullptr);
        mag_state_2 = new one_state_Magistral(state_Magistral::going_to_gate, 700, mag_state_3,nullptr, true, 0);
        mag_start_state = new one_state_Magistral(state_Magistral::all_close, 500, nullptr, mag_state_2, true, 1);
        mag_state_5->conditional_path = mag_start_state;
        mag_state_5->next_state = mag_state_2;

        current_state = mag_start_state;

    }
    void init();
    void turn_to(state_Magistral next_state);
    void update();
    void logic();//Для объединения логики поведения(используется в update())
    void start();
    void stop();
    state_Magistral getState(one_state_Magistral* st = nullptr);
    one_state_Magistral* getNextState(one_state_Magistral* st = nullptr);
    uint32_t getTimeInState(one_state_Magistral* st = nullptr);
    void setPathFlag( bool val,one_state_Magistral* st = nullptr);
    bool getPathFlag(one_state_Magistral* st = nullptr);
    bool getFlagResetTimer(one_state_Magistral* st = nullptr);
};


void Magistral::turn_to(state_Magistral next_state){
    switch(next_state){
        case state_Magistral::going_to_gate://актуатор открыт клапан закрыт шаровой кран закрыт
                actuator->open();
                clapan->close();
                ball_cran->close();
            break;
         case state_Magistral::all_close://актуатор закрыт клапан закрыт шаровой кран закрыт
                actuator->close();
                clapan->close();
                ball_cran->close();
            break;
        case state_Magistral::in_magistral://актуатор закрыт клапан открыт шаровой кран закрыт
                actuator->close();
                clapan->open();
                ball_cran->close();
            break;
        case state_Magistral::air_on://актуатор закрыт клапан закрыт шаровой кран открыт
                actuator->close();
                clapan->close();
                ball_cran->open();
            break;
        case state_Magistral::full_open:
                actuator->open();
                clapan->open();
                ball_cran->open();
            break;
    };
    time_to_start_new_state = millis();
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
    if(current_state != mag_start_state){
        /**
         * не знаю что выбрать учитывая возможность разных режимов
         * current_state != mag_start_state
         * или
         * getState() == state_Magistral::all_close - как то где мы должны застрять
         * 
         *  - указатель на начальное не очень т.к. мб будут другие режимы и там мы будем стоять в 
         * других состояниях(хотя норм т.к. его можно сделать как
         * конечное состояние у других режимов )
         * 
         *  - all_close не очень т.к. open() может попасть на это с
         * остояние когда мы работаем в штатном режиме и будет презапуск жопа и т д
         * 
         */
        println("not in strted state");
        return;
    }
    println("========start");
    setPathFlag(true, mag_start_state);
    setPathFlag(false, mag_state_5);
    // time_to_start = millis();
}

void Magistral::stop(){
    println("========stop");
    setPathFlag(false, mag_start_state);
    setPathFlag(true, mag_state_5);
}


void Magistral::logic(){
    
        // print("========start time: ");
        // println(time_to_start_new_state);
        // print("========state: ");
        // println(getState());
    if(
        clapan->getStatus() == state_Component::in_going ||
        actuator->getStatus() == state_Component::in_going ||
        ball_cran->getStatus() == state_Component::in_going
    ){
        return;
    }

    uint32_t curr_time = millis() - time_to_start_new_state;
    if(getTimeInState() < curr_time){
        
        print("========state: ");
        println(getState());
        current_state = getNextState();
        turn_to(getState());
        print("->");
        println(getState());
    }

}



    state_Magistral Magistral::getState(one_state_Magistral* st){
        if(st==nullptr){
            st = current_state;
        }
        if(st==nullptr){println("AAAA nullptr getState");}
        return st->curr_state;
    }
    one_state_Magistral* Magistral::getNextState(one_state_Magistral* st){
        if(st==nullptr){
            st = current_state;
        }
        if(st==nullptr){println("AAAA nullptr getNextState");}
        if(st->choose_conditional){
            if(st->conditional_path == nullptr){
                println("AAAA nullptr getNextState st->conditional_path");
            }
            return st->conditional_path;
        }else{
            if(st->next_state == nullptr){
                println("AAAA nullptr getNextState st->next_state");
            }
            return st->next_state;
        }
    }
    uint32_t Magistral::getTimeInState(one_state_Magistral* st){
        if(st==nullptr){
            st = current_state;
        }
        if(st==nullptr){println("AAAA nullptr getTimeInState");}
        return st->time_in_this;
    }
    void Magistral::setPathFlag(bool val, one_state_Magistral* st){
        if(st==nullptr){
            st = current_state;
        }
        if(st==nullptr){println("AAAA nullptr setPathFlag");}
        st->choose_conditional = val;
    }
    bool Magistral::getPathFlag(one_state_Magistral* st){
        if(st==nullptr){
            st = current_state;
        }
        if(st==nullptr){println("AAAA nullptr getPathFlag");}
        return st->choose_conditional;
    }

    bool Magistral::getFlagResetTimer(one_state_Magistral* st){
        if(st==nullptr){
            st = current_state;
        }
        if(st==nullptr){println("AAAA nullptr getFlagResetTimer");}
        return st->flag_reset_timer;
    }
#endif // MAGISTRAL_HPP
