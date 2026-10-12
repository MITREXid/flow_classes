#ifndef SETTINGS_HPP
#define SETTINGS_HPP
// =====================================================================
//  Настройки FLOW по программам (экран "Настройки" -> программа, или USB "cfg")
//  Хранятся в EEPROM (последние 64 байта, после журнала), переживают выключение.
//  Все параметры - байты; таблица SET_PARAMS описывает каждый: ключ для USB, пределы, по умолчанию,
//  знаков после запятой (1 - хранится в десятых: 26 = 2.6 с). По ней же работает экран.
//
//  Порция - от начала открытия актуатора до команды "закрыть" (меньше хода - актуатор приоткрывается).
//  Воздух Мx - сколько дует воздуходувка после загрузки магистрали x, с.
//  Вулкан - только настройки (режим пока не включён): высота Мx - сколько приоткрывается шаровой кран.
// =====================================================================
#include "declorations.hpp"

struct FlowSettings {
    // комплекс
    uint8_t cx_hz_t;     // частота триплет-части, Гц
    uint8_t cx_hz_s;     // частота соло-части, Гц
    uint8_t cx_portion;  // x0.1 с
    uint8_t cx_air[4];   // с
    // соло (магистраль 4)
    uint8_t so_hz;
    uint8_t so_portion;
    uint8_t so_air;
    // триплет (магистрали 1-3)
    uint8_t tr_hz;
    uint8_t tr_portion;
    uint8_t tr_air[3];
    // вулкан
    uint8_t vu_portion;
    uint8_t vu_height[4];// x0.1 с - приоткрытие шарового крана
    uint8_t vu_pulse_hz;
    uint8_t vu_pulse_ds; // x0.1 с
    uint8_t vu_pause_ds; // x0.1 с
    uint8_t vu_rounds;
    uint8_t vu_send_hz;
    uint8_t vu_send_s;
    // чистка
    uint8_t cl_hz;
    // продувка
    uint8_t pv_hz;
    uint8_t pv_s;
    // общие
    uint8_t ramp_s;      // разгон воздуходувки от 10 Гц, с
    uint8_t stop_s;      // плавная остановка до 10 Гц, с
    uint8_t act_ds;      // ход актуатора (питание на открытие/закрытие), x0.1 с
};

constexpr uint8_t SET_HZ_MAX = 70;        // в частотнике макс. частота должна быть >= 70 Гц
constexpr uint8_t SET_SIZE = sizeof(FlowSettings);

struct SetParam {
    char key[8];   // USB: cfg <key> <значение>
    uint8_t off;   // смещение в FlowSettings
    uint8_t lo, hi, def;
    uint8_t dec;   // 1 - десятые
};

#define SP(k, f, lo, hi, def, dec) {k, (uint8_t)offsetof(FlowSettings, f), lo, hi, def, dec}
#define SPA(k, f, i, lo, hi, def, dec) {k, (uint8_t)(offsetof(FlowSettings, f) + i), lo, hi, def, dec}
// Порядок = порядок строк на страницах экрана (SET_PAGES)
const SetParam SET_PARAMS[] = {
    // 0-6 комплекс
    SP("cx.hzt", cx_hz_t, 10, 70, 57, 0), SP("cx.hzs", cx_hz_s, 10, 70, 40, 0), SP("cx.por", cx_portion, 10, 200, 20, 1),
    SPA("cx.air1", cx_air, 0, 5, 180, 33, 0), SPA("cx.air2", cx_air, 1, 5, 180, 33, 0),
    SPA("cx.air3", cx_air, 2, 5, 180, 33, 0), SPA("cx.air4", cx_air, 3, 5, 180, 33, 0),
    // 7-9 соло
    SP("so.hz", so_hz, 10, 70, 40, 0), SP("so.por", so_portion, 10, 200, 20, 1), SP("so.air", so_air, 5, 180, 33, 0),
    // 10-14 триплет
    SP("tr.hz", tr_hz, 10, 70, 57, 0), SP("tr.por", tr_portion, 10, 200, 20, 1),
    SPA("tr.air1", tr_air, 0, 5, 180, 33, 0), SPA("tr.air2", tr_air, 1, 5, 180, 33, 0), SPA("tr.air3", tr_air, 2, 5, 180, 33, 0),
    // 15-25 вулкан
    SP("vu.por", vu_portion, 10, 200, 20, 1),
    SPA("vu.h1", vu_height, 0, 1, 100, 15, 1), SPA("vu.h2", vu_height, 1, 1, 100, 15, 1),
    SPA("vu.h3", vu_height, 2, 1, 100, 15, 1), SPA("vu.h4", vu_height, 3, 1, 100, 15, 1),
    SP("vu.phz", vu_pulse_hz, 10, 70, 30, 0), SP("vu.pul", vu_pulse_ds, 5, 100, 20, 1), SP("vu.pau", vu_pause_ds, 5, 100, 20, 1),
    SP("vu.rnd", vu_rounds, 1, 20, 5, 0), SP("vu.shz", vu_send_hz, 10, 70, 57, 0), SP("vu.snd", vu_send_s, 5, 180, 33, 0),
    // 26 чистка
    SP("cl.hz", cl_hz, 10, 70, 50, 0),
    // 27-28 продувка
    SP("pv.hz", pv_hz, 10, 70, 50, 0), SP("pv.s", pv_s, 5, 180, 15, 0),
    // 29-31 общие
    SP("ramp", ramp_s, 0, 20, 0, 0), SP("stop", stop_s, 0, 20, 3, 0), SP("act", act_ds, 5, 50, 30, 1),
};
constexpr uint8_t SET_N = sizeof(SET_PARAMS) / sizeof(SET_PARAMS[0]);

