#ifndef MAGISTRAL_HPP
#define MAGISTRAL_HPP
#include "universal_object.hpp"
#include "other_components.hpp"
#include "Shared_power_5V.hpp"
#include "math.h"

typedef uint16_t Time_in_this;

enum state_Magistral {
    going_to_gate = 1,//высыпаем зерно в шлюз
    all_close = 2,//все закрто
    in_magistral = 3,//в этом стотоянии ждем пока всё высыыпется в трубу
    air_on = 4,//в этом состоянии мы открываем шаровый (закрываем всё остальное) и ждем пока зерно дойдет до конца магистали
    full_open = 11//режим прочистки
};


class one_state_Magistral{
    public:
       
         one_state_Magistral(
            uint8_t kol_path_,
            uint8_t kol_users_/*количество магистралей которые будет использовать это состояние*/
         ){
           
            if(kol_path_ < 1){
                 kol_path=kol_path_;
            }else{
                kol_path_ = 1;
            }
            if(kol_users < 1){
                 kol_users=kol_users_;
            }else{
                kol_users = 1;
            }

            choose_path = (uint8_t *)calloc(
                sizeof(uint8_t) * ceil(kol_users), 
                sizeof(uint8_t)
            );
            flag_reset_timer = (uint8_t *)calloc(
                sizeof(uint8_t) * ceil(kol_users/8), 
                sizeof(uint8_t)
            );

            paths = (one_state_Magistral **)calloc(
                sizeof(one_state_Magistral *) * kol_path, 
                sizeof(one_state_Magistral *)
            );
            paths[0] = this;

         }
         
         Time_in_this get_time_in_this(){
            return time_in_this;
         }
         void set_time_in_this(Time_in_this val){
            time_in_this = val;
         }

         state_Magistral get_curr_state(){
            return curr_state;
         }
         void set_curr_state(state_Magistral val){
            curr_state = val;
         }

         one_state_Magistral *get_next_state(uint8_t user){
            if(inRangeUsers(user) && inRangePaths(get_choose_path(user))){
                return paths[get_choose_path(user)];
            }
            d_println("ERROR: get_next_state in one_state_Magistral");
            return nullptr;
         }

        void set_path(uint8_t path, one_state_Magistral * st){
            if(inRangePaths(path)){
                paths[path] = st;
            }
            d_println("ERROR: set_path in one_state_Magistral");
         }

          uint8_t get_choose_path(uint8_t user){
            if(inRangeUsers(user)){
                return choose_path[user];
            }
            d_println("ERROR: get_choose_path in one_state_Magistral");
            return 0;
         }

         void set_choose_path(uint8_t user, uint8_t path){
            if(inRangeUsers(user) && inRangePaths(path)){
                choose_path[user] = path;
            }
            d_println("ERROR: set_choose_path in one_state_Magistral");
         }


        void set_flag_reset_timer(uint8_t user, bool val){
            if(inRangeUsers(user)){
                flag_reset_timer[user] = val;
            }
            d_println("ERROR: set_flag_reset_timer in one_state_Magistral");
        }

         bool get_flag_reset_timer(uint8_t user){
            if(inRangeUsers(user)){
                return flag_reset_timer[user];
            }
            d_println("ERROR: get_flag_reset_timer in one_state_Magistral");
             return false;
        }

private:
        uint8_t kol_path = 1;
        uint8_t kol_users = 1;
        Time_in_this time_in_this = 0;
        one_state_Magistral **paths = nullptr;//все возможные пути
        state_Magistral curr_state;
        uint8_t *choose_path = nullptr;//куда дальше переходим
        uint8_t *flag_reset_timer = nullptr;

        bool inRangeUsers(uint8_t u){
            if(u<kol_users-1){
                return true;
            }
            d_println("ERROR: inRangeUsers = false in one_state_Magistral");
            return false;
        }
       bool inRangePaths(uint8_t p){
            if(p<kol_path-1){
                return true;
            }
            d_println("ERROR: inRangePaths = false in one_state_Magistral");
            return false;
       }

};

