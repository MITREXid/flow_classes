#ifndef ALG_STATE_MAG_HPP
#define ALG_STATE_MAG_HPP
// =====================================================================
//  АЛГОРИТМ МАГИСТРАЛИ
//
//  Алгоритм - граф шагов. Магистраль стоит в одном шаге, ждёт его время
//  и переходит в следующий. Где у шага два продолжения (развилка), путь
//  выбирает режим (Magistral::set_current_mode_alg).
//
//  Файл по разделам:
//    1. Узел графа (one_state_Magistral)       - как хранится один шаг
//    2. Таблица шагов (ALG_STEPS)               - ЧТО делает алгоритм
//    3. Времена шагов (alg_step_duration_ms)    - СКОЛЬКО длится каждый шаг
//    4. Названия шагов (alg_step_name)          - для вывода «cfg times»
//    5. Построение графа (setup_alg_magistral)  - превращает таблицу в граф
//    6. Режимы (state_Alg_mag)
//
//  Номера шагов (STEP_...) и путей (PATH_...): types_for_magistral.hpp
//
//  Добавить шаг:
//    1) номер в enum AlgStep (types_for_magistral.hpp);
//    2) строка в ALG_STEPS, и поменять «куда дальше» у соседнего шага;
//    3) case в alg_step_duration_ms();
//    4) case в alg_step_name().
// =====================================================================

#include "universal_object.hpp"
#include "config.hpp"
#include "types_for_magistral.hpp"
#include "settings.hpp"


// =====================================================================
//  1. УЗЕЛ ГРАФА: один шаг алгоритма
//
//  Узел общий для всех магистралей. Внутри для каждой магистрали отдельно
//  хранятся время шага и выбранный путь.
// =====================================================================
class one_state_Magistral {
public:
    one_state_Magistral(uint8_t paths_count, uint8_t magistrals_count, uint8_t step_id) {
        unik_id = step_id;
        if (paths_count >= 1) {
            kol_path = paths_count;
        }
        if (magistrals_count >= 1) {
            kol_users = magistrals_count;
        }
        paths = (one_state_Magistral **)calloc(kol_path, sizeof(one_state_Magistral *));
        choose_path = (uint8_t *)calloc(kol_users, sizeof(uint8_t));
        time_in_this = (Time_in_this *)calloc(kol_users, sizeof(Time_in_this));
        paths[0] = this;    // пока путь не задан - шаг ведёт сам в себя
    }

    uint8_t get_id() {
        return unik_id;
    }

    // что делают компоненты в этом шаге (актуатор, клапан, шаровой)
    state_Magistral get_curr_state() {
        return curr_state;
    }
    void set_curr_state(state_Magistral val) {
        curr_state = val;
    }

    // время шага для магистрали mag
    Time_in_this get_time_in_this(uint8_t mag) {
        if (!inRangeUsers(mag)) {
            d_println(F("ERROR: get_time_in_this in one_state_Magistral"));
            return 0;
        }
        return time_in_this[mag];
    }
    // mag = -1: одинаковое время для всех магистралей
    void set_time_in_this(Time_in_this val, int8_t mag = -1) {
        if (mag == -1) {
            for (uint8_t i = 0; i < kol_users; ++i) {
                time_in_this[i] = val;
            }
            return;
        }
        if (!inRangeUsers(mag)) {
            d_println(F("ERROR: set_time_in_this in one_state_Magistral"));
            return;
        }
        time_in_this[mag] = val;
    }

    // куда ведёт путь номер path
    void set_path(uint8_t path, one_state_Magistral *next) {
        if (!inRangePaths(path)) {
            d_println(F("ERROR: set_path in one_state_Magistral"));
            return;
        }
        paths[path] = next;
    }