// Страницы экрана: картинка фона, первая строка (индекс в SET_PARAMS), сколько строк (до 5)
struct SetPage { uint8_t pic; uint8_t first; uint8_t count; };
const SetPage SET_PAGES[] = {
    {19, 0, 5}, {20, 5, 2},             // 0-1 комплекс
    {21, 7, 3},                         // 2 соло
    {22, 10, 5},                        // 3 триплет
    {23, 15, 5}, {24, 20, 5}, {25, 25, 1}, // 4-6 вулкан
    {26, 26, 1},                        // 7 чистка
    {27, 27, 2},                        // 8 продувка
    {28, 29, 3},                        // 9 общие
};
// Программы меню (плитки экрана): первая страница и число страниц
struct SetProg { uint8_t page; uint8_t pages; };
const SetProg SET_PROGS[] = {{0, 2}, {2, 1}, {3, 1}, {4, 3}, {7, 1}, {8, 1}, {9, 1}};
constexpr uint8_t SET_PROG_N = sizeof(SET_PROGS) / sizeof(SET_PROGS[0]);

FlowSettings g_set;

inline uint8_t &set_ref(FlowSettings &s, uint8_t i) { return reinterpret_cast<uint8_t *>(&s)[SET_PARAMS[i].off]; }

void settings_default(FlowSettings &s) {
    for (uint8_t i = 0; i < SET_N; ++i) set_ref(s, i) = SET_PARAMS[i].def;
}

inline uint8_t clamp_u8(uint8_t v, uint8_t lo, uint8_t hi) { return v < lo ? lo : (v > hi ? hi : v); }

void settings_clamp(FlowSettings &s) {
    for (uint8_t i = 0; i < SET_N; ++i) set_ref(s, i) = clamp_u8(set_ref(s, i), SET_PARAMS[i].lo, SET_PARAMS[i].hi);
}

uint32_t settings_pack(const FlowSettings &s) {
    return (uint32_t)s.cx_hz_t | ((uint32_t)s.so_hz << 8) | ((uint32_t)s.tr_hz << 16) | ((uint32_t)s.pv_hz << 24);
}

// Адрес в EEPROM: сразу после журнала ошибок (см. logger.hpp, LOG_SETTINGS_BASE)
#define SETTINGS_MAGIC 0xF5
#define SETTINGS_VERSION 5  // 5 - по программам; 1-4 - общие настройки (переносятся во все программы)

#if !mode_work
#include <EEPROM.h>
// Раскладка v5: [0] magic [1] version [2..2+SET_SIZE) настройки [2+SET_SIZE] контрольная сумма
uint8_t settings_crc(const uint8_t *p, uint8_t n) {
    uint8_t c = 0x5A;
    for (uint8_t i = 0; i < n; ++i) { c = (c << 1 | c >> 7) ^ p[i]; }
    return c;
}

