#ifndef ALG_STATE_MAG_HPP
#define ALG_STATE_MAG_HPP

#include "universal_object.hpp"
// #include "magistral.hpp"
#include "types_for_magistral.hpp"



class one_state_Magistral{
    public:
       
         one_state_Magistral(
            uint8_t kol_path_,
            uint8_t kol_users_/*количество магистралей которые будет использовать это состояние*/
         ){
           
            if(kol_path_ < 1){
                kol_path_ = 1;
            }else{
                 kol_path=kol_path_;
            }
            if(kol_users < 1){
                kol_users = 1;
            }else{
                 kol_users=kol_users_;
            }

            choose_path = (uint8_t *)calloc(
                sizeof(uint8_t) * kol_users, 
                sizeof(uint8_t)
            );
            flag_reset_timer = (uint8_t *)calloc(
                sizeof(uint8_t) * kol_users/* ceil(kol_users/8)*/,//если нужно будет оптимизировать bool по 1 бит вместо 1 байта, но проблема в функции округления 
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
            d_println(F("ERROR: get_next_state in one_state_Magistral"));
            return nullptr;
         }

        void set_path(uint8_t path, one_state_Magistral * st){
            if(inRangePaths(path)){
                paths[path] = st;
                return;
            }
            d_println(F("ERROR: set_path in one_state_Magistral"));
         }

          uint8_t get_choose_path(uint8_t user){
            if(inRangeUsers(user)){
                return choose_path[user];
            }
            d_println(F("ERROR: get_choose_path in one_state_Magistral"));
            return 0;
         }

         void set_choose_path(uint8_t user, uint8_t path){
            if(inRangeUsers(user) && inRangePaths(path)){
                choose_path[user] = path;
                return;
            }
            d_println(F("ERROR: set_choose_path in one_state_Magistral"));
         }


        void set_flag_reset_timer(uint8_t user, bool val){
            if(inRangeUsers(user)){
                flag_reset_timer[user] = val;
                return;
            }
            d_println(F("ERROR: set_flag_reset_timer in one_state_Magistral"));
        }

         bool get_flag_reset_timer(uint8_t user){
            if(inRangeUsers(user)){
                return flag_reset_timer[user];
            }
            d_println(F("ERROR: get_flag_reset_timer in one_state_Magistral"));
             return false;
        }

private:
        uint8_t kol_path = 1;
        uint8_t kol_users = 1;
        Time_in_this time_in_this = 0;
        one_state_Magistral **paths = nullptr;//все возможные пути
        state_Magistral curr_state = state_Magistral::undefine;
        uint8_t *choose_path = nullptr;//куда дальше переходим
        uint8_t *flag_reset_timer = nullptr;

        bool inRangeUsers(uint8_t u){
            if(u<kol_users){
                return true;
            }
            d_println(F("ERROR: inRangeUsers = false in one_state_Magistral"));
            return false;
        }
       bool inRangePaths(uint8_t p){
            if(p<kol_path){
                return true;
            }
            d_println(F("ERROR: inRangePaths = false in one_state_Magistral"));
            return false;
       }

};

struct Data_alg{
    one_state_Magistral * start_state = nullptr;
    one_state_Magistral * exit_main_cycle =nullptr;  
    one_state_Magistral * in_mag =nullptr;  
};

bool setup_alg_magistral(uint8_t kol_users, 
Data_alg &result){
    one_state_Magistral* mag_start_state = new one_state_Magistral(4, kol_users);
    mag_start_state->set_curr_state(state_Magistral::start_state);
    mag_start_state->set_time_in_this(500);
    one_state_Magistral* going_to_gate_ = new one_state_Magistral(1, kol_users);
    going_to_gate_->set_curr_state(state_Magistral::going_to_gate);
    going_to_gate_->set_time_in_this(700);
    one_state_Magistral* all_close_ = new one_state_Magistral(1, kol_users);
    all_close_->set_curr_state(state_Magistral::all_close);
    all_close_->set_time_in_this(1000);
    one_state_Magistral* in_magistral_ = new one_state_Magistral(2, kol_users);
    in_magistral_->set_curr_state(state_Magistral::in_magistral);
    in_magistral_->set_time_in_this(2000);
    one_state_Magistral* air_on_ = new one_state_Magistral(2, kol_users);
    air_on_->set_curr_state(state_Magistral::air_on);
    air_on_->set_time_in_this(22000);
    one_state_Magistral* produv_ = new one_state_Magistral(1, kol_users);
    produv_->set_curr_state(state_Magistral::air_on);
    produv_->set_time_in_this(22000);
    one_state_Magistral* full_open_ = new one_state_Magistral(1, kol_users);
    full_open_->set_curr_state(state_Magistral::full_open);
    full_open_->set_time_in_this(10000);   


    air_on_->set_path(0,going_to_gate_);//продолжить цикл
    air_on_->set_path(1,mag_start_state);//на выход
    in_magistral_->set_path(0,air_on_);
    in_magistral_->set_path(1,in_magistral_);
    all_close_->set_path(0,in_magistral_);
    going_to_gate_->set_path(0,all_close_);
    mag_start_state->set_path(0,mag_start_state);
    mag_start_state->set_path(1,going_to_gate_);
    mag_start_state->set_path(2,produv_);
    mag_start_state->set_path(3,full_open_);
    produv_->set_path(0,mag_start_state);
    full_open_->set_path(0,produv_);
    // for(uint8_t i = 0;i<kol_users;++i){
    //     going_to_gate_->set_flag_reset_timer(i,true);
    //     mag_start_state->set_flag_reset_timer(i,true);
    // }

    // Data_alg result;
    result.start_state = mag_start_state;
    result.exit_main_cycle = air_on_;
    result.in_mag = in_magistral_;
    return true;
}

enum class state_Alg_mag{
    stop,
    prepare,
    one_cycle,
    cycle,
    produvka,
    clearing
};




#endif // ALG_STATE_MAG_HPP