    // какой путь выбран для магистрали mag
    uint8_t get_choose_path(uint8_t mag) {
        if (!inRangeUsers(mag)) {
            d_println(F("ERROR: get_choose_path in one_state_Magistral"));
            return 0;
        }
        return choose_path[mag];
    }
    void set_choose_path(uint8_t mag, uint8_t path) {
        if (!inRangeUsers(mag) || !inRangePaths(path)) {
            d_println(F("ERROR: set_choose_path in one_state_Magistral"));
            return;
        }
        choose_path[mag] = path;
    }

    // следующий шаг для магистрали mag (по выбранному пути)
    one_state_Magistral *get_next_state(uint8_t mag) {
        if (!inRangeUsers(mag) || !inRangePaths(get_choose_path(mag))) {
            d_println(F("ERROR: get_next_state in one_state_Magistral"));
            return nullptr;
        }
        return paths[get_choose_path(mag)];
    }

private:
    uint8_t unik_id = 0;                          // номер шага (AlgStep)
    uint8_t kol_path = 1;                         // сколько путей из шага
    uint8_t kol_users = 1;                        // сколько магистралей пользуются шагом
    state_Magistral curr_state = state_Magistral::undefine;
    one_state_Magistral **paths = nullptr;        // [путь] -> следующий шаг
    uint8_t *choose_path = nullptr;               // [магистраль] -> выбранный путь
    Time_in_this *time_in_this = nullptr;         // [магистраль] -> время шага, мс

    bool inRangeUsers(uint8_t mag) {
        if (mag < kol_users) {
            return true;
        }
        d_println(F("ERROR: inRangeUsers = false in one_state_Magistral"));
        return false;
    }
    bool inRangePaths(uint8_t path) {
        if (path < kol_path) {
            return true;
        }
        d_println(F("ERROR: inRangePaths = false in one_state_Magistral"));
        return false;
    }
};


// =====================================================================
//  2. ТАБЛИЦА ШАГОВ: что делает алгоритм
//
//  Строка: шаг | что делают компоненты | куда дальше.
//  У обычного шага один путь (второй - STEP_NONE).
//  У развилки два пути, одинаковые по смыслу у всех развилок:
//    путь 0 - продолжить            (PATH_WAIT_GO_ON / PATH_CYCLE_REPEAT)
//    путь 1 - задержаться или выйти (PATH_WAIT_HOLD  / PATH_CYCLE_EXIT)
//
//  Компоненты (state_Magistral):
//    all_close    - всё закрыто
//    in_magistral - открыт ножевой клапан
//    skip_gate    - открыты актуатор и клапан (шлюз)
//    air_on       - открыт шаровой кран (воздух)
// =====================================================================
struct AlgStepDefinition {
    uint8_t step;          // AlgStep
    uint8_t components;    // state_Magistral
    uint8_t next[2];       // следующий шаг по пути 0 и по пути 1
};