// v1-v4 (общие настройки): hz_solo hz_triplet hz_air ramp portion act air[4] stop - в программы
bool settings_load_old(const uint8_t *buf) {
    const uint8_t v = buf[1];
    const bool ok = (v == 4 && settings_crc(buf, 13) == buf[13]) || (v == 3 && settings_crc(buf, 12) == buf[12]) ||
                    (v == 2 && settings_crc(buf, 8) == buf[8]) || (v == 1 && settings_crc(buf, 7) == buf[7]);
    if (!ok) return false;
    FlowSettings &s = g_set;
    s.cx_hz_s = s.so_hz = buf[2];
    s.cx_hz_t = s.tr_hz = buf[3];
    s.cl_hz = s.pv_hz = buf[4];
    s.ramp_s = buf[5];
    s.cx_portion = s.so_portion = s.tr_portion = buf[6];
    if (v >= 2) s.act_ds = buf[7];
    if (v >= 3) {
        for (uint8_t i = 0; i < 4; ++i) s.cx_air[i] = buf[8 + i];
        for (uint8_t i = 0; i < 3; ++i) s.tr_air[i] = buf[8 + i];
        s.so_air = buf[11];
    }
    if (v >= 4) s.stop_s = buf[12];
    return true;
}

bool settings_load(uint16_t addr) {
    uint8_t buf[SET_SIZE + 3];
    for (uint8_t i = 0; i < SET_SIZE + 3; ++i) buf[i] = EEPROM.read(addr + i);
    settings_default(g_set);
    if (buf[0] != SETTINGS_MAGIC) return false;
    if (buf[1] == SETTINGS_VERSION && settings_crc(buf, SET_SIZE + 2) == buf[SET_SIZE + 2]) {
        memcpy(&g_set, buf + 2, SET_SIZE);
    } else if (!settings_load_old(buf)) {
        return false;
    }
    settings_clamp(g_set);
    return true;
}

void settings_save(uint16_t addr) {
    uint8_t buf[SET_SIZE + 3];
    buf[0] = SETTINGS_MAGIC;
    buf[1] = SETTINGS_VERSION;
    memcpy(buf + 2, &g_set, SET_SIZE);
    buf[SET_SIZE + 2] = settings_crc(buf, SET_SIZE + 2);
    for (uint8_t i = 0; i < SET_SIZE + 3; ++i) EEPROM.update(addr + i, buf[i]);
}

// значение параметра в строку: "57", "2.6"
void settings_fmt(char *out, uint8_t i, uint8_t v) {
    if (SET_PARAMS[i].dec) sprintf(out, "%u.%u", v / 10, v % 10);
    else sprintf(out, "%u", v);
}

void settings_print() {
    static const char *const PROG_NAMES[] = {"комплекс", "соло", "триплет", "вулкан (режим пока выключен)", "чистка", "продувка", "общие"};
    Serial.println(F("=== НАСТРОЙКИ FLOW (cfg <ключ> <значение>) ==="));
    char v[8];
    for (uint8_t p = 0; p < SET_PROG_N; ++p) {
        Serial.print(F("-- ")); Serial.println(PROG_NAMES[p]);
        for (uint8_t pg = SET_PROGS[p].page; pg < SET_PROGS[p].page + SET_PROGS[p].pages; ++pg) {
            for (uint8_t i = SET_PAGES[pg].first; i < SET_PAGES[pg].first + SET_PAGES[pg].count; ++i) {
                settings_fmt(v, i, set_ref(g_set, i));
                Serial.print(F("   ")); Serial.print(SET_PARAMS[i].key); Serial.print(F(" = ")); Serial.print(v);
                char lo[8], hi[8];
                settings_fmt(lo, i, SET_PARAMS[i].lo); settings_fmt(hi, i, SET_PARAMS[i].hi);
                Serial.print(F("   (")); Serial.print(lo); Serial.print('-'); Serial.print(hi); Serial.println(')');
            }
        }
    }
}
#else
inline bool settings_load(uint16_t) { settings_default(g_set); return false; }
inline void settings_save(uint16_t) {}
inline void settings_print() {}
#endif

#endif // SETTINGS_HPP
