#if(mode_work)//c++
#include "class_flow.ino"
int main(){
    time_global_start = std::chrono::system_clock::now();
    setup();
    while(1){
        loop();
    }
}
#endif