#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "flow.hpp"
#include "declorations.hpp"

// ==================== flow вцелом, пины

#define rele_5 27//шаровые 27
#define rele_6 43//шаровые 43
#define rele_7 29//шаровые 29
#define rele_8 41//шаровые 41

#define rele_9 31//актуатор 31
#define rele_11 33//актуатор 33
#define rele_13 35//актуатор  35
#define rele_15 37//актуатор 37

#define mod2_clap_rele_1_2  39//клапан 39
#define mod2_clap_rele_3_4 45//клапан 45
#define mod2_clap_rele_5_6 47//клапан 47
#define mod2_clap_rele_7_8 49//клапан 49

#define mod2_rele_15_power_clap 53// 53
#define rele_2_power_act 23// 23

#define pin_power_v12_act rele_2_power_act
#define pin_power_v12_act_clapan mod2_rele_15_power_clap

//============первая магистраль
#define mag_1_act rele_9//актуатор
#define mag_1_clap mod2_clap_rele_7_8//клапан
#define mag_1_ball rele_5//шаровый 
//===========вторая магистраль
#define mag_2_act rele_11//актуатор
#define mag_2_clap mod2_clap_rele_5_6//клапан
#define mag_2_ball rele_6//шаровый 
//===========третья магистраль
#define mag_3_act rele_13//актуатор
#define mag_3_clap mod2_clap_rele_3_4//клапан
#define mag_3_ball rele_7//шаровый 
//===========четвертая магистраль(соло)
#define mag_4_act rele_15//актуатор
#define mag_4_clap mod2_clap_rele_1_2//клапан
#define mag_4_ball rele_8//шаровый 
//========общие для всех магистралей


//RS485
#define RO_RS485 12//пин RX(RO) 
#define DI_RS485 10//пин TX(DI) 

#define pin_DE_RE 11


// #define mag_1_act //пин RE/DE настраивается в файле dyvka.hpp (был D11)
#define pow_v12_act pin_power_v12_act//пин открывающий реле на котором 12В
#define pow_v12_clap pin_power_v12_act_clapan






//=================экран


#define size_of_buf 15
#define SerialXXX Serial3



//=============для RS485


void preTransmission() {
    digitalWrite(pin_DE_RE, 1);
    // delayMicroseconds(10);
}

void postTransmission() {
    digitalWrite(pin_DE_RE, 0);
    // delayMicroseconds(10);
}



//для флоу


#define kol_mag_group 3
#define kol_all_mag 4
#define solo_mag 3//nomer от 0

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