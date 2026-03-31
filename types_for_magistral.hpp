#ifndef TYPES_FOR_MAGISTRAL_HPP
#define TYPES_FOR_MAGISTRAL_HPP

typedef uint16_t Time_in_this;

enum state_Magistral {
    start_state = 0,
    going_to_gate = 1,//высыпаем зерно в шлюз
    all_close = 2,//все закрто
    in_magistral = 3,//в этом стотоянии ждем пока всё высыыпется в трубу
    air_on = 4,//в этом состоянии мы открываем шаровый (закрываем всё остальное) и ждем пока зерно дойдет до конца магистали
    skip_gate = 5,//открыты актуатор и клапан
    full_open = 11,//режим прочистки
    undefine = 99
};

#endif // TYPES_FOR_MAGISTRAL_HPP
