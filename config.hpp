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







//=============времена шагов алгоритма, мс
// Порции, ход актуатора, время воздуха и частоты задаются в настройках (settings.hpp, экран, USB «cfg»).
// Здесь - постоянные части шагов. Как из них складываются времена: alg_step_duration_ms() в alg_state_mag.hpp,
// проверить на плате: команда «cfg times».

// комплекс и триплет
constexpr const uint32_t T_COMPLEX_PRED_LOAD = 100;           // ожидание перед загрузкой первой магистрали
constexpr const uint32_t T_COMPLEX_PRED_SKIP = 600;           // открыт клапан, до открытия актуатора
constexpr const uint32_t T_COMPLEX_POST_SKIP = 2000;          // после шлюза (рассчитано на ход актуатора 1 с)
constexpr const uint32_t T_COMPLEX_POST_LOAD = 500;           // после загрузки последней магистрали
constexpr const uint32_t T_COMPLEX_ALL_CLOSE_PRED_AIR = 600*3;// всё закрыто перед воздухом

// соло
constexpr const uint32_t T_SOLO_PRED_SKIP = 650;              // открыт клапан, до открытия актуатора
constexpr const uint32_t T_SOLO_POST_SKIP = 2000;             // после шлюза (рассчитано на ход актуатора 1 с)
constexpr const uint32_t T_SOLO_WAIT_AIROFF = 10000;          // пауза между циклами соло

// чистка
constexpr const uint32_t T_CLEARING_TOGGLE_CLAP = 600;
constexpr const uint32_t T_CLEARING_GAP_CLAP = 200;
constexpr const uint32_t T_CLEARING_GAP_ACT = 500;



//=============для RS485


// FIX: было time_break(20) до и после каждой передачи (40 мс простоя на кадр).
// SoftwareSerial передаёт байты блокирующе, поэтому после последнего байта DE/RE можно
// отпускать сразу. Если на объекте появятся ошибки связи — поставить обратно 20.
constexpr uint16_t T_RS485_PRE_TX_MS = 1;
constexpr uint16_t T_RS485_POST_TX_MS = 0;

void preTransmission() {
    digitalWrite(pin_DE_RE, 1);
    time_break(T_RS485_PRE_TX_MS);
}

void postTransmission() {
    digitalWrite(pin_DE_RE, 0);
    time_break(T_RS485_POST_TX_MS);
}

//=============безопасность силовых выходов МУ110 (FIX залипания реле)
// Пауза после снятия питания с мотора (DO25..DO32) до переключения выбора/полярности:
// мотор останавливается, дуга на контактах гаснет.
constexpr uint32_t T_MY110_MOTOR_BRAKE = 250;
// Пауза после переключения реле выбора (DO9..DO24) до подачи питания.
constexpr uint32_t T_MY110_SELECT_SETTLE = 60;
// Пауза между отпусканием OPEN и включением CLOSE шарового крана (и наоборот).
constexpr uint32_t T_MY110_BALL_GAP = 150;
// Максимум непрерывной подачи питания на мотор. Страховка от зависания логики/связи.
// Должно быть больше самого долгого хода (TIME_ACTUATOR_FULL_OPEN_CLOSE, клапан 500 мс).
constexpr uint32_t T_MY110_MAX_MOTOR_ON = 6000;//ход актуатора настраивается до 5.0 с
// Задержка от команды до реальной подачи питания на мотор (снять питание -> выбор -> питание), с запасом.
// Добавляется к шлюзу, чтобы приоткрытие на порцию успело закончиться до команды "закрыть".
constexpr uint32_t T_DOSE_START = 400;
// Период повторной отправки маски, даже если она не менялась (для таймаута связи МУ).
constexpr uint32_t T_MY110_KEEPALIVE = 500;
// Компонент дольше этого времени "в движении" на МУ -> запись в журнал ошибок.
constexpr uint32_t T_MY110_STUCK = 5000;
// При запуске ждём, пока МУ включится (он стартует дольше Arduino), мс.
constexpr uint32_t T_MY110_INIT_WAIT = 4000;

// Ножевые (нижние) клапаны, у которых мотор подключён в обратной полярности:
// на "закрыть" они открывались. true - направление в прошивке меняется местами.
// Караганда: клапаны 1 и 3 работали наоборот (журнал 01.10.2026).
// Если на объекте перекинут провода мотора - поставить false.
constexpr bool CLAP_INVERT[kol_all_mag] = {false, false, false, false};

// ---------- воздуходувка (частотник ESQ-770, адрес 1) ----------
// Повтор команды частотнику, если он не ответил (после "стоп" сразу "пуск" он часто молчит).
constexpr uint32_t T_DYVKA_RETRY = 300;

// ---------- давление ----------
// 0 - нет датчика (на экране "--"), 1 - читать регистр частотника по RS485
#define PRESSURE_SOURCE 0
// Регистр частотника с давлением (найти командой mbr в мониторе порта), значение -> кПа:
//   кПа = (регистр + PRESSURE_OFFSET) * PRESSURE_SCALE
constexpr uint16_t PRESSURE_REG = 0x0000;
constexpr float PRESSURE_OFFSET = 0;
constexpr float PRESSURE_SCALE = 0.1;
constexpr uint32_t T_PRESSURE_READ = 1000;//как часто читать, мс

// Пауза перед повтором записи после ошибки Modbus.
constexpr uint32_t T_MY110_RETRY = 30;

// Номера DO (1..32), которые нельзя включать одновременно:
// плюс и минус на один общий провод (КЗ блока 12 В) и OPEN+CLOSE одного шарового крана.
constexpr int8_t MY110_FORBIDDEN_PAIRS[][2] = {
    {25,30}, {26,29},   // актуаторы: COM5 = DO25(+)/DO30(-), COM6 = DO26(+)/DO29(-)
    {27,31}, {28,32},   // ножевые клапаны: COM3 = DO27(+)/DO31(-), COM4 = DO28(+)/DO32(-)
    {1,5}, {2,6}, {3,7}, {4,8}  // шаровые краны OPEN/CLOSE
};

// Маски битов (бит = номер DO - 1)
constexpr uint32_t MY110_BALL_BITS   = 0x000000FFUL; // DO1..DO8   шаровые краны
constexpr uint32_t MY110_SELECT_BITS = 0x00FFFF00UL; // DO9..DO24  выбор клапана/актуатора
constexpr uint32_t MY110_POWER_BITS  = 0xFF000000UL; // DO25..DO32 питание +12/-12 на общие
constexpr uint32_t MY110_POWER_ACT   = (1UL<<24)|(1UL<<25)|(1UL<<28)|(1UL<<29); // DO25,26,29,30
constexpr uint32_t MY110_POWER_CLAP  = (1UL<<26)|(1UL<<27)|(1UL<<30)|(1UL<<31); // DO27,28,31,32





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