const AlgStepDefinition ALG_STEPS[] PROGMEM = {
//   шаг                                     компоненты                    путь 0: дальше                         путь 1: ждать / выход
    // ---- комплекс ----
    {STEP_COMPLEX_WAIT_LOAD_QUEUE,          state_Magistral::all_close,    {STEP_COMPLEX_OPEN_CLAPAN,             STEP_NONE}},
    {STEP_COMPLEX_OPEN_CLAPAN,              state_Magistral::in_magistral, {STEP_COMPLEX_GATE_DOSING,             STEP_NONE}},
    {STEP_COMPLEX_GATE_DOSING,              state_Magistral::skip_gate,    {STEP_COMPLEX_AFTER_GATE,              STEP_NONE}},
    {STEP_COMPLEX_AFTER_GATE,               state_Magistral::in_magistral, {STEP_COMPLEX_CLOSE_ALL_BEFORE_AIR,    STEP_NONE}},
    {STEP_COMPLEX_CLOSE_ALL_BEFORE_AIR,     state_Magistral::all_close,    {STEP_COMPLEX_AIR_UNTIL_SOLO_START,    STEP_NONE}},
    {STEP_COMPLEX_AIR_UNTIL_SOLO_START,     state_Magistral::air_on,       {STEP_COMPLEX_AIR_WHILE_SOLO_LOADS,    STEP_NONE}},
    {STEP_COMPLEX_AIR_WHILE_SOLO_LOADS,     state_Magistral::air_on,       {STEP_START_IDLE,                      STEP_NONE}},

    // ---- соло ----
    {STEP_SOLO_OPEN_CLAPAN,                 state_Magistral::in_magistral, {STEP_SOLO_GATE_DOSING,                STEP_NONE}},
    {STEP_SOLO_GATE_DOSING,                 state_Magistral::skip_gate,    {STEP_SOLO_AFTER_GATE,                 STEP_NONE}},
    {STEP_SOLO_AFTER_GATE,                  state_Magistral::in_magistral, {STEP_SOLO_AIR,                        STEP_SOLO_AFTER_GATE}},
    {STEP_SOLO_AIR,                         state_Magistral::air_on,       {STEP_SOLO_PAUSE_BETWEEN_CYCLES,       STEP_START_IDLE}},
    {STEP_SOLO_PAUSE_BETWEEN_CYCLES,        state_Magistral::all_close,    {STEP_SOLO_OPEN_CLAPAN,                STEP_NONE}},

    // ---- триплет ----
    {STEP_TRIPLET_WAIT_LOAD_QUEUE,          state_Magistral::all_close,    {STEP_TRIPLET_OPEN_CLAPAN,             STEP_NONE}},
    {STEP_TRIPLET_OPEN_CLAPAN,              state_Magistral::in_magistral, {STEP_TRIPLET_GATE_DOSING,             STEP_NONE}},
    {STEP_TRIPLET_GATE_DOSING,              state_Magistral::skip_gate,    {STEP_TRIPLET_AFTER_GATE,              STEP_NONE}},
    {STEP_TRIPLET_AFTER_GATE,               state_Magistral::in_magistral, {STEP_TRIPLET_CLOSE_ALL_BEFORE_AIR,    STEP_NONE}},
    {STEP_TRIPLET_CLOSE_ALL_BEFORE_AIR,     state_Magistral::all_close,    {STEP_TRIPLET_AIR,                     STEP_TRIPLET_CLOSE_ALL_BEFORE_AIR}},
    {STEP_TRIPLET_AIR,                      state_Magistral::air_on,       {STEP_TRIPLET_WAIT_LOAD_QUEUE,         STEP_START_IDLE}},

    // ---- продувка ----
    {STEP_PRODUVKA_AIR,                     state_Magistral::air_on,       {STEP_START_IDLE,                      STEP_NONE}},

    // ---- чистка ----
    {STEP_CLEARING_OPEN_CLAPAN,             state_Magistral::in_magistral, {STEP_CLEARING_OPEN_ACTUATOR,          STEP_NONE}},
    {STEP_CLEARING_OPEN_ACTUATOR,           state_Magistral::skip_gate,    {STEP_CLEARING_WAIT_UNTIL_STOP,        STEP_NONE}},
    {STEP_CLEARING_WAIT_UNTIL_STOP,         state_Magistral::skip_gate,    {STEP_CLEARING_CLOSE_ACTUATOR,         STEP_CLEARING_WAIT_UNTIL_STOP}},
    {STEP_CLEARING_CLOSE_ACTUATOR,          state_Magistral::in_magistral, {STEP_CLEARING_CLOSE_CLAPAN,           STEP_NONE}},
    {STEP_CLEARING_CLOSE_CLAPAN,            state_Magistral::all_close,    {STEP_START_IDLE,                      STEP_NONE}},
};

