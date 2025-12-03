#ifndef UI_SIGNAL_PANEL_H
#define UI_SIGNAL_PANEL_H

#include "logic.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    SignalPanelState state;
    void* timer_repeat;
    void* timer_rx_delay;
    void* led;
    void* log_list;
    void* connect_switch;
    void* auto_container;
    void* manual_container;
    void* category_dd;
    void* function_dd;
    void* manual_id_ta;
    void* manual_data_ta;
    void* manual_repeat_sw;
    void* manual_interval_ta;
    void* transmit_btn;
    void* stop_btn;
} SignalPanelUI;

void signal_panel_create(void* parent, SignalPanelUI* ui);

#ifdef __cplusplus
}
#endif

#endif

