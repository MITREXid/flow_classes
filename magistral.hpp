#ifndef MAGISTRAL_HPP
#define MAGISTRAL_HPP
#include "universal_object.hpp"
enum state_Magistral {not_defined = 0, in_magistral = 1, air_on = 2, air_off = 3, in_gate = 4, full_open = 11};
class Magistral{//}: public Universal_object<state_Magistral>{
private:
    
    state_Magistral state;
public:
    void turn_to(state_Magistral next_state);
    void update();
};

void Magistral::update()
{
}

void Magistral::turn_to(state_Magistral next_state){
    switch(next_state){
        case state_Magistral::not_defined:

            break;
        case state_Magistral::in_magistral:

            break;
        case state_Magistral::air_on:

            break;
        case state_Magistral::air_off:

            break;
        case state_Magistral::in_gate:

            break;
        case state_Magistral::full_open:

            break;
    };
}

#endif // MAGISTRAL_HPP
