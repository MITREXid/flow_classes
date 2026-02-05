#ifndef CONFIG_HPP
#define CONFIG_HPP
#include "declorations.hpp"
// #include "component.hpp"
// #include "other_components.hpp"


struct data_for_func{
    char control_pin = 0;
    Shared_power_5V *PWM = nullptr;
    enum class type_str{
        act, 
        clapan,
        ball
    } type;
};

typedef void (*type_func)(data_for_func*);//тип указателя на функцию

void funcOpenStart(data_for_func * data) {
    d_print("Start Open ");
    if(data->type == data_for_func::type_str::act){d_println("Actuator");
    }else if(data->type == data_for_func::type_str::clapan){d_println("Clapan"); 
    }else if(data->type == data_for_func::type_str::ball){d_println("Ball_vall"); 
    }else {d_println("HZ"); }
    pinMode(data->control_pin, LOW);
    if(data->type != data_for_func::type_str::ball) data->PWM->voltageON();
}

void funcOpenEnd(data_for_func * data) {
    d_print("End Open ");
    if(data->type == data_for_func::type_str::act){d_println("Actuator");
    }else if(data->type == data_for_func::type_str::clapan){d_println("Clapan"); 
    }else if(data->type == data_for_func::type_str::ball){d_println("Ball_vall"); 
    }else {d_println("HZ"); }
    if(data->type != data_for_func::type_str::ball) data->PWM->voltageOFF();
}

void funcCloseStart(data_for_func * data) {
    d_print("Start Close ");
    if(data->type == data_for_func::type_str::act){d_println("Actuator");
    }else if(data->type == data_for_func::type_str::clapan){d_println("Clapan"); 
    }else if(data->type == data_for_func::type_str::ball){d_println("Ball_vall"); 
    }else {d_println("HZ"); }
    pinMode(data->control_pin, HIGH);
    if(data->type != data_for_func::type_str::ball) data->PWM->voltageON();
}

void funcCloseEnd(data_for_func * data) {
    d_print("End Close ");
    if(data->type == data_for_func::type_str::act){d_println("Actuator");
    }else if(data->type == data_for_func::type_str::clapan){d_println("Clapan"); 
    }else if(data->type == data_for_func::type_str::ball){d_println("Ball_vall"); 
    }else {d_println("HZ"); }
    if(data->type != data_for_func::type_str::ball) data->PWM->voltageOFF();
}

// Actuator act(state_Component::close, PWM, 10);
// Clapan act2(state_Component::open, PWM, 11);
// Ball_cran act3(state_Component::close, 12);



#endif //CONFIG_HPP