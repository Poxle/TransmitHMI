#include "logic.h"
#include <string.h>
#include <stdio.h>
#include <time.h>

static const char* categories[3] = {"显示 (Display)", "声音 (Sound)", "检查 (Inspection)"};
static const char* display_funcs[3] = {"启动发动机", "油门控制", "刹车控制"};
static const char* sound_funcs[3] = {"开启车灯", "解锁车门", "调节座椅"};
static const char* inspection_funcs[3] = {"激活ABS", "气囊检测", "胎压监测"};

static int repeat_interval_for(const char* func) {
    if(strcmp(func, "调节座椅") == 0) return 2000;
    if(strcmp(func, "气囊检测") == 0) return 3000;
    if(strcmp(func, "油门控制") == 0) return 1500;
    return 0;
}

static void make_timestamp(char out[32]) {
    time_t t = time(NULL);
    struct tm tmv;
#ifdef _WIN32
    localtime_s(&tmv, &t);
#else
    tmv = *localtime(&t);
#endif
    snprintf(out, 32, "%02d:%02d:%02d", tmv.tm_hour, tmv.tm_min, tmv.tm_sec);
}

void logic_init(SignalPanelState* s) {
    memset(s, 0, sizeof(*s));
    strcpy(s->selectedScene, "B");
    strcpy(s->selectedCategory, categories[0]);
    strcpy(s->selectedFunction, display_funcs[0]);
    s->manualInterval = 1000;
    s->viewMode = VIEW_AUTO;
}

void logic_add_log(SignalPanelState* s, LogType type, const char* msg) {
    if(s->log_count >= 256) s->log_count = 0;
    LogMessage* m = &s->logs[s->log_count++];
    m->id = s->next_log_id++;
    m->type = type;
    make_timestamp(m->timestamp);
    snprintf(m->message, sizeof(m->message), "%s", msg);
}

int logic_get_repeat_interval_for_function(const char* func) {
    return repeat_interval_for(func);
}

void logic_set_scene(SignalPanelState* s, const char* scene) {
    snprintf(s->selectedScene, sizeof(s->selectedScene), "%s", scene);
    if(s->isConnected) logic_add_log(s, LOG_TX, s->selectedScene);
}

void logic_set_category(SignalPanelState* s, const char* category) {
    snprintf(s->selectedCategory, sizeof(s->selectedCategory), "%s", category);
    const char* func = display_funcs[0];
    if(strcmp(category, categories[1]) == 0) func = sound_funcs[0];
    else if(strcmp(category, categories[2]) == 0) func = inspection_funcs[0];
    snprintf(s->selectedFunction, sizeof(s->selectedFunction), "%s", func);
}

void logic_set_function(SignalPanelState* s, const char* func) {
    snprintf(s->selectedFunction, sizeof(s->selectedFunction), "%s", func);
}

void logic_toggle_connected(SignalPanelState* s) {
    s->isConnected = !s->isConnected;
}

void logic_clear_logs(SignalPanelState* s) {
    s->log_count = 0;
}

void logic_start_repeating_auto(SignalPanelState* s) {
    s->isTransmitting = 1;
    s->isRepeating = 1;
    char buf[160];
    snprintf(buf, sizeof(buf), "%s - %s", s->selectedCategory, s->selectedFunction);
    logic_add_log(s, LOG_TX, buf);
}

void logic_repeat_auto_tick(SignalPanelState* s) {
    char buf[160];
    snprintf(buf, sizeof(buf), "%s - %s", s->selectedCategory, s->selectedFunction);
    logic_add_log(s, LOG_TX, buf);
}

void logic_send_auto_once(SignalPanelState* s) {
    s->isTransmitting = 1;
    char buf[160];
    snprintf(buf, sizeof(buf), "%s - %s", s->selectedCategory, s->selectedFunction);
    logic_add_log(s, LOG_TX, buf);
}

void logic_send_auto_rx(SignalPanelState* s) {
    logic_add_log(s, LOG_RX, "CAN ID: 0x123 | Data: [0x01, 0x02, 0x03]");
    s->isTransmitting = 0;
}

void logic_start_repeating_manual(SignalPanelState* s) {
    s->isTransmitting = 1;
    s->isRepeating = 1;
    char buf[200];
    snprintf(buf, sizeof(buf), "CAN ID: %s | Data: %s", s->manualId, s->manualData);
    logic_add_log(s, LOG_TX, buf);
}

void logic_repeat_manual_tick(SignalPanelState* s) {
    char buf[200];
    snprintf(buf, sizeof(buf), "CAN ID: %s | Data: %s", s->manualId, s->manualData);
    logic_add_log(s, LOG_TX, buf);
}

void logic_send_manual_once(SignalPanelState* s) {
    s->isTransmitting = 1;
    char buf[200];
    snprintf(buf, sizeof(buf), "CAN ID: %s | Data: %s", s->manualId, s->manualData);
    logic_add_log(s, LOG_TX, buf);
}

void logic_send_manual_rx(SignalPanelState* s) {
    logic_add_log(s, LOG_RX, "ACK: OK");
    s->isTransmitting = 0;
}

void logic_stop(SignalPanelState* s) {
    s->isTransmitting = 0;
    s->isRepeating = 0;
    logic_add_log(s, LOG_TX, "停止发送");
}

