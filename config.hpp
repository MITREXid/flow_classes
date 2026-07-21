#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "declorations.hpp"
#include "types_for_magistral.hpp"
//RS485
#define RO_RS485 12//пин RX(RO) 
#define DI_RS485 10//пин TX(DI) 

#define pin_DE_RE 11

//для флоу
#define kol_mag_group 3
#define kol_all_mag 4
#define solo_mag 3//nomer от 0

//=================экран


#define size_of_buf 15
#define SerialXXX Serial3







//время конфиги


constexpr const uint32_t TIME_ACTUATOR_FULL_OPEN_CLOSE[kol_all_mag] = {3000, 1300, 2800, 2800};//соло с индексом 3

constexpr const uint32_t T_COMPLEX_PRED_SKIP = 800;
constexpr const uint32_t T_COMPLEX_PRED_SKIP_1_MAG = T_COMPLEX_PRED_SKIP;
constexpr const uint32_t T_COMPLEX_PRED_SKIP_2_MAG = T_COMPLEX_PRED_SKIP;
constexpr const uint32_t T_COMPLEX_PRED_SKIP_3_MAG = T_COMPLEX_PRED_SKIP;
constexpr const uint32_t T_COMPLEX_PRED_SKIP_4_MAG = T_COMPLEX_PRED_SKIP;

constexpr const uint32_t T_COMPLEX_SKIP = 1500;
constexpr const uint32_t T_COMPLEX_SKIP_1_MAG = T_COMPLEX_SKIP - 1000;
constexpr const uint32_t T_COMPLEX_SKIP_2_MAG = 1000;
constexpr const uint32_t T_COMPLEX_SKIP_3_MAG = T_COMPLEX_SKIP - 1500;
constexpr const uint32_t T_COMPLEX_SKIP_4_MAG = T_COMPLEX_SKIP - 1000;

constexpr const uint32_t T_COMPLEX_POST_SKIP = 800;
constexpr const uint32_t T_COMPLEX_POST_SKIP_1_MAG = T_COMPLEX_POST_SKIP;
constexpr const uint32_t T_COMPLEX_POST_SKIP_2_MAG = 2000;
constexpr const uint32_t T_COMPLEX_POST_SKIP_3_MAG = T_COMPLEX_POST_SKIP;
constexpr const uint32_t T_COMPLEX_POST_SKIP_4_MAG = T_COMPLEX_POST_SKIP;



constexpr const Time_in_this T_COMPLEX_PAUSE_ONE_CYCLE_LOAD_1_MAG = T_COMPLEX_PRED_SKIP_1_MAG + T_COMPLEX_SKIP_1_MAG + T_COMPLEX_POST_SKIP_1_MAG ;
constexpr const Time_in_this T_COMPLEX_PAUSE_ONE_CYCLE_LOAD_2_MAG = T_COMPLEX_PRED_SKIP_2_MAG + T_COMPLEX_SKIP_2_MAG + T_COMPLEX_POST_SKIP_2_MAG ;
constexpr const Time_in_this T_COMPLEX_PAUSE_ONE_CYCLE_LOAD_3_MAG = T_COMPLEX_PRED_SKIP_3_MAG + T_COMPLEX_SKIP_3_MAG + T_COMPLEX_POST_SKIP_3_MAG ;
constexpr const Time_in_this T_COMPLEX_PAUSE_ONE_CYCLE_LOAD_4_MAG = T_COMPLEX_PRED_SKIP_4_MAG + T_COMPLEX_SKIP_4_MAG + T_COMPLEX_POST_SKIP_4_MAG ;


constexpr const uint32_t T_COMPLEX_PRED_LOAD = 100;
constexpr const uint32_t T_COMPLEX_POST_LOAD = 500;
constexpr const uint32_t T_COMPLEX_ALL_CLOSE_PRED_AIR = 600*3;
constexpr const uint32_t T_COMPLEX_AIRON_TRIPL = 25000;
constexpr const uint32_t T_COMPLEX_AIRON_SOLO = 39000;

// constexpr const uint32_t T_TRIPLET_PRED_SKIP = T_COMPLEX_PRED_SKIP;
// constexpr const uint32_t T_TRIPLET_SKIP = T_COMPLEX_SKIP;
// constexpr const uint32_t T_TRIPLET_POST_SKIP = T_COMPLEX_POST_SKIP;
// constexpr const uint32_t T_TRIPLET_PRED_PAUSE = T_COMPLEX_PRED_PAUSE;
// constexpr const uint32_t T_TRIPLET_POST_PAUSE = T_COMPLEX_POST_PAUSE;
// constexpr const uint32_t T_TRIPLET_ALL_CLOSE_PRED_AIR = T_COMPLEX_ALL_CLOSE_PRED_AIR;
// constexpr const uint32_t T_TRIPLET_AIRON_TRIPL = T_COMPLEX_AIRON_TRIPL;
// constexpr const uint32_t T_TRIPLET_AIRON_SOLO = T_COMPLEX_AIRON_SOLO;

constexpr const uint32_t T_SOLO_PRED_SKIP = 650;
constexpr const uint32_t T_SOLO_SKIP = 2500;
constexpr const uint32_t T_SOLO_POST_SKIP = 3200;
constexpr const uint32_t T_SOLO_AIRON = 39000;
constexpr const uint32_t T_SOLO_WAIT_AIROFF = 10000;


constexpr const uint32_t T_AIR_AIRON = 10000;

