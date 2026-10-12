#ifndef DISPLAY_HPP
#define DISPLAY_HPP
#include "flow.hpp"
#include "logger.hpp"

/**
 * Экран Nextion 3.5" 480x320, проект "FLOW screen v2".
 * Картинки и расстановка компонентов - см. FLOW_screen_v2/Сборка экрана.html
 *
 * Экран -> Arduino (как раньше):
 *   A0 - комплекс, A1 - соло, A2 - чистка, A3 - триплет
 *   @* - остановить активный режим (в том числе продувку)
 *   B1 - старт продувки, B0 - конец продувки
 *   E0 - (новое) сброс значка ошибки
 *   0x88 - экран включился/перезагрузился (Nextion шлёт сам) -> перерисовать
 *   P1   - открыта главная страница -> перерисовать
 *   R0   - открыто меню настроек (page2)
 *   M + '0'..'6' - выбрана программа в меню: комплекс, соло, триплет, вулкан, чистка, продувка, общие
 *   T0   - открыта страница программы (page3) -> нарисовать её
 *   K + строка '0'..'4' + '-'/'+' - нажата кнопка изменения (держат - повторяем сами), K X X - отпущена
 *   N + 'P'/'N' - страница программы назад/вперёд, N S - сохранить, N B - назад в меню
 *
 * page3: p0 - фон (картинка страницы с подписями, SET_PAGES[].pic), t0..t4 - значения (Arduino),
 *   p1 - плашка "Сохранено" / "Не сохранено" (картинки 17/18), прячется через 2 с
 *
 * Arduino -> экран (новое): страница page1 ("main")
 *   p4.pic   - строка состояния (картинки 6..14)
 *   q0..q3.picc - плитки магистралей: 1 ожидание, 2 загрузка, 3 воздух, 4 чистка
 *   bt0..bt5.val - подсветка активного режима (bt0 комплекс, bt1 соло, bt2 триплет, bt3 чистка,
 *                  bt4 продувка, bt5 вулкан)
 *   tsw bt0..bt5 - блокировка кнопок режимов, пока FLOW работает; bt5 (вулкан) пока выключена всегда
 *   vis p5 - мигающая рыжая точка "что-то не так" (журнал - по USB)
 *   t0..t3.txt - обратный отсчёт текущего шага магистрали ("25 с"), пусто в ожидании
 *   t0..t3.picc / .pco - фон и цвет текста под состояние плитки
 *   t4.txt - давление ("12.5 кПа" или "-- кПа")
 * Текст на экран идёт в кодировке koi8-r (как в проекте экрана).
 */

// Цвета текста RGB565
#define SCR_COLOR_INK 12678     // #333333 - на оранжевой плитке
#define SCR_COLOR_ORANGE 62570  // #f18c54 - на тёмной плитке
// "с" и "кПа" в koi8-r
#define SCR_KOI8_S "\xD3"
#define SCR_KOI8_KPA "\xCB\xF0\xC1"

// Номера картинок в проекте экрана
#define SCR_PIC_TILE_IDLE 1
#define SCR_PIC_TILE_LOAD 2
#define SCR_PIC_TILE_AIR 3
#define SCR_PIC_TILE_CLEAN 4
#define SCR_PIC_ST_READY 6
#define SCR_PIC_ST_PREPARE 7
#define SCR_PIC_ST_COMPLEX 8
#define SCR_PIC_ST_SOLO 9
#define SCR_PIC_ST_TRIPLET 10
#define SCR_PIC_ST_CLEARING 11
#define SCR_PIC_ST_AIR 12
#define SCR_PIC_ST_STOPPING 13
#define SCR_PIC_TOAST_OK 17
#define SCR_PIC_TOAST_FAIL 18
#define SCR_TOAST_MS 2000
#define SCR_REPEAT_FIRST 400   // удержание "-"/"+": первый повтор
#define SCR_REPEAT_MS 100      // и дальше
#define SCR_BLINK_MS 500       // мигание точки ошибки
#define SCR_VOLCANO_ENABLED 0  // кнопка "Вулкан" на главной: 0 - неактивна

#define SCR_REFRESH_MS 200      // как часто проверяем, что изменилось
#define SCR_FULL_REFRESH_MS 5000 // полная перерисовка (на случай сбоя экрана)
#define SCR_NOT_SENT 0xFF