// Стартовый шаг: отдельный путь на каждый режим (номер пути = PATH_START_...)
const uint8_t ALG_START_PATHS[] PROGMEM = {
    STEP_START_IDLE,                 // PATH_START_STAY - остаться в старте
    STEP_COMPLEX_WAIT_LOAD_QUEUE,    // PATH_START_COMPLEX
    STEP_SOLO_OPEN_CLAPAN,           // PATH_START_SOLO
    STEP_CLEARING_OPEN_CLAPAN,       // PATH_START_CLEARING
    STEP_TRIPLET_WAIT_LOAD_QUEUE,    // PATH_START_TRIPLET
    STEP_PRODUVKA_AIR,               // PATH_START_PRODUVKA
};

constexpr uint8_t ALG_STEP_COUNT = sizeof(ALG_STEPS) / sizeof(ALG_STEPS[0]);
constexpr uint8_t ALG_START_PATH_COUNT = sizeof(ALG_START_PATHS);
constexpr uint8_t ALG_NODE_COUNT = ALG_STEP_COUNT + 1;    // все шаги таблицы + стартовый

// Все узлы графа
struct Data_alg {
    one_state_Magistral *nodes[ALG_NODE_COUNT] = {};    // [0] - стартовый, дальше в порядке ALG_STEPS

    // узел по номеру шага: data_alg[STEP_SOLO_AIR]; nullptr - такого шага нет
    one_state_Magistral *operator[](uint8_t step) const {
        for (uint8_t i = 0; i < ALG_NODE_COUNT; ++i) {
            if (nodes[i] != nullptr && nodes[i]->get_id() == step) {
                return nodes[i];
            }
        }
        return nullptr;
    }
};


// =====================================================================
//  3. ВРЕМЕНА ШАГОВ: сколько длится каждый шаг
//
//  Все времена берутся из настроек (settings.hpp) и констант config.hpp.
//  Пересчитываются при старте и после сохранения настроек.
//  Проверить на плате: команда «cfg times».
//
//  Комплекс и триплет, магистрали 1-3:
//    грузятся по очереди: 2-я начинает через одну «загрузку магистрали»
//    после 1-й, 3-я - через две. После шлюза ранние магистрали держат клапан
//    открытым, пока грузятся остальные, и к воздуху все три приходят вместе.
//
//                     |<- загрузка одной ->|<- загрузка одной ->|<- загрузка одной ->|
//      магистраль 1:  [клапан][шлюз][после шлюза: закрытие, ждёт загрузку 2-й и 3-й ...][воздух
//      магистраль 2:  [очередь .............][клапан][шлюз][после шлюза, ждёт 3-ю .....][воздух
//      магистраль 3:  [очередь ...................................][клапан][шлюз][после][воздух
//
//      «загрузка одной» = клапан (T_COMPLEX_PRED_SKIP) + шлюз (порция) + после шлюза
// =====================================================================

// Величины, из которых складываются времена шагов
struct AlgStepTimings {
    uint32_t actuator_stroke_ms;              // полный ход актуатора (настройка «act»)
    uint32_t actuator_extra_ms;               // на сколько ход длиннее 1 с (константы конфига рассчитаны на 1 с)
    uint32_t after_gate_ms;                   // после шлюза: актуатор успевает закрыться
    uint32_t gate_complex_ms;                 // шлюз = порция + задержка подачи питания на мотор
    uint32_t gate_triplet_ms;
    uint32_t gate_solo_ms;
    uint32_t load_one_magistral_complex_ms;   // сколько длится загрузка одной магистрали
    uint32_t load_one_magistral_triplet_ms;
};