constexpr const uint32_t T_CLEARING_PRED = 600*5;
constexpr const uint32_t T_CLEARING_SKIP = 500 + TIME_ACTUATOR_FULL_OPEN_CLOSE[0]+TIME_ACTUATOR_FULL_OPEN_CLOSE[1]+TIME_ACTUATOR_FULL_OPEN_CLOSE[2]+TIME_ACTUATOR_FULL_OPEN_CLOSE[3];
constexpr const uint32_t T_CLEARING_POST = 500 + TIME_ACTUATOR_FULL_OPEN_CLOSE[0]+TIME_ACTUATOR_FULL_OPEN_CLOSE[1]+TIME_ACTUATOR_FULL_OPEN_CLOSE[2]+TIME_ACTUATOR_FULL_OPEN_CLOSE[3];



constexpr const uint32_t FREQ_SOLO = 2500;
constexpr const uint32_t FREQ_COMPLEX = 6000;
constexpr const uint32_t FREQ_COMPLEX_PART_SOLO = 4000;
constexpr const uint32_t FREQ_AIR = 5000;
constexpr const uint32_t FREQ_CLEARING = 5000;
constexpr const uint32_t FREQ_TRIPLET = 6000;



//=============для RS485


void preTransmission() {
    digitalWrite(pin_DE_RE, 1);
    time_break(20);
    // delayMicroseconds(10);
}

void postTransmission() {
    digitalWrite(pin_DE_RE, 0);
    time_break(20);
    // delayMicroseconds(10);
}





//для MY110(-1 это конец массива, для того чтоб была динамичность кол-ва пинов)
constexpr int8_t pins_MY110_mag1_act_forward[] = {25,29,17,21,-1};
constexpr int8_t pins_MY110_mag1_act_revers[] = {26,30,17,21,-1};

constexpr int8_t pins_MY110_mag2_act_forward[] = {25,29,18,22,-1};
constexpr int8_t pins_MY110_mag2_act_revers[] = {26,30,18,22,-1};

constexpr int8_t pins_MY110_mag3_act_forward[] = {25,29,19,23,-1};
constexpr int8_t pins_MY110_mag3_act_revers[] = {26,30,19,23,-1};

constexpr int8_t pins_MY110_mag4_act_forward[] = {25,29,20,24,-1};
constexpr int8_t pins_MY110_mag4_act_revers[] = {26,30,20,24,-1};

constexpr int8_t pins_MY110_mag1_clap_revers[] = {31,28,12,16,-1};
constexpr int8_t pins_MY110_mag1_clap_forward[] = {32,27,12,16,-1};

constexpr int8_t pins_MY110_mag2_clap_revers[] = {31,28,11,15,-1};
constexpr int8_t pins_MY110_mag2_clap_forward[] = {32,27,11,15,-1};

constexpr int8_t pins_MY110_mag3_clap_revers[] = {31,28,10,14,-1};
constexpr int8_t pins_MY110_mag3_clap_forward[] = {32,27,10,14,-1};

constexpr int8_t pins_MY110_mag4_clap_revers[] = {31,28,9,13,-1};
constexpr int8_t pins_MY110_mag4_clap_forward[] = {32,27,9,13,-1};

constexpr int8_t pins_MY110_mag1_ball_revers[] = {4,-1};
constexpr int8_t pins_MY110_mag1_ball_forward[] = {8,-1};

constexpr int8_t pins_MY110_mag2_ball_revers[] = {3,-1};
constexpr int8_t pins_MY110_mag2_ball_forward[] = {7,-1};

constexpr int8_t pins_MY110_mag3_ball_revers[] = {2,-1};
constexpr int8_t pins_MY110_mag3_ball_forward[] = {6,-1};

constexpr int8_t pins_MY110_mag4_ball_revers[] = {1,-1};
constexpr int8_t pins_MY110_mag4_ball_forward[] = {5,-1};



constexpr const int8_t* pins_MY110_all_mags_act_clap_forward[] = {
    pins_MY110_mag1_act_forward, 
    pins_MY110_mag2_act_forward, 
    pins_MY110_mag3_act_forward, 
    pins_MY110_mag4_act_forward,

    pins_MY110_mag1_clap_forward, 
    pins_MY110_mag2_clap_forward, 
    pins_MY110_mag3_clap_forward, 
    pins_MY110_mag4_clap_forward
};

constexpr const int8_t* pins_MY110_all_mags_act_clap_revers[] = {
    pins_MY110_mag1_act_revers, 
    pins_MY110_mag2_act_revers, 
    pins_MY110_mag3_act_revers, 
    pins_MY110_mag4_act_revers,

    pins_MY110_mag1_clap_revers, 
    pins_MY110_mag2_clap_revers,
    pins_MY110_mag3_clap_revers, 
    pins_MY110_mag4_clap_revers
};

constexpr const int8_t* pins_MY110_all_mags_ball_forward[] = {
    pins_MY110_mag1_ball_forward, 
    pins_MY110_mag2_ball_forward, 
    pins_MY110_mag3_ball_forward, 
    pins_MY110_mag4_ball_forward
};

constexpr const int8_t* pins_MY110_all_mags_ball_revers[] = {
    pins_MY110_mag1_ball_revers, 
    pins_MY110_mag2_ball_revers, 
    pins_MY110_mag3_ball_revers, 
    pins_MY110_mag4_ball_revers
};

constexpr int8_t pins_MY110_clear_act_clap[] = {
    9,10,11,12,
    13,14,15,16,
    17,18,19,20,
    21,22,23,24,
    25,26,27,28,
    29,30,31,32,    
    -1};

    constexpr int8_t pins_MY110_clear_ball[] = {
    1,2,3,4,5,6,7,8,    
    -1};
constexpr const int8_t* pins_MY110_all_mags_clear[] = {
    pins_MY110_clear_act_clap, 
    pins_MY110_clear_ball
};

#endif // CONFIG_HPPs