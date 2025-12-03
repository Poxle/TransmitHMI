#include "ui_signal_panel.h"
#include <lvgl.h>
#include <string.h>

static void update_transmit_button_state(SignalPanelUI* ui) {
    int disabled = ui->state.isTransmitting || ui->state.isRepeating || !ui->state.isConnected;
    if(ui->state.viewMode == VIEW_MANUAL) {
        if(strlen(ui->state.manualId) == 0 || strlen(ui->state.manualData) == 0) disabled = 1;
    }
    lv_obj_t* btn = (lv_obj_t*)ui->transmit_btn;
    if(disabled) lv_obj_add_state(btn, LV_STATE_DISABLED); else lv_obj_remove_state(btn, LV_STATE_DISABLED);
}

static void refresh_logs(SignalPanelUI* ui) {
    lv_obj_t* list = (lv_obj_t*)ui->log_list;
    lv_obj_clean(list);
    if(!ui->state.isConnected) {
        lv_list_add_text(list, "未连接");
        return;
    }
    if(ui->state.log_count == 0) {
        lv_list_add_text(list, "已连接 - 等待发送...");
        return;
    }
    for(int i=0;i<ui->state.log_count;i++) {
        LogMessage* m = &ui->state.logs[i];
        char line[256];
        const char* t = m->type == LOG_TX ? "[TX]" : "[RX]";
        snprintf(line, sizeof(line), "%s %s %s", m->timestamp, t, m->message);
        lv_list_add_text(list, line);
    }
    lv_obj_scroll_to_y(list, lv_obj_get_scroll_y(list)+1000, LV_ANIM_OFF);
}

static void rx_auto_cb(lv_timer_t* t) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_timer_get_user_data(t);
    logic_send_auto_rx(&ui->state);
    refresh_logs(ui);
    lv_timer_delete(t);
}

static void rx_manual_cb(lv_timer_t* t) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_timer_get_user_data(t);
    logic_send_manual_rx(&ui->state);
    refresh_logs(ui);
    lv_timer_delete(t);
}

static void schedule_rx_delay(SignalPanelUI* ui, int ms, int is_manual) {
    if(ui->timer_rx_delay) { lv_timer_delete((lv_timer_t*)ui->timer_rx_delay); ui->timer_rx_delay = NULL; }
    lv_timer_t* t = lv_timer_create(is_manual ? rx_manual_cb : rx_auto_cb, ms, ui);
    ui->timer_rx_delay = t;
}

static void repeat_tick_cb(lv_timer_t* timer) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_timer_get_user_data(timer);
    if(ui->state.viewMode == VIEW_AUTO) {
        logic_repeat_auto_tick(&ui->state);
        refresh_logs(ui);
        schedule_rx_delay(ui, 500, 0);
    } else {
        logic_repeat_manual_tick(&ui->state);
        refresh_logs(ui);
        schedule_rx_delay(ui, 500, 1);
    }
}

static void on_connect_sw(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    logic_toggle_connected(&ui->state);
    refresh_logs(ui);
    update_transmit_button_state(ui);
}

static void on_scene_btn(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    const char* scene = lv_label_get_text(lv_obj_get_child(lv_event_get_target_obj(e), 0));
    logic_set_scene(&ui->state, scene);
    refresh_logs(ui);
}

static void on_category_changed(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    char buf[64];
    lv_dropdown_get_selected_str((lv_obj_t*)ui->category_dd, buf, sizeof(buf));
    logic_set_category(&ui->state, buf);
    lv_dropdown_clear_options((lv_obj_t*)ui->function_dd);
    if(strcmp(buf, "显示 (Display)") == 0) {
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "启动发动机", LV_DROPDOWN_POS_LAST);
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "油门控制", LV_DROPDOWN_POS_LAST);
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "刹车控制", LV_DROPDOWN_POS_LAST);
    } else if(strcmp(buf, "声音 (Sound)") == 0) {
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "开启车灯", LV_DROPDOWN_POS_LAST);
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "解锁车门", LV_DROPDOWN_POS_LAST);
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "调节座椅", LV_DROPDOWN_POS_LAST);
    } else {
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "激活ABS", LV_DROPDOWN_POS_LAST);
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "气囊检测", LV_DROPDOWN_POS_LAST);
        lv_dropdown_add_option((lv_obj_t*)ui->function_dd, "胎压监测", LV_DROPDOWN_POS_LAST);
    }
    lv_dropdown_set_selected((lv_obj_t*)ui->function_dd, 0);
    update_transmit_button_state(ui);
}

