#ifndef TYPES_FOR_MAGISTRAL_HPP
#define TYPES_FOR_MAGISTRAL_HPP

typedef int Time_in_this;

enum state_Magistral {
    going_to_gate = 1,//высыпаем зерно в шлюз
    all_close = 2,//все закрто
    in_magistral = 3,//в этом стотоянии ждем пока всё высыыпется в трубу
    air_on = 4,//в этом состоянии мы открываем шаровый (закрываем всё остальное) и ждем пока зерно дойдет до конца магистали
    full_open = 11//режим прочистки
};

#endif // TYPES_FOR_MAGISTRAL_HPP
