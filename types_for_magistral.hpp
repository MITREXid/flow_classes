#ifndef TYPES_FOR_MAGISTRAL_HPP
#define TYPES_FOR_MAGISTRAL_HPP

typedef uint32_t Time_in_this;

enum state_Magistral {
    //см. all_close
    start_state = 0,
    //высыпаем зерно в шлюз
    //(act = open, clap = close, ball = close)
    going_to_gate = 1,
    //все закрто
    //(act = close, clap = close, ball = close)
    all_close = 2,
    //в этом стотоянии ждем пока всё высыыпется в трубу
    //(act = close, clap = open, ball = close)
    in_magistral = 3,
    //в этом состоянии мы открываем шаровый (закрываем всё
    // остальное) и ждем пока зерно дойдет до конца магистали
    //(act = close, clap = close, ball = open)
    air_on = 4,
    //открыты актуатор и клапан
    //(act = open, clap = open, ball = close)
    skip_gate = 5,
    //для режима прочистки
    //(act = open, clap = open, ball = open)
    full_open = 11,
    //неопределенномсть, приводит к start_state
    undefine = 99
};

// =====================================================================
//  Номера шагов алгоритма магистрали (вершин графа в alg_state_mag.hpp).
//  Номера заданы явно и не зависят от порядка строк: новый шаг получает
//  свободный номер в десятке своего режима, остальные номера не сдвигаются.
//  Номер виден в отладочном выводе и журнале, поэтому его не переиспользуем.
//  Уникальность номеров проверяется при старте (setup_alg_magistral).
// =====================================================================
enum AlgStep : uint8_t {
    STEP_NONE = 0,                               // «перехода нет» (у шага один путь)
    STEP_START_IDLE = 1,                         // стоим в стартовом состоянии, ждём режим

    // комплекс: магистрали 1-3 загружаются по очереди, затем дуют, в это время загружается 4-я (соло-часть)
    STEP_COMPLEX_WAIT_LOAD_QUEUE = 11,           // ждём своей очереди загрузки
    STEP_COMPLEX_OPEN_CLAPAN = 12,               // открыли ножевой клапан
    STEP_COMPLEX_GATE_DOSING = 13,               // шлюз: актуатор открыт на порцию
    STEP_COMPLEX_AFTER_GATE = 14,                // актуатор закрывается, клапан открыт, зерно уходит в трубу
    STEP_COMPLEX_CLOSE_ALL_BEFORE_AIR = 15,      // всё закрыто перед воздухом
    STEP_COMPLEX_AIR_UNTIL_SOLO_START = 16,      // воздух до старта соло-части
    STEP_COMPLEX_AIR_WHILE_SOLO_LOADS = 17,      // воздух, хвост: в это время загружается соло-часть

    // соло (магистраль 4)
    STEP_SOLO_OPEN_CLAPAN = 21,
    STEP_SOLO_GATE_DOSING = 22,
    STEP_SOLO_AFTER_GATE = 23,                   // развилка: дальше / ждать здесь (режим «подготовка»)
    STEP_SOLO_AIR = 24,                          // развилка: новый цикл / выход в старт
    STEP_SOLO_PAUSE_BETWEEN_CYCLES = 25,

    // триплет (магистрали 1-3). Раньше использовал те же номера 11-16, что и комплекс
    STEP_TRIPLET_WAIT_LOAD_QUEUE = 31,
    STEP_TRIPLET_OPEN_CLAPAN = 32,
    STEP_TRIPLET_GATE_DOSING = 33,
    STEP_TRIPLET_AFTER_GATE = 34,
    STEP_TRIPLET_CLOSE_ALL_BEFORE_AIR = 35,      // развилка: дальше / ждать здесь (режим «подготовка»)
    STEP_TRIPLET_AIR = 36,                       // развилка: новый цикл / выход в старт

    // продувка
    STEP_PRODUVKA_AIR = 41,

    // чистка
    STEP_CLEARING_OPEN_CLAPAN = 51,
    STEP_CLEARING_OPEN_ACTUATOR = 52,            // полный ход
    STEP_CLEARING_WAIT_UNTIL_STOP = 53,          // развилка: закрывать / ждать (идёт чистка)
    STEP_CLEARING_CLOSE_ACTUATOR = 54,
    STEP_CLEARING_CLOSE_CLAPAN = 55,
};

// Пути из стартового состояния: номер пути = режим
enum : uint8_t {
    PATH_START_STAY = 0,                         // остаться в старте
    PATH_START_COMPLEX = 1,
    PATH_START_SOLO = 2,
    PATH_START_CLEARING = 3,
    PATH_START_TRIPLET = 4,
    PATH_START_PRODUVKA = 5,
};

// Пути развилок. У всех развилок одинаково: путь 0 - продолжить, путь 1 - задержаться/выйти
enum : uint8_t {
    PATH_WAIT_GO_ON = 0,                         // точка ожидания: идти дальше
    PATH_WAIT_HOLD = 1,                          // точка ожидания: оставаться в этом шаге
    PATH_CYCLE_REPEAT = 0,                       // конец цикла: начать новый цикл
    PATH_CYCLE_EXIT = 1,                         // конец цикла: вернуться в старт
};

#endif // TYPES_FOR_MAGISTRAL_HPP
