#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include "flow.hpp"
#include "display.hpp"

/**
 * A0 - режим троицы
 * A1 - соло режим
 * A2 - режим чистки
 * @* - остановить активный режим(в том счисле и продувку)
 * B1 - старт продувки
 * B0 - конец продувки
 * 
 * 
 * 
 * 
 * 
 */



class Display{
private:
    enum class events_Display{
    NOTHING,
    START_EVENT,
    STOP_EVENT,
    AIR_START,
    AIR_STOP
    };
    char buf[size_of_buf] = {};  //данные, которые читаем из порта
    events_Display flag;
    Flow & flow;
    uint8_t pin_rx = -1;
    uint8_t pin_tx = -1;
    uint32_t time_init;
    bool flag_waiting = false;
    uint32_t time_waiting = 17000;
    events_Display Check_message();

public:
    Display(Flow &flow, uint8_t pin_rx = -1, uint8_t pin_tx = -1);
    void update();
    void init();
    void logic();
    void lock_btn(bool lock=true);
    void send_comand(String cmd);
};

Display::Display(Flow & flow_, uint8_t pin_rx_, uint8_t pin_tx_):
flow(flow_),
pin_rx(pin_rx_),
pin_tx(pin_tx_)
{
//Пины сделаны на будущее, ксли придется переходить на SoftwareSerial
}

void Display::update(){
    if (SerialXXX.available() > 0) {
        flag = Check_message();
    }
    logic();
}

void Display::init()
{
    SerialXXX.begin(9600);
    time_init = millis();

    while(millis() - time_init < 1000){}
    send_comand("page page0");
    time_break(10);
    lock_btn(true);
}

//lock = true - блокировка кнопок, false - разблокировка
void Display::lock_btn(bool lock){
    if(lock){
    send_comand("tsw bt0,0");
    time_break(10);
    send_comand("tsw bt1,0");
    time_break(10);
    send_comand("tsw bt2,0");
    time_break(10);
    send_comand("tsw bt3,0");
    time_break(10);
    send_comand("tsw bt4,0");
    }else{
    send_comand("tsw bt0,1");
    time_break(10);
    send_comand("tsw bt1,1");
    time_break(10);
    send_comand("tsw bt2,1");
    time_break(10);
    send_comand("tsw bt3,1");
    time_break(10);
    send_comand("tsw bt4,1");
    }
    // for(int i = 0; i<5;++i){
    //     char num_btn = ('0'+i);
    //     send_comand(String("tsw bt")+String(&num_btn)+String(",")+String(lock==true?"0":"1"));
    //     time_break(10);
    // }
}

void Display::send_comand(String cmd){
    SerialXXX.print(cmd);
    uint8_t ndt[3] = { 255,255,255};//0xFF, 0xFF, 0xFF};
    SerialXXX.write(ndt, 3);

}

void Display::logic()
{
    if(millis() - time_init > time_waiting && !flag_waiting){
        flag_waiting = 1;
        lock_btn(false);
    }
    if (flag == events_Display::AIR_STOP) {
        flow.stop();
    }

    if (flag == events_Display::STOP_EVENT) {
        flow.stop();
    }

    if (flag == events_Display::START_EVENT) {
        switch (buf[1]) {
            case 0x30:
                flow.start(state_Flow::do_complex);
                break;

            case 0x31:
                flow.start(state_Flow::do_solo);
                break;

            case 0x32:
                flow.start(state_Flow::do_clearing);
                break;

            case 0x33:
                flow.start(state_Flow::do_triplet);
                break;
        }
    }

  if (flag == events_Display::AIR_START) {
        flow.start(state_Flow::do_produvka);
  }
    flag = events_Display::NOTHING;

}


Display::events_Display Display::Check_message() {

    int length_of_message;
    char command = SerialXXX.peek();
    //    d_print(F("\n\nDISPLAY: "));

    switch (command) {

        case 0x41:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
           // d_print(F("\n\nDISPLAY: "));
            d_println(F("START_EVENT 0x41")); 
            return events_Display::START_EVENT;
            break;

        case 0x40:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
           // d_print(F("\n\nDISPLAY: "));
            d_println(F("STOP_EVENT 0x40"));
            return events_Display::STOP_EVENT;
            break;

        case 0x42:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
           // d_print(F("\n\nDISPLAY: "));
             d_println(F("AIR_EVENT 0x42"));
            if (buf[1] == 0x31){
                d_println(F("AIR_START 0x31"));
                return events_Display::AIR_START;
            }
            else if (buf[1] == 0x30){
                d_println(F("AIR_STOP 0x30"));
                return events_Display::AIR_STOP;
            }
            break;

        case 0x68:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
           // d_print(F("\n\nDISPLAY: "));
            d_println(F("START_EVENT 0x68"));
            // buf[0] = 1;
            // buf[1] = 1;
            return events_Display::START_EVENT;
            break;

        case 0x72:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
           // d_print(F("\n\nDISPLAY: "));
            d_println(F("STOP_EVENT 0x72"));
            // buf[0] = 1;
            // buf[1] = 0;
            return events_Display::STOP_EVENT;
            break;

        default:
            SerialXXX.read(); //чтобы не застревать на нераспознанных командах
            return events_Display::NOTHING;
            break;
    }
    return events_Display::NOTHING;
}

#endif // DISPLAY_HPP