AlgStepTimings alg_calc_timings(const FlowSettings &settings) {
    AlgStepTimings t;

    t.actuator_stroke_ms = (uint32_t)settings.act_ds * 100;
    t.actuator_extra_ms = 0;
    if (t.actuator_stroke_ms > 1000) {
        t.actuator_extra_ms = t.actuator_stroke_ms - 1000;
    }
    t.after_gate_ms = T_COMPLEX_POST_SKIP + t.actuator_extra_ms;

    t.gate_complex_ms = (uint32_t)settings.cx_portion * 100 + T_DOSE_START;
    t.gate_triplet_ms = (uint32_t)settings.tr_portion * 100 + T_DOSE_START;
    t.gate_solo_ms = (uint32_t)settings.so_portion * 100 + T_DOSE_START;

    // загрузка одной магистрали: открыть клапан + шлюз + после шлюза
    t.load_one_magistral_complex_ms = T_COMPLEX_PRED_SKIP + t.gate_complex_ms + t.after_gate_ms;
    t.load_one_magistral_triplet_ms = T_COMPLEX_PRED_SKIP + t.gate_triplet_ms + t.after_gate_ms;
    return t;
}

// Магистрали 1-3 (номера 0..2) - группа комплекса и триплета; 4-я (номер 3) - соло
bool is_group_magistral(uint8_t mag) {
    return mag < kol_mag_group;
}

// Ожидание своей очереди загрузки: 1-я сразу, 2-я через одну загрузку, 3-я через две
uint32_t load_queue_wait_ms(uint8_t mag, uint32_t load_one_magistral_ms) {
    uint32_t wait = T_COMPLEX_PRED_LOAD;
    if (is_group_magistral(mag)) {
        wait += mag * load_one_magistral_ms;
    }
    return wait;
}

// После шлюза: закрыть актуатор и дождаться, пока загрузятся магистрали после этой
uint32_t after_gate_with_queue_ms(uint8_t mag, uint32_t after_gate_ms, uint32_t load_one_magistral_ms) {
    if (!is_group_magistral(mag)) {
        return after_gate_ms;                                   // 4-я грузится одна, ждать некого
    }
    uint8_t magistrals_after_me = kol_mag_group - 1 - mag;     // 1-я: 2, 2-я: 1, 3-я: 0
    return after_gate_ms + magistrals_after_me * load_one_magistral_ms + T_COMPLEX_POST_LOAD;
}

// Всё закрыто перед воздухом: у 4-й шага нет (время 0 = шаг пропускается)
uint32_t close_all_before_air_ms(uint8_t mag) {
    if (!is_group_magistral(mag)) {
        return 0;
    }
    return T_COMPLEX_ALL_CLOSE_PRED_AIR;
}

// Воздух комплекса у магистралей 1-3 делится на два шага:
//   «до старта соло» + «пока грузится соло».
// Второй шаг равен загрузке одной магистрали: за это время грузится 4-я
// (соло-часть), и её воздух начинается сразу после воздуха группы.
// У 4-й магистрали весь воздух - во втором шаге.
uint32_t complex_air_while_solo_loads_ms(uint8_t mag, const AlgStepTimings &t, const FlowSettings &settings) {
    uint32_t air_ms = (uint32_t)settings.cx_air[mag] * 1000;
    if (!is_group_magistral(mag)) {
        return air_ms;
    }
    if (air_ms < t.load_one_magistral_complex_ms) {
        return air_ms;                                          // воздуха меньше, чем грузится соло
    }
    return t.load_one_magistral_complex_ms;
}

uint32_t complex_air_until_solo_start_ms(uint8_t mag, const AlgStepTimings &t, const FlowSettings &settings) {
    if (!is_group_magistral(mag)) {
        return 0;
    }
    uint32_t air_ms = (uint32_t)settings.cx_air[mag] * 1000;
    return air_ms - complex_air_while_solo_loads_ms(mag, t, settings);
}

// Воздух триплета: 4-я магистраль в триплете не участвует
uint32_t triplet_air_ms(uint8_t mag, const FlowSettings &settings) {
    if (!is_group_magistral(mag)) {
        return 0;
    }
    return (uint32_t)settings.tr_air[mag] * 1000;
}

