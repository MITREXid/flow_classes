#ifndef LOGGER_HPP
#define LOGGER_HPP
// =====================================================================
//  Бортовой журнал FLOW в EEPROM ("чёрный ящик")
//
//  Пишется сам, без компьютера; переживает выключение и перезагрузки.
//  Прочитать: USB, монитор порта 1000000 бод, отправить команду
//    log    - журнал событий (команды, ходы моторов, воздух, ошибки)
//    err    - только журнал ошибок (хранится дольше, отдельная область)
//    clear  - начать оба журнала заново (старые записи просто не выводятся)
//    help   - список команд
//
//  EEPROM 4 КБ поделена на две кольцевые области:
//    события - 256 записей, ошибки и запуски - 85 записей.
//  Обычные события не могут вытеснить ошибки.
//  Запись 12 байт. Байты пишутся по одному за проход loop() (~3.4 мс на байт),
//  работа машины не тормозится. Номер записи (seq) пишется последним: если
//  питание пропадёт посреди записи, по номерам всё равно видно конец журнала.
// =====================================================================
#include "declorations.hpp"

#define FLOW_FW_VERSION "karaganda_MY_110 fix 1.8"
#define FLOW_FW_MAJOR 1
#define FLOW_FW_MINOR 4

// Раскладка записи:
//  [0-1] seq  [2] тип  [3] arg  [4-7] время с запуска, мс  [8-11] данные
enum LogType : uint8_t {
    LOG_BOOT = 1,     // arg: MCUSR (причина сброса), data: версия
    LOG_MARK = 2,     // отметка "clear"
    LOG_CMD = 3,      // arg: команда (см. log_cmd_name), data: источник 0 экран, 1 USB, 2 авто; бит 8 - отклонена
    LOG_MOVE = 4,     // arg: компонент 0-3 актуатор, 4-7 клапан, бит 3 - открытие, бит 4 - снят по лимиту
                      // data: [0-15] время под током, мс  [16-23] реле питания DO25..DO32
    LOG_BALL = 5,     // arg: кран 0-3, бит 3 - открытие
    LOG_AIR = 6,      // arg: 1 пуск, 0 стоп; data: [0-15] частота x100, [16-23] код ответа Modbus
    LOG_MAG = 7,      // arg: магистраль, data: id состояния алгоритма (если LOG_MAG_STATES 1)
    LOG_MB_OK = 8,    // связь с МУ восстановлена, data: число ошибок подряд
    LOG_LOST = 9,     // очередь журнала переполнилась, data: сколько записей потеряно
    LOG_CFG = 10,     // изменены настройки, arg: порция x0.1 с, data: соло | триплет<<8 | продувка<<16 | разгон<<24
    LOG_CFG_REJECT = 11, // настройки не приняты (FLOW в работе)
    // ---- ошибки (пишутся и в журнал ошибок) ----
    LOG_ERR_FIRST = 20,
    LOG_ERR_MB = 20,      // ошибка записи в МУ, arg: код ModbusMaster, data: маска
    LOG_ERR_UNSAFE = 21,  // заблокирована опасная маска, data: маска
    LOG_ERR_LIMIT = 22,   // мотор под током дольше T_MY110_MAX_MOTOR_ON, data: маска выходов
    LOG_ERR_STUCK = 23,   // компонент слишком долго в движении, arg: компонент, data: статус
    LOG_ERR_AIR = 24,     // частотник не ответил, arg: код ModbusMaster, data: регистр
    LOG_ERR_INIT = 25     // не удалось выключить выходы МУ при запуске
};

// Какие команды пишем в LOG_CMD (совпадает с порядком state_Flow)
// 0 stop, 1 solo, 2 complex, 3 produvka, 4 clearing, 5 triplet

// Пишем ли каждый переход состояния магистралей (подробно, но журнал заполняется быстрее)
#ifndef LOG_MAG_STATES
#define LOG_MAG_STATES 0
#endif

