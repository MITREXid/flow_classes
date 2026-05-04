#ifndef TYPES_FOR_MAGISTRAL_HPP
#define TYPES_FOR_MAGISTRAL_HPP

typedef uint16_t Time_in_this;

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

#endif // TYPES_FOR_MAGISTRAL_HPP