// Длительность шага step для магистрали mag (0..3), мс
uint32_t alg_step_duration_ms(uint8_t step, uint8_t mag, const AlgStepTimings &t, const FlowSettings &settings) {
    switch (step) {
        case STEP_START_IDLE:                     return 10;

        // ---- комплекс ----
        case STEP_COMPLEX_WAIT_LOAD_QUEUE:        return load_queue_wait_ms(mag, t.load_one_magistral_complex_ms);
        case STEP_COMPLEX_OPEN_CLAPAN:            return T_COMPLEX_PRED_SKIP;
        case STEP_COMPLEX_GATE_DOSING:            return t.gate_complex_ms;
        case STEP_COMPLEX_AFTER_GATE:             return after_gate_with_queue_ms(mag, t.after_gate_ms, t.load_one_magistral_complex_ms);
        case STEP_COMPLEX_CLOSE_ALL_BEFORE_AIR:   return close_all_before_air_ms(mag);
        case STEP_COMPLEX_AIR_UNTIL_SOLO_START:   return complex_air_until_solo_start_ms(mag, t, settings);
        case STEP_COMPLEX_AIR_WHILE_SOLO_LOADS:   return complex_air_while_solo_loads_ms(mag, t, settings);

        // ---- соло ----
        case STEP_SOLO_OPEN_CLAPAN:               return T_SOLO_PRED_SKIP;
        case STEP_SOLO_GATE_DOSING:               return t.gate_solo_ms;
        case STEP_SOLO_AFTER_GATE:                return T_SOLO_POST_SKIP + t.actuator_extra_ms;
        case STEP_SOLO_AIR:                       return (uint32_t)settings.so_air * 1000;
        case STEP_SOLO_PAUSE_BETWEEN_CYCLES:      return T_SOLO_WAIT_AIROFF;

        // ---- триплет ----
        case STEP_TRIPLET_WAIT_LOAD_QUEUE:        return load_queue_wait_ms(mag, t.load_one_magistral_triplet_ms);
        case STEP_TRIPLET_OPEN_CLAPAN:            return T_COMPLEX_PRED_SKIP;
        case STEP_TRIPLET_GATE_DOSING:            return t.gate_triplet_ms;
        case STEP_TRIPLET_AFTER_GATE:             return after_gate_with_queue_ms(mag, t.after_gate_ms, t.load_one_magistral_triplet_ms);
        case STEP_TRIPLET_CLOSE_ALL_BEFORE_AIR:   return close_all_before_air_ms(mag);
        case STEP_TRIPLET_AIR:                    return triplet_air_ms(mag, settings);

        // ---- продувка ----
        case STEP_PRODUVKA_AIR:                   return (uint32_t)settings.pv_s * 1000;

        // ---- чистка ----
        case STEP_CLEARING_OPEN_CLAPAN:           return T_CLEARING_GAP_CLAP + T_CLEARING_TOGGLE_CLAP;
        case STEP_CLEARING_OPEN_ACTUATOR:         return t.actuator_stroke_ms + T_CLEARING_GAP_ACT;
        case STEP_CLEARING_WAIT_UNTIL_STOP:       return 10;
        case STEP_CLEARING_CLOSE_ACTUATOR:        return t.actuator_stroke_ms + T_CLEARING_GAP_ACT;
        case STEP_CLEARING_CLOSE_CLAPAN:          return T_CLEARING_GAP_CLAP + T_CLEARING_TOGGLE_CLAP;
    }
    return 0;
}

// Записать времена всех шагов для всех магистралей.
// Вызывает Flow::apply_settings() при старте и после изменения настроек.
void alg_apply_step_durations(Data_alg &alg, const FlowSettings &settings) {
    AlgStepTimings t = alg_calc_timings(settings);
    for (uint8_t i = 0; i < ALG_NODE_COUNT; ++i) {
        one_state_Magistral *node = alg.nodes[i];
        if (node == nullptr) {
            continue;
        }
        for (uint8_t mag = 0; mag < kol_all_mag; ++mag) {
            uint32_t ms = alg_step_duration_ms(node->get_id(), mag, t, settings);
            node->set_time_in_this(ms, mag);
        }
    }
}