#define LOG_RECORD_SIZE 12
#define LOG_QUEUE 16
#define LOG_SEQ_EMPTY 0xFFFF

// Области EEPROM: [начало, число слотов]
#define LOG_EV_BASE 0
#define LOG_EV_SLOTS 256                      // 3072 байта
#define LOG_ER_BASE (LOG_EV_BASE + LOG_EV_SLOTS * LOG_RECORD_SIZE)
#define LOG_ER_SLOTS 80                       // 960 байт
#define LOG_SETTINGS_BASE (LOG_ER_BASE + LOG_ER_SLOTS * LOG_RECORD_SIZE) // 4032: настройки (settings.hpp)

// Для экрана: была ошибка с момента последнего сброса индикатора
bool log_error_flag = false;
// Разрешено ли сейчас выводить журнал (вывод занимает время, машина должна стоять)
bool log_dump_allowed = true;
// Источник следующей команды для LOG_CMD
uint8_t log_cmd_source = 0;

#if !mode_work
#include <EEPROM.h>
#include <avr/eeprom.h>
#include <avr/wdt.h>

struct LogRing {
    uint16_t base;
    uint16_t slots;
    uint16_t head;
    uint16_t seq;
};
LogRing log_rings[2] = {{LOG_EV_BASE, LOG_EV_SLOTS, 0, 0}, {LOG_ER_BASE, LOG_ER_SLOTS, 0, 0}};

uint8_t log_queue[LOG_QUEUE][LOG_RECORD_SIZE];
uint8_t log_queue_ring[LOG_QUEUE];
uint8_t log_q_first = 0, log_q_count = 0, log_q_byte = 0;
uint16_t log_lost = 0;

inline uint16_t log_next_seq(uint16_t s) { return s >= 0xFFFE ? 0 : s + 1; }

inline uint16_t log_read16(uint16_t addr) {
    return EEPROM.read(addr) | ((uint16_t)EEPROM.read(addr + 1) << 8);
}
inline uint32_t log_read32(uint16_t addr) {
    return (uint32_t)log_read16(addr) | ((uint32_t)log_read16(addr + 2) << 16);
}
inline uint16_t log_slot_addr(const LogRing &r, uint16_t slot) { return r.base + slot * LOG_RECORD_SIZE; }

// Последняя запись - та, за которой нет её продолжения по номеру
void log_find_head(LogRing &r) {
    for (uint16_t i = 0; i < r.slots; i++) {
        uint16_t s = log_read16(log_slot_addr(r, i));
        if (s == LOG_SEQ_EMPTY) continue;
        if (log_read16(log_slot_addr(r, (i + 1) % r.slots)) != log_next_seq(s)) {
            r.head = (i + 1) % r.slots;
            r.seq = log_next_seq(s);
            return;
        }
    }
    r.head = 0;
    r.seq = 0;
}

inline void log_put16(uint8_t *p, uint16_t v) { p[0] = v & 0xFF; p[1] = v >> 8; }
inline void log_put32(uint8_t *p, uint32_t v) { log_put16(p, v & 0xFFFF); log_put16(p + 2, v >> 16); }

bool log_push(uint8_t ring, uint8_t type, uint8_t arg, uint32_t data) {
    if (log_q_count >= LOG_QUEUE) { log_lost++; return false; }
    uint8_t idx = (log_q_first + log_q_count) % LOG_QUEUE;
    uint8_t *r = log_queue[idx];
    LogRing &lr = log_rings[ring];
    log_put16(r, lr.seq);
    r[2] = type;
    r[3] = arg;
    log_put32(r + 4, millis());
    log_put32(r + 8, data);
    lr.seq = log_next_seq(lr.seq);
    log_queue_ring[idx] = ring;
    log_q_count++;
    return true;
}

// ----- Что пишем -----

