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
           time_in_this = (Time_in_this *)calloc(
                sizeof(Time_in_this) * kol_users, 
                sizeof(Time_in_this)
            );
            paths[0] = this;

         }
         
         Time_in_this get_time_in_this(int8_t user){
             if(inRangeUsers(user)){
                return time_in_this[user];
            }  
            d_println(F("ERROR: get_time_in_this in one_state_Magistral"));
            return 0;

         }
         void set_time_in_this(Time_in_this val, int8_t user = -1){
            if(user == -1){
                for(int i = 0;i<kol_users;++i){
                    if(inRangeUsers(i)){
                        time_in_this[i] = val;
                    }
                }
            }else{
                if(inRangeUsers(user)){
                    time_in_this[user] = val;
                    return;
                }
                d_println(F("ERROR: set_time_in_this in one_state_Magistral"));
                return;   
            }
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
        Time_in_this *time_in_this = 0;
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
    one_state_Magistral * exit_main_cycle_complex =nullptr; 
    one_state_Magistral * exit_main_cycle_solo =nullptr;  
    one_state_Magistral * exit_main_cycle_triplet =nullptr; 
    one_state_Magistral * wait_with_coffe_in_mag_solo =nullptr;  
    one_state_Magistral * wait_with_coffe_in_mag_triplet =nullptr;  
    one_state_Magistral * wait_clearing = nullptr;
};