// =====================================================================
//  4. НАЗВАНИЯ ШАГОВ (для вывода «cfg times»)
// =====================================================================
const __FlashStringHelper *alg_step_name(uint8_t step) {
    switch (step) {
        case STEP_START_IDLE:                     return F("старт: ожидание режима");

        case STEP_COMPLEX_WAIT_LOAD_QUEUE:        return F("комплекс: ожидание очереди загрузки");
        case STEP_COMPLEX_OPEN_CLAPAN:            return F("комплекс: открыт клапан");
        case STEP_COMPLEX_GATE_DOSING:            return F("комплекс: шлюз (порция)");
        case STEP_COMPLEX_AFTER_GATE:             return F("комплекс: после шлюза");
        case STEP_COMPLEX_CLOSE_ALL_BEFORE_AIR:   return F("комплекс: всё закрыто перед воздухом");
        case STEP_COMPLEX_AIR_UNTIL_SOLO_START:   return F("комплекс: воздух до старта соло");
        case STEP_COMPLEX_AIR_WHILE_SOLO_LOADS:   return F("комплекс: воздух, пока грузится соло");

        case STEP_SOLO_OPEN_CLAPAN:               return F("соло: открыт клапан");
        case STEP_SOLO_GATE_DOSING:               return F("соло: шлюз (порция)");
        case STEP_SOLO_AFTER_GATE:                return F("соло: после шлюза");
        case STEP_SOLO_AIR:                       return F("соло: воздух");
        case STEP_SOLO_PAUSE_BETWEEN_CYCLES:      return F("соло: пауза между циклами");

        case STEP_TRIPLET_WAIT_LOAD_QUEUE:        return F("триплет: ожидание очереди загрузки");
        case STEP_TRIPLET_OPEN_CLAPAN:            return F("триплет: открыт клапан");
        case STEP_TRIPLET_GATE_DOSING:            return F("триплет: шлюз (порция)");
        case STEP_TRIPLET_AFTER_GATE:             return F("триплет: после шлюза");
        case STEP_TRIPLET_CLOSE_ALL_BEFORE_AIR:   return F("триплет: всё закрыто перед воздухом");
        case STEP_TRIPLET_AIR:                    return F("триплет: воздух");

        case STEP_PRODUVKA_AIR:                   return F("продувка: воздух");

        case STEP_CLEARING_OPEN_CLAPAN:           return F("чистка: открыть клапан");
        case STEP_CLEARING_OPEN_ACTUATOR:         return F("чистка: открыть актуатор");
        case STEP_CLEARING_WAIT_UNTIL_STOP:       return F("чистка: ожидание стопа");
        case STEP_CLEARING_CLOSE_ACTUATOR:        return F("чистка: закрыть актуатор");
        case STEP_CLEARING_CLOSE_CLAPAN:          return F("чистка: закрыть клапан");
    }
    return F("?");
}


// =====================================================================
//  5. ПОСТРОЕНИЕ ГРАФА ПО ТАБЛИЦЕ
//
//  Вызывается один раз при старте (Flow::init). Времена шагов здесь
//  не задаются - их записывает alg_apply_step_durations().
// =====================================================================

// Строка таблицы ALG_STEPS (таблица лежит во flash, читаем через memcpy_P)
AlgStepDefinition alg_read_step_definition(uint8_t index) {
    AlgStepDefinition def;
    memcpy_P(&def, &ALG_STEPS[index], sizeof(def));
    return def;
}

// Сколько путей у шага: у развилки 2, у обычного шага 1
uint8_t alg_paths_count(const AlgStepDefinition &def) {
    if (def.next[1] == STEP_NONE) {
        return 1;
    }
    return 2;
}