static void on_function_changed(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    char buf[64];
    lv_dropdown_get_selected_str((lv_obj_t*)ui->function_dd, buf, sizeof(buf));
    logic_set_function(&ui->state, buf);
    update_transmit_button_state(ui);
}

static void on_manual_id_changed(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    const char* txt = lv_textarea_get_text((lv_obj_t*)ui->manual_id_ta);
    snprintf(ui->state.manualId, sizeof(ui->state.manualId), "%s", txt);
    update_transmit_button_state(ui);
}

static void on_manual_data_changed(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    const char* txt = lv_textarea_get_text((lv_obj_t*)ui->manual_data_ta);
    snprintf(ui->state.manualData, sizeof(ui->state.manualData), "%s", txt);
    update_transmit_button_state(ui);
}

static void on_manual_repeat_sw(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    ui->state.manualRepeat = lv_obj_has_state(lv_event_get_target_obj(e), LV_STATE_CHECKED) ? 1 : 0;
}

static void on_manual_interval_changed(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    const char* txt = lv_textarea_get_text((lv_obj_t*)ui->manual_interval_ta);
    int v = atoi(txt);
    if(v < 100) v = 100;
    ui->state.manualInterval = v;
}

static void goto_auto(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    ui->state.viewMode = VIEW_AUTO;
    lv_obj_remove_flag((lv_obj_t*)ui->auto_container, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag((lv_obj_t*)ui->manual_container, LV_OBJ_FLAG_HIDDEN);
    update_transmit_button_state(ui);
}

static void goto_manual(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    ui->state.viewMode = VIEW_MANUAL;
    lv_obj_add_flag((lv_obj_t*)ui->auto_container, LV_OBJ_FLAG_HIDDEN);
    lv_obj_remove_flag((lv_obj_t*)ui->manual_container, LV_OBJ_FLAG_HIDDEN);
    update_transmit_button_state(ui);
}

static void on_transmit(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    if(ui->state.viewMode == VIEW_AUTO) {
        int interval = logic_get_repeat_interval_for_function(ui->state.selectedFunction);
        if(interval > 0) {
            logic_start_repeating_auto(&ui->state);
            refresh_logs(ui);
            if(ui->timer_repeat) lv_timer_delete((lv_timer_t*)ui->timer_repeat);
            lv_timer_t* t = lv_timer_create(repeat_tick_cb, interval, ui);
            ui->timer_repeat = t;
            schedule_rx_delay(ui, 500, 0);
        } else {
            logic_send_auto_once(&ui->state);
            refresh_logs(ui);
            schedule_rx_delay(ui, 1000, 0);
        }
    } else {
        if(ui->state.manualRepeat) {
            logic_start_repeating_manual(&ui->state);
            refresh_logs(ui);
            if(ui->timer_repeat) lv_timer_delete((lv_timer_t*)ui->timer_repeat);
            lv_timer_t* t = lv_timer_create(repeat_tick_cb, ui->state.manualInterval, ui);
            ui->timer_repeat = t;
            schedule_rx_delay(ui, 500, 1);
        } else {
            logic_send_manual_once(&ui->state);
            refresh_logs(ui);
            schedule_rx_delay(ui, 1000, 1);
        }
    }
    update_transmit_button_state(ui);
}

static void on_stop(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    if(ui->timer_repeat) { lv_timer_delete((lv_timer_t*)ui->timer_repeat); ui->timer_repeat = NULL; }
    logic_stop(&ui->state);
    refresh_logs(ui);
    update_transmit_button_state(ui);
}

static void on_clear(lv_event_t* e) {
    SignalPanelUI* ui = (SignalPanelUI*)lv_event_get_user_data(e);
    logic_clear_logs(&ui->state);
    refresh_logs(ui);
}

void signal_panel_create(void* parent, SignalPanelUI* ui) {
    logic_init(&ui->state);

    lv_obj_t* root = lv_obj_create((lv_obj_t*)parent);
    lv_obj_set_size(root, 172, 640);
    lv_obj_center(root);

    lv_obj_t* header = lv_obj_create(root);
    lv_obj_set_size(header, 172, 32);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_t* icon_label = lv_label_create(header);
    lv_label_set_text(icon_label, "CAN BUS TX");
    lv_obj_align(icon_label, LV_ALIGN_LEFT_MID, 4, 0);
    lv_obj_t* sw = lv_switch_create(header);
    lv_obj_align(sw, LV_ALIGN_RIGHT_MID, -4, 0);
    ui->connect_switch = sw;
    lv_obj_add_event_cb(sw, on_connect_sw, LV_EVENT_CLICKED, ui);

    lv_obj_t* log = lv_list_create(root);
    lv_obj_set_size(log, 164, 155);
    lv_obj_align_to(log, header, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    ui->log_list = log;
    refresh_logs(ui);

    lv_obj_t* clear_btn = lv_button_create(root);
    lv_obj_set_size(clear_btn, 164, 24);
    lv_obj_align_to(clear_btn, log, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);
    lv_obj_t* clear_lbl = lv_label_create(clear_btn);
    lv_label_set_text(clear_lbl, "清空日志");
    lv_obj_center(clear_lbl);
    lv_obj_add_event_cb(clear_btn, on_clear, LV_EVENT_CLICKED, ui);

    lv_obj_t* controls = lv_obj_create(root);
    lv_obj_set_size(controls, 164, 320);
    lv_obj_align_to(controls, clear_btn, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

    lv_obj_t* auto_cont = lv_obj_create(controls);
    lv_obj_set_size(auto_cont, 160, 180);
    ui->auto_container = auto_cont;

    lv_obj_t* grid = lv_obj_create(auto_cont);
    lv_obj_set_size(grid, 160, 80);
    const char* scenes[6] = {"B","BA","IGP","IGR","ST","ACC"};
    for(int i=0;i<6;i++){
        lv_obj_t* b = lv_button_create(grid);
        lv_obj_set_size(b, 74, 24);
        lv_obj_set_pos(b, (i%2)*80, (i/2)*26);
        lv_obj_t* l = lv_label_create(b);
        lv_label_set_text(l, scenes[i]);
        lv_obj_center(l);
        lv_obj_add_event_cb(b, on_scene_btn, LV_EVENT_CLICKED, ui);
    }

    lv_obj_t* cat = lv_dropdown_create(auto_cont);
    lv_dropdown_set_options(cat, "显示 (Display)\n声音 (Sound)\n检查 (Inspection)");
    lv_obj_set_width(cat, 160);
    lv_obj_set_pos(cat, 0, 100);
    lv_obj_add_event_cb(cat, on_category_changed, LV_EVENT_VALUE_CHANGED, ui);
    ui->category_dd = cat;

    lv_obj_t* func = lv_dropdown_create(auto_cont);
    lv_dropdown_set_options(func, "启动发动机\n油门控制\n刹车控制");
    lv_obj_set_width(func, 160);
    lv_obj_set_pos(func, 0, 132);
    lv_obj_add_event_cb(func, on_function_changed, LV_EVENT_VALUE_CHANGED, ui);
    ui->function_dd = func;

    lv_obj_t* manual_cont = lv_obj_create(controls);
    lv_obj_set_size(manual_cont, 160, 180);
    ui->manual_container = manual_cont;
    lv_obj_add_flag(manual_cont, LV_OBJ_FLAG_HIDDEN);

    lv_obj_t* id_ta = lv_textarea_create(manual_cont);
    lv_obj_set_size(id_ta, 160, 24);
    lv_textarea_set_placeholder_text(id_ta, "CAN ID 例如: 0x123");
    lv_obj_add_event_cb(id_ta, on_manual_id_changed, LV_EVENT_VALUE_CHANGED, ui);
    ui->manual_id_ta = id_ta;

    lv_obj_t* data_ta = lv_textarea_create(manual_cont);
    lv_obj_set_size(data_ta, 160, 60);
    lv_textarea_set_placeholder_text(data_ta, "DATA 例如: [0x01,0x02,0x03]");
    lv_obj_set_pos(data_ta, 0, 28);
    lv_obj_add_event_cb(data_ta, on_manual_data_changed, LV_EVENT_VALUE_CHANGED, ui);
    ui->manual_data_ta = data_ta;

    lv_obj_t* repeat_sw = lv_switch_create(manual_cont);
    lv_obj_set_pos(repeat_sw, 0, 92);
    lv_obj_add_event_cb(repeat_sw, on_manual_repeat_sw, LV_EVENT_VALUE_CHANGED, ui);
    ui->manual_repeat_sw = repeat_sw;

    lv_obj_t* interval_ta = lv_textarea_create(manual_cont);
    lv_obj_set_size(interval_ta, 160, 24);
    lv_obj_set_pos(interval_ta, 0, 122);
    lv_textarea_set_text(interval_ta, "1000");
    lv_obj_add_event_cb(interval_ta, on_manual_interval_changed, LV_EVENT_VALUE_CHANGED, ui);
    ui->manual_interval_ta = interval_ta;

    lv_obj_t* back_btn = lv_button_create(manual_cont);
    lv_obj_set_size(back_btn, 160, 24);
    lv_obj_set_pos(back_btn, 0, 152);
    lv_obj_t* back_lbl = lv_label_create(back_btn);
    lv_label_set_text(back_lbl, "返回");
    lv_obj_center(back_lbl);
    lv_obj_add_event_cb(back_btn, goto_auto, LV_EVENT_CLICKED, ui);

    lv_obj_t* manual_btn = lv_button_create(auto_cont);
    lv_obj_set_size(manual_btn, 160, 24);
    lv_obj_set_pos(manual_btn, 0, 160);
    lv_obj_t* manual_lbl = lv_label_create(manual_btn);
    lv_label_set_text(manual_lbl, "手动输入");
    lv_obj_center(manual_lbl);
    lv_obj_add_event_cb(manual_btn, goto_manual, LV_EVENT_CLICKED, ui);

    lv_obj_t* footer = lv_obj_create(root);
    lv_obj_set_size(footer, 164, 100);
    lv_obj_align_to(footer, controls, LV_ALIGN_OUT_BOTTOM_MID, 0, 4);

    lv_obj_t* stop_btn = lv_button_create(footer);
    lv_obj_set_size(stop_btn, 64, 24);
    lv_obj_set_pos(stop_btn, 96, 4);
    lv_obj_t* stop_lbl = lv_label_create(stop_btn);
    lv_label_set_text(stop_lbl, "STOP");
    lv_obj_center(stop_lbl);
    lv_obj_add_event_cb(stop_btn, on_stop, LV_EVENT_CLICKED, ui);
    ui->stop_btn = stop_btn;

    lv_obj_t* transmit_btn = lv_button_create(footer);
    lv_obj_set_size(transmit_btn, 160, 32);
    lv_obj_set_pos(transmit_btn, 0, 36);
    lv_obj_t* tx_lbl = lv_label_create(transmit_btn);
    lv_label_set_text(tx_lbl, "TRANSMIT");
    lv_obj_center(tx_lbl);
    lv_obj_add_event_cb(transmit_btn, on_transmit, LV_EVENT_CLICKED, ui);
    ui->transmit_btn = transmit_btn;

    update_transmit_button_state(ui);
}