class Display{
private:
    enum class events_Display{
    NOTHING,
    START_EVENT,
    STOP_EVENT,
    AIR_START,
    AIR_STOP,
    ERROR_ACK,
    SCREEN_READY,
    PAGE_MAIN,
    MENU_OPEN,
    MENU_PROG,
    PROG_OPEN,
    PROG_KEY,
    PROG_NAV
    };
    uint8_t page = 1;//какая страница сейчас на экране (главную обновляем только на page1)
    //страница программы
    uint8_t prog = 0;       //программа (SET_PROGS)
    uint8_t prog_page = 0;  //страница (SET_PAGES)
    FlowSettings draft;     //правим копию, в g_set - только по "Сохранить"
    int8_t rep_row = -1;    //удержание "-"/"+"
    int8_t rep_dir = 0;
    uint32_t t_rep = 0;
    uint32_t t_toast = 0;
    bool toast_on = false;
    uint8_t blink = 0;
    uint32_t t_blink = 0;
    void draw_prog_page();
    void send_value(uint8_t row);
    void step_value(uint8_t row, int8_t dir);
    char buf[size_of_buf] = {};  //данные, которые читаем из порта
    events_Display flag = events_Display::NOTHING;
    Flow & flow;
    uint8_t pin_rx = -1;
    uint8_t pin_tx = -1;
    uint32_t time_init;
    uint32_t time_waiting = 20000;//подготовка после включения, кнопки заблокированы
    uint32_t t_refresh = 0;
    uint32_t t_full_refresh = 0;

    //что уже отправлено на экран (SCR_NOT_SENT - неизвестно, отправить заново)
    uint8_t sent_status = SCR_NOT_SENT;
    uint8_t sent_tile[kol_all_mag];
    uint8_t sent_btn[6];
    uint8_t sent_lock[7];
    uint8_t sent_err = SCR_NOT_SENT;
    uint16_t sent_cd[kol_all_mag];//отправленный отсчёт, с (0xFFFF - неизвестно, 0 - пусто)
    uint8_t sent_cd_pic[kol_all_mag];
    int16_t sent_press = 0;
    bool sent_press_valid = false;

    events_Display Check_message();
    void mark_all_unsent();
    bool can_send(uint8_t len);
    uint8_t status_pic();
    uint8_t tile_pic(uint8_t mag);
    int8_t active_btn();
    void refresh();

public:
    Display(Flow &flow, uint8_t pin_rx = -1, uint8_t pin_tx = -1);
    void update();
    void init();
    void logic();
    void send_comand(String cmd);
};

Display::Display(Flow & flow_, uint8_t pin_rx_, uint8_t pin_tx_):
flow(flow_),
pin_rx(pin_rx_),
pin_tx(pin_tx_)
{
//Пины сделаны на будущее, ксли придется переходить на SoftwareSerial
    mark_all_unsent();
}

void Display::mark_all_unsent(){
    sent_status = SCR_NOT_SENT;
    for(uint8_t i = 0; i < kol_all_mag; ++i){ sent_tile[i] = SCR_NOT_SENT; }
    for(uint8_t i = 0; i < 6; ++i){ sent_btn[i] = SCR_NOT_SENT; }
    for(uint8_t i = 0; i < 7; ++i){ sent_lock[i] = SCR_NOT_SENT; }
    sent_err = SCR_NOT_SENT;
    for(uint8_t i = 0; i < kol_all_mag; ++i){ sent_cd[i] = 0xFFFF; sent_cd_pic[i] = SCR_NOT_SENT; }
    sent_press_valid = false;
}

void Display::update(){
    while (SerialXXX.available() > 0) {
        flag = Check_message();
        logic();
    }
    if(millis() - t_refresh >= SCR_REFRESH_MS){
        t_refresh = millis();
        if(millis() - t_full_refresh >= SCR_FULL_REFRESH_MS){
            t_full_refresh = millis();
            mark_all_unsent();
        }
        refresh();
    }
    //страница программы: удержание кнопки и плашка сохранения
    if(page == 3){
        if(rep_row >= 0 && millis() - t_rep >= SCR_REPEAT_MS){
            t_rep = millis();
            step_value(rep_row, rep_dir);
        }
        if(toast_on && millis() - t_toast >= SCR_TOAST_MS){
            toast_on = false;
            send_comand("vis p1,0");
            draw_prog_page();//под плашкой - фон и значения
        }
    }
}

void Display::init()
{
    SerialXXX.begin(9600);
    time_init = millis();

    while(millis() - time_init < 1000){}//заставка page0
    send_comand("page 1");
    mark_all_unsent();
}

