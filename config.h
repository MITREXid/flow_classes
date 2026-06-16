#ifndef CONFIG_HPP
#define CONFIG_HPP

#include "flow.hpp"

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



#endif // CONFIG_HPPs