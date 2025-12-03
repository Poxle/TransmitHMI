#ifndef LOGIC_H
#define LOGIC_H

#include <stdint.h>

typedef enum { VIEW_AUTO = 0, VIEW_MANUAL = 1 } ViewMode;
typedef enum { LOG_TX = 0, LOG_RX = 1 } LogType;

typedef struct {
    int id;
    LogType type;
    char timestamp[32];
    char message[160];
} LogMessage;

typedef struct {
    char selectedScene[8];
    char selectedCategory[32];
    char selectedFunction[32];
    int isTransmitting;
    int isRepeating;
    int isConnected;
    ViewMode viewMode;
    char manualId[32];
    char manualData[160];
    int manualRepeat;
    int manualInterval;
    LogMessage logs[256];
    int log_count;
    int next_log_id;
} SignalPanelState;

void logic_init(SignalPanelState* s);
void logic_add_log(SignalPanelState* s, LogType type, const char* msg);
int logic_get_repeat_interval_for_function(const char* func);
void logic_set_scene(SignalPanelState* s, const char* scene);
void logic_set_category(SignalPanelState* s, const char* category);
void logic_set_function(SignalPanelState* s, const char* func);
void logic_toggle_connected(SignalPanelState* s);
void logic_clear_logs(SignalPanelState* s);

void logic_start_repeating_auto(SignalPanelState* s);
void logic_repeat_auto_tick(SignalPanelState* s);
void logic_send_auto_once(SignalPanelState* s);
void logic_send_auto_rx(SignalPanelState* s);

void logic_start_repeating_manual(SignalPanelState* s);
void logic_repeat_manual_tick(SignalPanelState* s);
void logic_send_manual_once(SignalPanelState* s);
void logic_send_manual_rx(SignalPanelState* s);

void logic_stop(SignalPanelState* s);

#endif

