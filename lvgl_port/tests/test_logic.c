#include "logic.h"
#include <stdio.h>
#include <string.h>

int main() {
    SignalPanelState s;
    logic_init(&s);
    if(strcmp(s.selectedScene, "B") != 0) return 1;
    logic_toggle_connected(&s);
    logic_set_scene(&s, "ACC");
    if(s.log_count == 0) return 2;
    logic_set_category(&s, "声音 (Sound)");
    logic_set_function(&s, "调节座椅");
    int ri = logic_get_repeat_interval_for_function(s.selectedFunction);
    if(ri != 2000) return 3;
    logic_start_repeating_auto(&s);
    logic_repeat_auto_tick(&s);
    logic_send_auto_rx(&s);
    logic_stop(&s);
    snprintf(s.manualId, sizeof(s.manualId), "0x123");
    snprintf(s.manualData, sizeof(s.manualData), "[0x01,0x02,0x03]");
    s.manualRepeat = 0;
    logic_send_manual_once(&s);
    logic_send_manual_rx(&s);
    logic_clear_logs(&s);
    if(s.log_count != 0) return 4;
    printf("OK\n");
    return 0;
}

