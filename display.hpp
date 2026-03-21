#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include "flow.hpp"


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

#define size_of_buf 15


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
    events_Display Check_message();

public:
    Display(Flow &flow, uint8_t pin_rx = -1, uint8_t pin_tx = -1);
    void update();
    void init();
    void logic();
};

Display::Display(Flow & flow_, uint8_t pin_rx_, uint8_t pin_tx_):
flow(flow_),
pin_rx(pin_rx_),
pin_tx(pin_tx_)
{
//Пины сделаны на будущее, ксли придется переходить на SoftwareSerial
}

void Display::update(){
    if (Serial3.available() > 0) {
        flag = Check_message();
    }
    logic();
}

void Display::init()
{
    uint32_t time_init = millis();

    while(millis() - time_init < 1000){}
    String cmd = "page page0";
    uint8_t ndt[3] = { 255,255,255};//0xFF, 0xFF, 0xFF};

    // cmd = "page page0";

    Serial3.print(cmd);
    Serial3.write(ndt, 3);

}

void Display::logic()
{
    if (flag == events_Display::AIR_STOP) {
        flow.stop();
    }

    if (flag == events_Display::STOP_EVENT) {
        flow.stop();
    }

    if (flag == events_Display::START_EVENT) {
        switch (buf[1]) {
            case 0x30:
                flow.start(state_Flow::do_trio);
                break;

            case 0x31:
                flow.start(state_Flow::do_solo);
                break;

            case 0x32:
                flow.start(state_Flow::do_clearing);
                break;

            case 0x33:
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
    char command = Serial3.peek();
    // Serial3.println(command, HEX);

    switch (command) {

        case 0x41:
            length_of_message = 2;
            Serial3.readBytes(buf, length_of_message);
            // Serial3.println("ало START_EVENT"); 
            return events_Display::START_EVENT;
            break;

        case 0x40:
            length_of_message = 2;
            Serial3.readBytes(buf, length_of_message);
            // Serial3.println("алооооо STOP_EVENT");
            return events_Display::STOP_EVENT;
            break;

        case 0x42:
            length_of_message = 2;
            Serial3.readBytes(buf, length_of_message);
            // Serial3.println("алооооо");

            if (buf[1] == 0x31)
                return events_Display::AIR_START;
            else if (buf[1] == 0x30)
                return events_Display::AIR_STOP;
            break;

        case 0x68:
            length_of_message = 2;
            Serial3.readBytes(buf, length_of_message);
            // buf[0] = 1;
            // buf[1] = 1;
            return events_Display::START_EVENT;
            break;

        case 0x72:
            length_of_message = 2;
            Serial3.readBytes(buf, length_of_message);
            // buf[0] = 1;
            // buf[1] = 0;
            return events_Display::STOP_EVENT;
            break;

        default:
            return events_Display::NOTHING;
            break;
    }
    return events_Display::NOTHING;
}

#endif // DISPLAY_HPP