one_state_Magistral * setup_alg_magistral(int kol_users){
    one_state_Magistral* mag_start_state = new one_state_Magistral(2, kol_users);
    mag_start_state->set_curr_state(state_Magistral::all_close);
    mag_start_state->set_time_in_this(500);
    one_state_Magistral* going_to_gate_ = new one_state_Magistral(1, kol_users);
    mag_start_state->set_curr_state(state_Magistral::going_to_gate);
    mag_start_state->set_time_in_this(700);
    one_state_Magistral* all_close_ = new one_state_Magistral(1, kol_users);
    mag_start_state->set_curr_state(state_Magistral::all_close);
    mag_start_state->set_time_in_this(1000);
    one_state_Magistral* in_magistral_ = new one_state_Magistral(1, kol_users);
    mag_start_state->set_curr_state(state_Magistral::in_magistral);
    mag_start_state->set_time_in_this(2000);
    one_state_Magistral* air_on_ = new one_state_Magistral(2, kol_users);
    mag_start_state->set_curr_state(state_Magistral::air_on);
    mag_start_state->set_time_in_this(22000);



    air_on_->set_path(0,going_to_gate_);//на выход
    air_on_->set_path(1,mag_start_state);//продолжить цикл
    in_magistral_->set_path(0,air_on_);
    all_close_->set_path(0,in_magistral_);
    going_to_gate_->set_path(0,all_close_);
    mag_start_state->set_path(1,going_to_gate_);
    for(uint8_t i = 0;i<kol_users;++i){
        going_to_gate_->set_flag_reset_timer(i,true);
        mag_start_state->set_flag_reset_timer(i,true);
    }
}




class Magistral{//}: public Universal_object<state_Magistral>{
private:

   
    uint8_t id  = 0;
    Actuator *actuator;
    Clapan *clapan;
    Ball_cran *ball_cran;
    uint32_t time_to_start_new_state = 0;//время старта
    // bool was_reset_time = false;//для того чтоб в состоянии в котором надо обнулить вермя не обнулять его бесконечно

    one_state_Magistral* current_state ;
    one_state_Magistral* start_state ;

public:
    Magistral(uint8_t id, Shared_power_5V &PWM_, char control_pin_actuator, char control_pin_clapan, char control_pin_ball_cran, one_state_Magistral* current_ste)
    {
        start_state = current_ste;
        actuator = new Actuator(state_Component::close, PWM_, control_pin_actuator);
        clapan = new Clapan(state_Component::close, PWM_, control_pin_clapan);
        ball_cran = new Ball_cran(state_Component::close, control_pin_ball_cran);
        current_state = current_ste;
    }
    void init();
    void turn_to(state_Magistral next_state);
    void update();
    void logic();//Для объединения логики поведения(используется в update())
    void start();
    void stop();
    // state_Magistral getState(one_state_Magistral* st = nullptr);
    // one_state_Magistral* getNextState(one_state_Magistral* st = nullptr);
    // uint32_t getTimeInState(one_state_Magistral* st = nullptr);
    // void setPathFlag( bool val,one_state_Magistral* st = nullptr);
    // bool getPathFlag(one_state_Magistral* st = nullptr);
    // bool getFlagResetTimer(one_state_Magistral* st = nullptr);
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
    if(current_state == start_state){
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
    d_println("========start");
    current_state->set_choose_path(id,1);
    return;
    }
        d_println("not in started state");
        return;
    // time_to_start = millis();
}

void Magistral::stop(){
    d_println("========stop");
    setPathFlag(false, mag_start_state);
    setPathFlag(true, mag_state_5);
}


void Magistral::logic(){
    
        // d_print("========start time: ");
        // d_println(time_to_start_new_state);
        // d_print("========state: ");
        // d_println(getState());
    if(
        clapan->getStatus() == state_Component::in_going ||
        actuator->getStatus() == state_Component::in_going ||
        ball_cran->getStatus() == state_Component::in_going
    ){
        return;
    }

    uint32_t curr_time = millis() - time_to_start_new_state;
    if(getTimeInState() < curr_time){
        
        d_print("========state: ");
        d_println(getState());
        current_state = getNextState();
        turn_to(getState());
        d_print("->");
        d_println(getState());
    }

}



    // state_Magistral Magistral::getState(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println("AAAA nullptr getState");}
    //     return st->curr_state;
    // }
    // one_state_Magistral* Magistral::getNextState(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println("AAAA nullptr getNextState");}
    //     if(st->choose_conditional){
    //         if(st->conditional_path == nullptr){
    //             d_println("AAAA nullptr getNextState st->conditional_path");
    //         }
    //         return st->conditional_path;
    //     }else{
    //         if(st->next_state == nullptr){
    //             d_println("AAAA nullptr getNextState st->next_state");
    //         }
    //         return st->next_state;
    //     }
    // }
    // uint32_t Magistral::getTimeInState(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println("AAAA nullptr getTimeInState");}
    //     return st->time_in_this;
    // }
    // void Magistral::setPathFlag(bool val, one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println("AAAA nullptr setPathFlag");}
    //     st->choose_conditional = val;
    // }
    // bool Magistral::getPathFlag(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println("AAAA nullptr getPathFlag");}
    //     return st->choose_conditional;
    // }

    // bool Magistral::getFlagResetTimer(one_state_Magistral* st){
    //     if(st==nullptr){
    //         st = current_state;
    //     }
    //     if(st==nullptr){d_println("AAAA nullptr getFlagResetTimer");}
    //     return st->flag_reset_timer;
    // }
#endif // MAGISTRAL_HPP
