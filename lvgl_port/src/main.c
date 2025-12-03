#include <lvgl.h>
#include "ui_signal_panel.h"

int main(void) {
    lv_init();
    SignalPanelUI ui;
    lv_obj_t* screen = lv_screen_active();
    signal_panel_create(screen, &ui);
    while(1) {
        lv_timer_handler();
    }
    return 0;
}