void log_event(uint8_t type, uint8_t arg = 0, uint32_t data = 0) {
    if (log_lost && log_q_count < LOG_QUEUE - 1) {
        if (log_push(0, LOG_LOST, 0, log_lost)) log_lost = 0;
    }
    log_push(0, type, arg, data);
    if (type >= LOG_ERR_FIRST) {
        log_push(1, type, arg, data);
        log_error_flag = true;
    }
}

void log_begin() {
    uint8_t reset_flags = MCUSR;     // причина сброса (загрузчик Mega может её обнулять)
    MCUSR = 0;
    log_find_head(log_rings[0]);
    log_find_head(log_rings[1]);
    uint32_t ver = ((uint32_t)FLOW_FW_MAJOR << 8) | FLOW_FW_MINOR;
    log_push(0, LOG_BOOT, reset_flags, ver);
    log_push(1, LOG_BOOT, reset_flags, ver);
    if (reset_flags & (_BV(WDRF) | _BV(BORF))) log_error_flag = true;
}

// Запись: по одному байту EEPROM за вызов, номер записи - последним
void log_write_step() {
    if (log_q_count == 0 || !eeprom_is_ready()) return;
    static const uint8_t order[LOG_RECORD_SIZE] = {2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 0, 1};
    uint8_t *r = log_queue[log_q_first];
    LogRing &lr = log_rings[log_queue_ring[log_q_first]];
    uint8_t b = order[log_q_byte];
    EEPROM.update(log_slot_addr(lr, lr.head) + b, r[b]);
    if (++log_q_byte >= LOG_RECORD_SIZE) {
        log_q_byte = 0;
        log_q_first = (log_q_first + 1) % LOG_QUEUE;
        log_q_count--;
        lr.head = (lr.head + 1) % lr.slots;
    }
}

// ----- Вывод журнала -----

void log_print_time(uint32_t ms) {
    uint32_t s = ms / 1000;
    Serial.print(s / 3600);
    Serial.print(':');
    if ((s / 60) % 60 < 10) Serial.print('0');
    Serial.print((s / 60) % 60);
    Serial.print(':');
    if (s % 60 < 10) Serial.print('0');
    Serial.print(s % 60);
    Serial.print('.');
    uint16_t m = ms % 1000;
    if (m < 100) Serial.print('0');
    if (m < 10) Serial.print('0');
    Serial.print(m);
}

void log_print_reset_cause(uint8_t f) {
    if (f == 0) { Serial.print(F("неизвестна")); return; }
    if (f & _BV(PORF)) Serial.print(F("включение питания "));
    if (f & _BV(EXTRF)) Serial.print(F("кнопка RESET/USB "));
    if (f & _BV(BORF)) Serial.print(F("ПРОСАДКА ПИТАНИЯ "));
    if (f & _BV(WDRF)) Serial.print(F("СТОРОЖЕВОЙ ТАЙМЕР (зависание) "));
    if (f & _BV(JTRF)) Serial.print(F("JTAG "));
}

void log_print_cmd(uint8_t c) {
    switch (c) {
        case 0: Serial.print(F("СТОП")); break;
        case 1: Serial.print(F("соло")); break;
        case 2: Serial.print(F("комплекс")); break;
        case 3: Serial.print(F("продувка")); break;
        case 4: Serial.print(F("чистка")); break;
        case 5: Serial.print(F("триплет")); break;
        default: Serial.print(c); break;
    }
}

void log_print_comp(uint8_t id) {
    if (id < 4) { Serial.print(F("актуатор ")); Serial.print(id + 1); }
    else if (id < 8) { Serial.print(F("клапан ")); Serial.print(id - 3); }
    else { Serial.print(F("компонент ")); Serial.print(id); }
}

void log_print_mask(uint32_t m) {
    Serial.print(F("DO:"));
    bool any = false;
    for (uint8_t i = 0; i < 32; ++i) {
        if (m & (1UL << i)) { Serial.print(any ? ',' : ' '); Serial.print(i + 1); any = true; }
    }
    if (!any) Serial.print(F(" все выкл"));
}

