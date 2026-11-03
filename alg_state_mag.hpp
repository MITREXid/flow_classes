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
    clearing,
    retry_one_cycle_solo_and_wait  // соло с повтором: цикл, пауза so.rep в шаге 26, снова цикл - до "стоп"
};

#endif // ALG_STATE_MAG_HPP