void Display::send_comand(String cmd){
    SerialXXX.print(cmd);
    uint8_t ndt[3] = { 255,255,255};//0xFF, 0xFF, 0xFF};
    SerialXXX.write(ndt, 3);
}

//Не блокируем loop(): отправляем, только если команда целиком влезает в буфер порта
bool Display::can_send(uint8_t len){
    return SerialXXX.availableForWrite() >= len + 3;
}

uint8_t Display::status_pic(){
    if(millis() - time_init < time_waiting){
        return SCR_PIC_ST_PREPARE;
    }
    if(!flow.is_idle()){
        switch(flow.getStatus()){
            case state_Flow::do_complex:   return SCR_PIC_ST_COMPLEX;
            case state_Flow::do_solo:      return SCR_PIC_ST_SOLO;
            case state_Flow::do_triplet:   return SCR_PIC_ST_TRIPLET;
            case state_Flow::do_clearing:  return SCR_PIC_ST_CLEARING;
            case state_Flow::do_produvka:  return SCR_PIC_ST_AIR;
            default:                       return SCR_PIC_ST_STOPPING;
        }
    }
    return SCR_PIC_ST_READY;//ошибка - мигающей точкой p5
}

uint8_t Display::tile_pic(uint8_t mag){
    switch(flow.group[mag].getState()){
        case state_Magistral::going_to_gate:
        case state_Magistral::in_magistral:
        case state_Magistral::skip_gate:
            return SCR_PIC_TILE_LOAD;
        case state_Magistral::air_on:
            return SCR_PIC_TILE_AIR;
        case state_Magistral::full_open:
            return SCR_PIC_TILE_CLEAN;
        default:
            return SCR_PIC_TILE_IDLE;
    }
}

//bt0 комплекс, bt1 соло, bt2 триплет, bt3 чистка, bt4 продувка (bt5 вулкан - пока не работает)
int8_t Display::active_btn(){
    if(flow.is_idle()){
        return -1;
    }
    switch(flow.getStatus()){
        case state_Flow::do_complex:   return 0;
        case state_Flow::do_solo:      return 1;
        case state_Flow::do_triplet:   return 2;
        case state_Flow::do_clearing:  return 3;
        case state_Flow::do_produvka:  return 4;
        default:                       return -1;
    }
}

void Display::refresh(){
    char cmd[32];
    if(page != 1){
        return;//на странице настроек главную не трогаем
    }

    uint8_t st = status_pic();
    if(st != sent_status){
        if(!can_send(12)) return;
        snprintf(cmd, sizeof(cmd), "p4.pic=%u", st);
        send_comand(cmd);
        sent_status = st;
    }

    for(uint8_t i = 0; i < kol_all_mag; ++i){
        uint8_t t = tile_pic(i);
        if(t != sent_tile[i]){
            if(!can_send(12)) return;
            snprintf(cmd, sizeof(cmd), "q%u.picc=%u", i, t);
            send_comand(cmd);
            sent_tile[i] = t;
        }
    }

    int8_t act = active_btn();
    for(uint8_t i = 0; i < 6; ++i){
        uint8_t v = (act == i) ? 1 : 0;
        if(v != sent_btn[i]){
            if(!can_send(12)) return;
            snprintf(cmd, sizeof(cmd), "bt%u.val=%u", i, v);
            send_comand(cmd);
            sent_btn[i] = v;
        }
    }

    //кнопки режимов доступны, только когда FLOW стоит и подготовка закончилась
    uint8_t lock = (millis() - time_init < time_waiting || !flow.is_idle()) ? 1 : 0;
    for(uint8_t i = 0; i < 7; ++i){//bt0..bt5 и m0 (вход в настройки)
        uint8_t l = (i == 5 && !SCR_VOLCANO_ENABLED) ? 1 : lock;
        if(l != sent_lock[i]){
            if(!can_send(10)) return;
            if(i < 6){
                snprintf(cmd, sizeof(cmd), "tsw bt%u,%u", i, l ? 0 : 1);
            }else{
                snprintf(cmd, sizeof(cmd), "tsw m0,%u", l ? 0 : 1);
            }
            send_comand(cmd);
            sent_lock[i] = l;
        }
    }

    //обратный отсчёт на плитках
    for(uint8_t i = 0; i < kol_all_mag; ++i){
        uint8_t t = tile_pic(i);
        if(t != sent_cd_pic[i]){
            if(!can_send(30)) return;
            snprintf(cmd, sizeof(cmd), "t%u.picc=%u", i, t);
            send_comand(cmd);
            snprintf(cmd, sizeof(cmd), "t%u.pco=%u", i, t == SCR_PIC_TILE_LOAD ? SCR_COLOR_INK : SCR_COLOR_ORANGE);
            send_comand(cmd);
            sent_cd_pic[i] = t;
            sent_cd[i] = 0xFFFF;
        }
        uint16_t sec = 0;
        if(t != SCR_PIC_TILE_IDLE){
            uint32_t rem = flow.group[i].get_remaining_ms();
            sec = (rem + 999) / 1000;
        }
        if(sec != sent_cd[i]){
            if(!can_send(18)) return;
            if(sec){
                snprintf(cmd, sizeof(cmd), "t%u.txt=\"%u " SCR_KOI8_S "\"", i, sec);
            }else{
                snprintf(cmd, sizeof(cmd), "t%u.txt=\"\"", i);
            }
            send_comand(cmd);
            sent_cd[i] = sec;
        }
    }

    //давление
    int16_t p = flow.get_pressure_x10();
    bool p_valid = (p != INT16_MIN);
    if(!sent_press_valid || p != sent_press){
        if(!can_send(22)) return;
        if(p_valid){
            int16_t a = p < 0 ? -p : p;
            snprintf(cmd, sizeof(cmd), "t4.txt=\"%s%d.%d " SCR_KOI8_KPA "\"", p < 0 ? "-" : "", a / 10, a % 10);
        }else{
            snprintf(cmd, sizeof(cmd), "t4.txt=\"-- " SCR_KOI8_KPA "\"");
        }
        send_comand(cmd);
        sent_press = p;
        sent_press_valid = true;
    }

    //точка ошибки мигает
    if(log_error_flag && millis() - t_blink >= SCR_BLINK_MS){
        t_blink = millis();
        blink ^= 1;
    }
    uint8_t err = log_error_flag ? blink : 0;
    if(err != sent_err){
        if(!can_send(10)) return;
        snprintf(cmd, sizeof(cmd), "vis p5,%u", err);
        send_comand(cmd);
        sent_err = err;
    }
}