bool setup_alg_magistral(uint8_t kol_users, 
Data_alg &result){
    /*==========стартовое состояние==========*/
    one_state_Magistral* mag_start_state = new one_state_Magistral(6, kol_users);
    mag_start_state->set_curr_state(state_Magistral::start_state);
    mag_start_state->set_time_in_this(10);

    mag_start_state->set_path(0,mag_start_state);

    result.start_state = mag_start_state;
    /*==========стартовое состояние==========*/

 
    Time_in_this t_preparing_in_magistral_triplet_complex = 800;
    Time_in_this t_skip_gate_triplet_complex = 3500;
    Time_in_this t_in_magistral_triplet_complex = 800;//2000;
    Time_in_this pause_for_one_triplet_complex = t_preparing_in_magistral_triplet_complex + t_skip_gate_triplet_complex + t_in_magistral_triplet_complex ;//600 на переключение клапана после отгрузки
    Time_in_this pred_pause_triplet_complex = 100;
    Time_in_this post_pause_triplet_complex = 500;

    
   /*==========комплекс начало==========*/
   
   one_state_Magistral* starting_in_magistral_complex= new one_state_Magistral(1, kol_users);
    starting_in_magistral_complex->set_curr_state(state_Magistral::all_close);
    // starting_in_magistral_complex->set_time_in_this(650);
    starting_in_magistral_complex->set_time_in_this(pred_pause_triplet_complex , 0);
    starting_in_magistral_complex->set_time_in_this(pred_pause_triplet_complex + pause_for_one_triplet_complex, 1);
    starting_in_magistral_complex->set_time_in_this(pred_pause_triplet_complex + pause_for_one_triplet_complex + pause_for_one_triplet_complex, 2);

    starting_in_magistral_complex->set_time_in_this(
        pred_pause_triplet_complex + pause_for_one_triplet_complex + pause_for_one_triplet_complex + 
        t_preparing_in_magistral_triplet_complex +
         t_skip_gate_triplet_complex + 
         t_in_magistral_triplet_complex  
          + ((Time_in_this)600*3) + 200
        
        , 3);

    one_state_Magistral* preparing_in_magistral_complex= new one_state_Magistral(1, kol_users);
    preparing_in_magistral_complex->set_curr_state(state_Magistral::in_magistral);
    preparing_in_magistral_complex->set_time_in_this(t_preparing_in_magistral_triplet_complex);
    // preparing_in_magistral_complex->set_time_in_this(500, 0);
    // preparing_in_magistral_complex->set_time_in_this(500, 1);
    // preparing_in_magistral_complex->set_time_in_this(500, 2);
    // preparing_in_magistral_complex->set_time_in_this(500, 3);
    one_state_Magistral* skip_gate_complex= new one_state_Magistral(1, kol_users);
    skip_gate_complex->set_curr_state(state_Magistral::skip_gate);
    skip_gate_complex->set_time_in_this(t_skip_gate_triplet_complex);
    // skip_gate_complex->set_time_in_this(1500, 0);
    // skip_gate_complex->set_time_in_this(1500, 1);
    // skip_gate_complex->set_time_in_this(1500, 2);
    // skip_gate_complex->set_time_in_this(1500, 3);
    one_state_Magistral* in_magistral_complex= new one_state_Magistral(1, kol_users);
    in_magistral_complex->set_curr_state(state_Magistral::in_magistral);
    // in_magistral_complex->set_time_in_this(t_in_magistral_triplet_complex);
    in_magistral_complex->set_time_in_this(t_in_magistral_triplet_complex+ pause_for_one_triplet_complex + pause_for_one_triplet_complex + post_pause_triplet_complex, 0);
    in_magistral_complex->set_time_in_this(t_in_magistral_triplet_complex + pause_for_one_triplet_complex + post_pause_triplet_complex, 1);
    in_magistral_complex->set_time_in_this(t_in_magistral_triplet_complex + post_pause_triplet_complex, 2);
    in_magistral_complex->set_time_in_this(t_in_magistral_triplet_complex , 3);

    one_state_Magistral* all_close_complex= new one_state_Magistral(1, kol_users);//закрыл все клапана поочередно
    all_close_complex->set_curr_state(state_Magistral::all_close);
    all_close_complex->set_time_in_this(600*3);
    // all_close_complex->set_time_in_this(500, 0);
    // all_close_complex->set_time_in_this(500, 1);
    // all_close_complex->set_time_in_this(500, 2);
    all_close_complex->set_time_in_this(((Time_in_this)30000), 3);
    one_state_Magistral* air_on_complex= new one_state_Magistral(1, kol_users);
    air_on_complex->set_curr_state(state_Magistral::air_on);
    air_on_complex->set_time_in_this(30000);
    // air_on_complex->set_time_in_this(27000, 0);
    // air_on_complex->set_time_in_this(27000, 1);
    // air_on_complex->set_time_in_this(27000, 2);
    air_on_complex->set_time_in_this(30000, 3);

    mag_start_state->set_path(1,starting_in_magistral_complex);//триплет 5ый путь
    starting_in_magistral_complex->set_path(0,preparing_in_magistral_complex);
    preparing_in_magistral_complex->set_path(0,skip_gate_complex);
    skip_gate_complex->set_path(0,in_magistral_complex);

    in_magistral_complex->set_path(0,all_close_complex);

    all_close_complex->set_path(0,air_on_complex);
    air_on_complex->set_path(0,mag_start_state);

    result.exit_main_cycle_complex = air_on_complex;
    /*==========комплекс конец==========*/


    /*==========соло начало==========*/
    one_state_Magistral* preparing_in_magistral_solo = new one_state_Magistral(1, kol_users);
    preparing_in_magistral_solo->set_curr_state(state_Magistral::in_magistral);
    preparing_in_magistral_solo->set_time_in_this(650);
    //preparing_in_magistral_solo->set_time_in_this(500, 0);
    //preparing_in_magistral_solo->set_time_in_this(500, 1);
    //preparing_in_magistral_solo->set_time_in_this(500, 2);
    //preparing_in_magistral_solo->set_time_in_this(500, 3);
    one_state_Magistral* skip_gate_solo = new one_state_Magistral(1, kol_users);
    skip_gate_solo->set_curr_state(state_Magistral::skip_gate);
    skip_gate_solo->set_time_in_this(3500);
    // skip_gate_solo->set_time_in_this(1500, 0);
    // skip_gate_solo->set_time_in_this(1500, 1);
    // skip_gate_solo->set_time_in_this(1500, 2);
    // skip_gate_solo->set_time_in_this(1500, 3);
    one_state_Magistral* in_magistral_solo = new one_state_Magistral(2, kol_users);
    in_magistral_solo->set_curr_state(state_Magistral::in_magistral);
    in_magistral_solo->set_time_in_this(2000);
    // in_magistral_solo->set_time_in_this(2000, 0);
    // in_magistral_solo->set_time_in_this(2000, 1);
    // in_magistral_solo->set_time_in_this(2000, 2);
    // in_magistral_solo->set_time_in_this(2000, 3);
    one_state_Magistral* air_on_solo = new one_state_Magistral(1, kol_users);
    air_on_solo->set_curr_state(state_Magistral::air_on);
    air_on_solo->set_time_in_this(43000);
    // air_on_solo->set_time_in_this(27000, 0);
    // air_on_solo->set_time_in_this(27000, 1);
    // air_on_solo->set_time_in_this(27000, 2);
    // air_on_solo->set_time_in_this(27000, 3);
    //нужно после продувки(перед загрузкой) сделать задержку, чтоб кофе норм отнрузилось 
    one_state_Magistral* waiting_for_air_off_solo = new one_state_Magistral(2, kol_users);
    waiting_for_air_off_solo->set_curr_state(state_Magistral::all_close);
    waiting_for_air_off_solo->set_time_in_this(10000);
    // waiting_for_air_off_solo->set_time_in_this(2000, 0);
    // waiting_for_air_off_solo->set_time_in_this(2000, 1);
    // waiting_for_air_off_solo->set_time_in_this(2000, 2);
    // waiting_for_air_off_solo->set_time_in_this(2000, 3);


    mag_start_state->set_path(2,preparing_in_magistral_solo);//соло 2ой путь
    preparing_in_magistral_solo->set_path(0,skip_gate_solo);
    skip_gate_solo->set_path(0,in_magistral_solo);
    in_magistral_solo->set_path(1,in_magistral_solo);
    in_magistral_solo->set_path(0,air_on_solo);
    air_on_solo->set_path(0,waiting_for_air_off_solo);
    waiting_for_air_off_solo->set_path(0,preparing_in_magistral_solo);
    waiting_for_air_off_solo->set_path(1,mag_start_state);
    
    result.exit_main_cycle_solo = waiting_for_air_off_solo;
    result.wait_with_coffe_in_mag_solo = in_magistral_solo;
    /*==========соло конец==========*/


   /*==========триплет начало==========*/
   
 
   one_state_Magistral* starting_in_magistral_triplet = new one_state_Magistral(1, kol_users);
    starting_in_magistral_triplet->set_curr_state(state_Magistral::all_close);
    // starting_in_magistral_triplet->set_time_in_this(650);
    starting_in_magistral_triplet->set_time_in_this(pred_pause_triplet_complex , 0);
    starting_in_magistral_triplet->set_time_in_this(pred_pause_triplet_complex + pause_for_one_triplet_complex, 1);
    starting_in_magistral_triplet->set_time_in_this(pred_pause_triplet_complex + pause_for_one_triplet_complex + pause_for_one_triplet_complex, 2);
    starting_in_magistral_triplet->set_time_in_this(0, 3);

    one_state_Magistral* preparing_in_magistral_triplet = new one_state_Magistral(1, kol_users);
    preparing_in_magistral_triplet->set_curr_state(state_Magistral::in_magistral);
    preparing_in_magistral_triplet->set_time_in_this(t_preparing_in_magistral_triplet_complex);
    // preparing_in_magistral_triplet->set_time_in_this(500, 0);
    // preparing_in_magistral_triplet->set_time_in_this(500, 1);
    // preparing_in_magistral_triplet->set_time_in_this(500, 2);
    preparing_in_magistral_triplet->set_time_in_this(0, 3);
    one_state_Magistral* skip_gate_triplet = new one_state_Magistral(1, kol_users);
    skip_gate_triplet->set_curr_state(state_Magistral::skip_gate);
    skip_gate_triplet->set_time_in_this(t_skip_gate_triplet_complex);
    // skip_gate_triplet->set_time_in_this(1500, 0);
    // skip_gate_triplet->set_time_in_this(1500, 1);
    // skip_gate_triplet->set_time_in_this(1500, 2);
    skip_gate_triplet->set_time_in_this(0, 3);
    one_state_Magistral* in_magistral_triplet = new one_state_Magistral(1, kol_users);
    in_magistral_triplet->set_curr_state(state_Magistral::in_magistral);
    // in_magistral_triplet->set_time_in_this(t_in_magistral_triplet_complex);
    in_magistral_triplet->set_time_in_this(t_in_magistral_triplet_complex+ pause_for_one_triplet_complex + pause_for_one_triplet_complex + post_pause_triplet_complex, 0);
    in_magistral_triplet->set_time_in_this(t_in_magistral_triplet_complex + pause_for_one_triplet_complex + post_pause_triplet_complex, 1);
    in_magistral_triplet->set_time_in_this(t_in_magistral_triplet_complex + post_pause_triplet_complex, 2);
    in_magistral_triplet->set_time_in_this(0 , 3);

    one_state_Magistral* all_close_triplet = new one_state_Magistral(2, kol_users);//закрыл все клапана поочередно
    all_close_triplet->set_curr_state(state_Magistral::all_close);
    all_close_triplet->set_time_in_this(600*3);
    // all_close_triplet->set_time_in_this(500, 0);
    // all_close_triplet->set_time_in_this(500, 1);
    // all_close_triplet->set_time_in_this(500, 2);
    all_close_triplet->set_time_in_this(0, 3);
    one_state_Magistral* air_on_triplet = new one_state_Magistral(2, kol_users);
    air_on_triplet->set_curr_state(state_Magistral::air_on);
    air_on_triplet->set_time_in_this(30000);
    // air_on_triplet->set_time_in_this(27000, 0);
    // air_on_triplet->set_time_in_this(27000, 1);
    // air_on_triplet->set_time_in_this(27000, 2);
    air_on_triplet->set_time_in_this(0, 3);

    mag_start_state->set_path(4,starting_in_magistral_triplet);//триплет 4ый путь
    starting_in_magistral_triplet->set_path(0,preparing_in_magistral_triplet);
    preparing_in_magistral_triplet->set_path(0,skip_gate_triplet);
    skip_gate_triplet->set_path(0,in_magistral_triplet);

    in_magistral_triplet->set_path(0,all_close_triplet);

    all_close_triplet->set_path(0,air_on_triplet);
    all_close_triplet->set_path(1,all_close_triplet);
    air_on_triplet->set_path(0,starting_in_magistral_triplet);
    air_on_triplet->set_path(1, mag_start_state);

    result.exit_main_cycle_triplet = air_on_triplet;
    result.wait_with_coffe_in_mag_triplet = all_close_triplet;
    /*==========триплет конец==========*/

    /*==========продувка начало==========*/
    
    one_state_Magistral* produv_ = new one_state_Magistral(1, kol_users);
    produv_->set_curr_state(state_Magistral::air_on);
    produv_->set_time_in_this(15000);
    // produv_->set_time_in_this(27000, 0);
    // produv_->set_time_in_this(27000, 1);
    // produv_->set_time_in_this(27000, 2);
    // produv_->set_time_in_this(27000, 3);
    
    mag_start_state->set_path(5,produv_);//продувка 5ый путь
    produv_->set_path(0,mag_start_state);
    /*==========продувка конец==========*/


    /*==========прочистка начало==========*/
    one_state_Magistral* preparing_in_magistral_clearing = new one_state_Magistral(1, kol_users);
    preparing_in_magistral_clearing->set_curr_state(state_Magistral::in_magistral);
    preparing_in_magistral_clearing->set_time_in_this(800*4);
    //preparing_in_magistral_clearing->set_time_in_this(500, 0);
    //preparing_in_magistral_clearing->set_time_in_this(500, 1);
    //preparing_in_magistral_clearing->set_time_in_this(500, 2);
    //preparing_in_magistral_clearing->set_time_in_this(500, 3);
    one_state_Magistral* skip_gate_clearing = new one_state_Magistral(2, kol_users);
    skip_gate_clearing->set_curr_state(state_Magistral::skip_gate);
    skip_gate_clearing->set_time_in_this(700*4);
    // skip_gate_clearing->set_time_in_this(1500, 0);
    // skip_gate_clearing->set_time_in_this(1500, 1);
    // skip_gate_clearing->set_time_in_this(1500, 2);
    // skip_gate_clearing->set_time_in_this(1500, 3);
    one_state_Magistral* in_magistral_clearing = new one_state_Magistral(1, kol_users);
    in_magistral_clearing->set_curr_state(state_Magistral::in_magistral);
    in_magistral_clearing->set_time_in_this(800*4);
    // in_magistral_clearing->set_time_in_this(2000, 0);
    // in_magistral_clearing->set_time_in_this(2000, 1);
    // in_magistral_clearing->set_time_in_this(2000, 2);
    // in_magistral_clearing->set_time_in_this(2000, 3);


    mag_start_state->set_path(3,preparing_in_magistral_clearing);//прочистка 3bй путь
    preparing_in_magistral_clearing->set_path(0,skip_gate_clearing);
    skip_gate_clearing->set_path(0,skip_gate_clearing);
    skip_gate_clearing->set_path(1,in_magistral_clearing);
    in_magistral_clearing->set_path(0,mag_start_state);

    result.wait_clearing = skip_gate_clearing;//нажали на стоп и переключили
    /*==========прочистка конец==========*/


    
    // one_state_Magistral* preparing_in_magistral_ = new one_state_Magistral(1, kol_users);
    // preparing_in_magistral_->set_curr_state(state_Magistral::in_magistral);
    // preparing_in_magistral_->set_time_in_this(500);
    // one_state_Magistral* skip_gate_ = new one_state_Magistral(1, kol_users);
    // skip_gate_->set_curr_state(state_Magistral::skip_gate);
    // skip_gate_->set_time_in_this(1500);
    // one_state_Magistral* in_magistral_ = new one_state_Magistral(2, kol_users);
    // in_magistral_->set_curr_state(state_Magistral::in_magistral);
    // in_magistral_->set_time_in_this(2000);
    // one_state_Magistral* air_on_ = new one_state_Magistral(1, kol_users);
    // air_on_->set_curr_state(state_Magistral::air_on);
    // air_on_->set_time_in_this(27000);
    // one_state_Magistral* produv_ = new one_state_Magistral(1, kol_users);
    // produv_->set_curr_state(state_Magistral::air_on);
    // produv_->set_time_in_this(27000);

    // //нужно после продувки(перед загрузкой) сделать задержку, чтоб кофе норм отнрузилось 
    // one_state_Magistral* waiting_for_air_off_ = new one_state_Magistral(2, kol_users);
    // waiting_for_air_off_->set_curr_state(state_Magistral::all_close);
    // waiting_for_air_off_->set_time_in_this(0);
    // waiting_for_air_off_->set_time_in_this(2000, 3);//тоько для 4ой магистрали, которая соло


    // /*========== чистка ==========*/
    // one_state_Magistral* pred_skip_gate_clearing_ = new one_state_Magistral(1, kol_users);
    // pred_skip_gate_clearing_->set_curr_state(state_Magistral::going_to_gate);
    // pred_skip_gate_clearing_->set_time_in_this(500);  
    // one_state_Magistral* skip_gate_clearing_ = new one_state_Magistral(1, kol_users);
    // skip_gate_clearing_->set_curr_state(state_Magistral::skip_gate);
    // skip_gate_clearing_->set_time_in_this(60000);   
    // // skip_gate_clearing_->set_time_in_this(10000, 0);//пример изменения времени одного состояния конкретной  магистрали
    // one_state_Magistral* full_open_warning_clearing_ = new one_state_Magistral(1, kol_users);
    // full_open_warning_clearing_->set_curr_state(state_Magistral::full_open);
    // full_open_warning_clearing_->set_time_in_this(15000);   
    //  one_state_Magistral* post_full_open_warning_clearing_ = new one_state_Magistral(1, kol_users);
    // post_full_open_warning_clearing_->set_curr_state(state_Magistral::going_to_gate);
    // post_full_open_warning_clearing_->set_time_in_this(500);
    // /*========== чистка ==========*/

    // // air_on_->set_path(0,preparing_in_magistral_);//продолжить цикл
    // // air_on_->set_path(1,mag_start_state);//на выход
    // // in_magistral_->set_path(0,air_on_);

    // in_magistral_->set_path(0,air_on_);
    // waiting_for_air_off_->set_path(0,preparing_in_magistral_);//продолжить цикл
    // waiting_for_air_off_->set_path(1,mag_start_state);//на выход
    // air_on_->set_path(0,waiting_for_air_off_);

    // in_magistral_->set_path(1,in_magistral_);
    // skip_gate_->set_path(0,in_magistral_);
    // preparing_in_magistral_->set_path(0,skip_gate_);
    // mag_start_state->set_path(0,mag_start_state);
    // mag_start_state->set_path(1,preparing_in_magistral_);
    // mag_start_state->set_path(2,produv_);
    // mag_start_state->set_path(3,pred_skip_gate_clearing_);
    // produv_->set_path(0,mag_start_state);
    // /*========== чистка ==========*/
    // skip_gate_clearing_->set_path(0,full_open_warning_clearing_);
    // pred_skip_gate_clearing_->set_path(0,skip_gate_clearing_);
    // full_open_warning_clearing_->set_path(0,post_full_open_warning_clearing_);
    // post_full_open_warning_clearing_->set_path(0,mag_start_state);
    // /*========== чистка ==========*/
   

    // Data_alg result;
    // result.start_state = mag_start_state;
    // result.exit_main_cycle = waiting_for_air_off_;
    // result.in_mag = in_magistral_;



    return true;
}

enum class state_Alg_mag{
    stop,
    prepare_solo,
    prepare_triplet,
    one_cycle_solo,
    one_cycle_complex,
    one_cycle_triplet,
    cycle_solo,
    cycle_triplet,
    produvka,
    clearing
};




#endif // ALG_STATE_MAG_HPP