void log_print_record(uint16_t a) {
    uint8_t type = EEPROM.read(a + 2);
    uint8_t arg = EEPROM.read(a + 3);
    uint32_t t = log_read32(a + 4);
    uint32_t data = log_read32(a + 8);
    if (type == LOG_BOOT) {
        Serial.print(F("----- ЗАПУСК v"));
        Serial.print((data >> 8) & 0xFF);
        Serial.print('.');
        Serial.print(data & 0xFF);
        Serial.print(F(", причина: "));
        log_print_reset_cause(arg);
        Serial.println();
        return;
    }
    log_print_time(t);
    Serial.print(F("  "));
    switch (type) {
        case LOG_CMD:
            Serial.print(F("КОМАНДА "));
            log_print_cmd(arg);
            Serial.print((data & 0xFF) == 0 ? F(" (экран)") : (data & 0xFF) == 1 ? F(" (USB)") : F(" (авто)"));
            if (data & 0x100) Serial.print(F(" - ОТКЛОНЕНА: не в стартовом состоянии"));
            break;
        case LOG_MOVE:
            Serial.print(F("ход: "));
            log_print_comp(arg & 7);
            Serial.print((arg & 8) ? F(" открытие ") : F(" закрытие "));
            Serial.print(data & 0xFFFF);
            Serial.print(F(" мс"));
            if (arg & 16) Serial.print(F(" - СНЯТ ПО ЛИМИТУ"));
            break;
        case LOG_BALL:
            Serial.print(F("шаровой кран "));
            Serial.print((arg & 7) + 1);
            Serial.print((arg & 8) ? F(" открыть") : F(" закрыть"));
            break;
        case LOG_AIR:
            Serial.print(arg ? F("воздуходувка ПУСК ") : F("воздуходувка стоп"));
            if (arg) { Serial.print((data & 0xFFFF) / 100.0, 2); Serial.print(F(" Гц")); }
            if ((data >> 16) & 0xFF) { Serial.print(F(" (нет ответа, код ")); Serial.print((data >> 16) & 0xFF); Serial.print(')'); }
            if (data >> 24) { Serial.print(F(" - ответил с попытки ")); Serial.print((data >> 24) + 1); }
            break;
        case LOG_MAG:
            Serial.print(F("магистраль "));
            Serial.print(arg + 1);
            Serial.print(F(" -> состояние "));
            Serial.print(data);
            break;
        case LOG_CFG:
            Serial.print(F("НАСТРОЙКИ: соло ")); Serial.print(data & 0xFF);
            Serial.print(F(" Гц, триплет ")); Serial.print((data >> 8) & 0xFF);
            Serial.print(F(" Гц, продувка ")); Serial.print((data >> 16) & 0xFF);
            Serial.print(F(" Гц, разгон ")); Serial.print((data >> 24) & 0xFF);
            Serial.print(F(" с, порция ")); Serial.print(arg / 10); Serial.print('.'); Serial.print(arg % 10); Serial.print(F(" с"));
            break;
        case LOG_CFG_REJECT:
            Serial.print(F("настройки НЕ приняты: FLOW в работе"));
            break;
        case LOG_MB_OK:
            Serial.print(F("связь с МУ восстановлена после ошибок: "));
            Serial.print(data);
            break;
        case LOG_LOST:
            Serial.print(F("журнал не успевал, пропущено записей: "));
            Serial.print(data);
            break;
        case LOG_ERR_MB:
            Serial.print(F("ОШИБКА связи с МУ, код "));
            Serial.print(arg);
            Serial.print(F(", маска "));
            log_print_mask(data);
            break;
        case LOG_ERR_UNSAFE:
            Serial.print(F("ОШИБКА: заблокирована опасная маска "));
            log_print_mask(data);
            break;
        case LOG_ERR_LIMIT:
            Serial.print(F("ОШИБКА: мотор под током слишком долго, питание снято. Было "));
            log_print_mask(data);
            break;
        case LOG_ERR_STUCK:
            Serial.print(F("ОШИБКА: "));
            log_print_comp(arg);
            Serial.print(F(" слишком долго в движении, статус "));
            Serial.print(data);
            break;
        case LOG_ERR_AIR:
            Serial.print(F("ОШИБКА: частотник не ответил, код "));
            Serial.print(arg);
            break;
        case LOG_ERR_INIT:
            Serial.print(F("ОШИБКА: при запуске не удалось выключить выходы МУ"));
            break;
        default:
            Serial.print(F("? тип "));
            Serial.print(type);
            break;
    }
    Serial.println();
}