void alg_report_error(const __FlashStringHelper *text, uint8_t step) {
    d_print(F("ERROR alg: "));
    d_print(text);
    d_println((int)step);
}

// Шаг 1: создать узлы - стартовый и по одному на каждую строку таблицы
void alg_create_nodes(uint8_t magistrals_count, Data_alg &alg) {
    one_state_Magistral *start = new one_state_Magistral(ALG_START_PATH_COUNT, magistrals_count, STEP_START_IDLE);
    start->set_curr_state(state_Magistral::start_state);
    alg.nodes[0] = start;

    for (uint8_t i = 0; i < ALG_STEP_COUNT; ++i) {
        AlgStepDefinition def = alg_read_step_definition(i);
        one_state_Magistral *node = new one_state_Magistral(alg_paths_count(def), magistrals_count, def.step);
        node->set_curr_state((state_Magistral)def.components);
        alg.nodes[i + 1] = node;
    }
}

// Шаг 2: номера шагов не должны повторяться
bool alg_check_unique_steps(const Data_alg &alg) {
    bool ok = true;
    for (uint8_t i = 0; i < ALG_NODE_COUNT; ++i) {
        for (uint8_t j = i + 1; j < ALG_NODE_COUNT; ++j) {
            if (alg.nodes[i]->get_id() == alg.nodes[j]->get_id()) {
                alg_report_error(F("повтор номера шага "), alg.nodes[i]->get_id());
                ok = false;
            }
        }
    }
    return ok;
}

// Шаг 3: связать переходы
bool alg_link_paths(Data_alg &alg) {
    bool ok = true;

    // из стартового - по пути на каждый режим
    for (uint8_t path = 0; path < ALG_START_PATH_COUNT; ++path) {
        uint8_t next_step = pgm_read_byte(&ALG_START_PATHS[path]);
        one_state_Magistral *next = alg[next_step];
        if (next == nullptr) {
            alg_report_error(F("старт ведёт в несуществующий шаг "), next_step);
            ok = false;
            continue;
        }
        alg.nodes[0]->set_path(path, next);
    }

    // из каждого шага таблицы - по её колонкам «путь 0» и «путь 1»
    for (uint8_t i = 0; i < ALG_STEP_COUNT; ++i) {
        AlgStepDefinition def = alg_read_step_definition(i);
        for (uint8_t path = 0; path < 2; ++path) {
            uint8_t next_step = def.next[path];
            if (next_step == STEP_NONE) {
                continue;
            }
            one_state_Magistral *next = alg[next_step];
            if (next == nullptr) {
                alg_report_error(F("переход в несуществующий шаг из шага "), def.step);
                ok = false;
                continue;
            }
            alg.nodes[i + 1]->set_path(path, next);
        }
    }
    return ok;
}

// Построить граф. false - ошибка в таблице (сообщение в Serial)
bool setup_alg_magistral(uint8_t magistrals_count, Data_alg &alg) {
    alg_create_nodes(magistrals_count, alg);
    bool unique_ok = alg_check_unique_steps(alg);
    bool links_ok = alg_link_paths(alg);
    return unique_ok && links_ok;
}


// =====================================================================
//  6. РЕЖИМЫ МАГИСТРАЛИ
//  Какой путь выбирается на развилках в каждом режиме:
//  Magistral::set_current_mode_alg() (magistral.hpp)
// =====================================================================
enum class state_Alg_mag {
    stop,               // доработать текущий цикл и остаться в старте
    prepare_solo,       // загрузить соло и ждать в магистрали
    prepare_triplet,    // загрузить триплет и ждать перед воздухом
    one_cycle_solo,
    one_cycle_complex,
    one_cycle_triplet,
    cycle_solo,         // соло по кругу
    cycle_triplet,      // триплет по кругу
    produvka,
    clearing
};

#endif // ALG_STATE_MAG_HPP