void Display::logic()
{
    log_cmd_source = 0;//команды с экрана
    if (flag == events_Display::AIR_STOP) {
        flow.stop();
    }

    if (flag == events_Display::STOP_EVENT) {
        flow.stop();
    }

    if (flag == events_Display::START_EVENT && millis() - time_init >= time_waiting) {
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

    if (flag == events_Display::AIR_START && millis() - time_init >= time_waiting) {
        flow.start(state_Flow::do_produvka);
    }

    if (flag == events_Display::ERROR_ACK) {
        log_error_flag = false;
    }

    if (flag == events_Display::SCREEN_READY) {
        send_comand("page 1");
        page = 1;
        mark_all_unsent();
    }

    if (flag == events_Display::PAGE_MAIN) {
        page = 1;
        mark_all_unsent();
    }

    if (flag == events_Display::MENU_OPEN) {
        page = 2;
        rep_row = -1;
    }

    if (flag == events_Display::MENU_PROG) {
        uint8_t p = buf[1] - '0';
        if(p < SET_PROG_N){
            prog = p;
            prog_page = SET_PROGS[p].page;
            draft = g_set;
            send_comand("page 3");
        }
    }

    if (flag == events_Display::PROG_OPEN) {
        page = 3;
        toast_on = false;
        draw_prog_page();
    }

    if (flag == events_Display::PROG_KEY) {
        if(buf[1] >= '0' && buf[1] <= '4' && (buf[2] == '-' || buf[2] == '+')){
            rep_row = buf[1] - '0';
            rep_dir = buf[2] == '+' ? 1 : -1;
            step_value(rep_row, rep_dir);
            t_rep = millis() + SCR_REPEAT_FIRST - SCR_REPEAT_MS;
        }else{
            rep_row = -1;//отпустили
        }
    }

    if (flag == events_Display::PROG_NAV) {
        const SetProg &pr = SET_PROGS[prog];
        rep_row = -1;
        switch(buf[1]){
            case 'P':
                if(prog_page > pr.page){ --prog_page; draw_prog_page(); }
                break;
            case 'N':
                if(prog_page + 1 < pr.page + pr.pages){ ++prog_page; draw_prog_page(); }
                break;
            case 'S': {
                bool ok = flow.change_settings(draft);//FLOW работает - не примет (и запишет в журнал)
                if(ok){ draft = g_set; }
                draw_prog_page();//показать то, что реально сохранено
                char cmd[16];
                snprintf(cmd, sizeof(cmd), "p1.pic=%u", ok ? SCR_PIC_TOAST_OK : SCR_PIC_TOAST_FAIL);
                send_comand(cmd);
                send_comand("vis p1,1");
                toast_on = true;
                t_toast = millis();
                break;
            }
            case 'B':
                send_comand("page 2");
                break;
        }
    }
    if (flag != events_Display::NOTHING) {
        mark_all_unsent();//после нажатия сразу показать настоящее состояние (кнопка могла переключиться сама)
    }
    flag = events_Display::NOTHING;
}


//Страница программы: сначала прячем лишние строки, потом фон, потом значения
void Display::draw_prog_page(){
    char cmd[20];
    const SetPage &pg = SET_PAGES[prog_page];
    for(uint8_t r = pg.count; r < 5; ++r){
        snprintf(cmd, sizeof(cmd), "vis t%u,0", r);
        send_comand(cmd);
    }
    snprintf(cmd, sizeof(cmd), "p0.pic=%u", pg.pic);
    send_comand(cmd);
    for(uint8_t r = 0; r < pg.count; ++r){
        snprintf(cmd, sizeof(cmd), "vis t%u,1", r);
        send_comand(cmd);
        send_value(r);
    }
}

void Display::send_value(uint8_t row){
    const SetPage &pg = SET_PAGES[prog_page];
    if(row >= pg.count){ return; }
    char v[8], cmd[24];
    const uint8_t i = pg.first + row;
    settings_fmt(v, i, set_ref(draft, i));
    snprintf(cmd, sizeof(cmd), "t%u.txt=\"%s\"", row, v);
    send_comand(cmd);
}

//Шаг "-"/"+" по строке страницы (в пределах параметра)
void Display::step_value(uint8_t row, int8_t dir){
    const SetPage &pg = SET_PAGES[prog_page];
    if(row >= pg.count){ rep_row = -1; return; }
    const uint8_t i = pg.first + row;
    uint8_t &v = set_ref(draft, i);
    if(dir > 0 && v < SET_PARAMS[i].hi){ ++v; }
    else if(dir < 0 && v > SET_PARAMS[i].lo){ --v; }
    else{ rep_row = -1; return; }//упёрлись в предел
    send_value(row);
}

Display::events_Display Display::Check_message() {

    int length_of_message;
    char command = SerialXXX.peek();
    //    d_print(F("\n\nDISPLAY: "));

    switch (command) {

        case 0x41:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
            d_println(F("START_EVENT 0x41"));
            return events_Display::START_EVENT;
            break;

        case 0x40:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
            d_println(F("STOP_EVENT 0x40"));
            return events_Display::STOP_EVENT;
            break;

        case 0x42:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
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

        case 0x50://P1 - открыта главная
            SerialXXX.readBytes(buf, 2);
            return events_Display::PAGE_MAIN;

        case 0x52://R0 - открыто меню настроек
            SerialXXX.readBytes(buf, 2);
            return events_Display::MENU_OPEN;

        case 0x4D://M + номер программы
            SerialXXX.readBytes(buf, 2);
            return events_Display::MENU_PROG;

        case 0x54://T0 - открыта страница программы
            SerialXXX.readBytes(buf, 2);
            return events_Display::PROG_OPEN;

        case 0x4B://K + строка + направление
            if(SerialXXX.readBytes(buf, 3) == 3){
                return events_Display::PROG_KEY;
            }
            return events_Display::NOTHING;

        case 0x4E://N + P/N/S/B
            SerialXXX.readBytes(buf, 2);
            return events_Display::PROG_NAV;

        case 0x45:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
            d_println(F("ERROR_ACK 0x45"));
            return events_Display::ERROR_ACK;
            break;

        case 0x68:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
            d_println(F("START_EVENT 0x68"));
            return events_Display::START_EVENT;
            break;

        case 0x72:
            length_of_message = 2;
            SerialXXX.readBytes(buf, length_of_message);
            d_println(F("STOP_EVENT 0x72"));
            return events_Display::STOP_EVENT;
            break;

        case (char)0x88://экран включился (Nextion: 00 00 00 FF FF FF 88 FF FF FF)
            SerialXXX.read();
            d_println(F("SCREEN READY 0x88"));
            return events_Display::SCREEN_READY;
            break;

        default:
            SerialXXX.read(); //чтобы не застревать на нераспознанных командах
            return events_Display::NOTHING;
            break;
    }
    return events_Display::NOTHING;
}

#endif // DISPLAY_HPP