void log_dump(uint8_t ring) {
    if (!log_dump_allowed) {
        Serial.println(F("Сначала остановите FLOW: пока выводится журнал, программа занята"));
        return;
    }
    while (log_q_count) { log_write_step(); }   // дописать очередь (EEPROM ждёт сама)
    LogRing &lr = log_rings[ring];

    // журнал начинается после последней отметки "clear"
    uint16_t start = 0, count = 0;
    for (uint16_t k = 0; k < lr.slots; k++) {
        uint16_t a = log_slot_addr(lr, (lr.head + k) % lr.slots);
        if (log_read16(a) == LOG_SEQ_EMPTY) continue;
        if (EEPROM.read(a + 2) == LOG_MARK) { start = k + 1; count = 0; }
        else count++;
    }

    Serial.println();
    Serial.print(ring == 0 ? F("=== ЖУРНАЛ СОБЫТИЙ FLOW") : F("=== ЖУРНАЛ ОШИБОК FLOW"));
    Serial.print(F(", прошивка " FLOW_FW_VERSION " === записей: "));
    Serial.println(count);
    Serial.println(F("время с запуска (ч:мм:сс.мс)  событие"));
    for (uint16_t k = start; k < lr.slots; k++) {
        uint16_t a = log_slot_addr(lr, (lr.head + k) % lr.slots);
        if (log_read16(a) == LOG_SEQ_EMPTY) continue;
        log_print_record(a);
        wdt_reset();
    }
    Serial.println(F("=== КОНЕЦ ЖУРНАЛА ==="));
}

void log_clear() {
    log_push(0, LOG_MARK, 0, 0);
    log_push(1, LOG_MARK, 0, 0);
    log_error_flag = false;
    Serial.println(F("Журналы начаты заново"));
}

// Команды из монитора порта. Возвращает false, если команда не журнальная.
bool log_command(const char *cmd) {
    if (strcmp(cmd, "log") == 0) { log_dump(0); return true; }
    if (strcmp(cmd, "err") == 0) { log_dump(1); return true; }
    if (strcmp(cmd, "clear") == 0) { log_clear(); return true; }
    if (strcmp(cmd, "help") == 0) {
        Serial.println(F("Команды: log - журнал событий, err - журнал ошибок, clear - начать журналы заново"));
        Serial.println(F("Отладка: 1 комплекс, 2 соло, 3 чистка, 4 триплет, 5 продувка, 0 стоп"));
        Serial.println(F("mbr A201 1 - прочитать регистр частотника (по одному)"));
        Serial.println(F("cfg - настройки; cfg solo 45 | trip 60 | air 50 | ramp 5 | portion 2.5 | act 2.0 | air1 60 | default"));
        return true;
    }
    return false;
}

#else  // g++ (mode_work): журнал не пишется
inline void log_event(uint8_t, uint8_t = 0, uint32_t = 0) {}
inline void log_begin() {}
inline void log_write_step() {}
inline bool log_command(const char *) { return false; }
#endif

#endif // LOGGER_HPP
