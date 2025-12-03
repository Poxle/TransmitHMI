/**
 * @file ui_manual_input.c
 * @brief Manual Input Mode Component Implementation
 * 
 * Manual CAN ID/Data input with repeat settings
 */

#include "lvgl.h"
#include "ui_config.h"
#include "ui_state.h"
#include "ui_binding.h"

static lv_obj_t* manual_container = NULL;
static lv_obj_t* id_textarea = NULL;
static lv_obj_t* data_textarea = NULL;
static lv_obj_t* repeat_switch = NULL;
static lv_obj_t* interval_textarea = NULL;
static lv_obj_t* interval_container = NULL;

// Forward declarations
extern lv_obj_t* ui_controls_get_container(void);

// Back button callback
static void back_btn_cb(lv_event_t* e) {
    ui_state_set_view_mode(VIEW_MODE_AUTO);
    
    // Hide manual container, show auto controls
    if (manual_container != NULL) {
        lv_obj_add_flag(manual_container, LV_OBJ_FLAG_HIDDEN);
    }
    
    lv_obj_t* controls = ui_controls_get_container();
    if (controls != NULL) {
        lv_obj_clear_flag(controls, LV_OBJ_FLAG_HIDDEN);
    }
}

// ID textarea callback
static void id_textarea_cb(lv_event_t* e) {
    lv_obj_t* ta = lv_event_get_target(e);
    const char* text = lv_textarea_get_text(ta);
    ui_state_set_manual_id(text);
}

// Data textarea callback
static void data_textarea_cb(lv_event_t* e) {
    lv_obj_t* ta = lv_event_get_target(e);
    const char* text = lv_textarea_get_text(ta);
    ui_state_set_manual_data(text);
}

// Repeat switch callback
static void repeat_switch_cb(lv_event_t* e) {
    lv_obj_t* sw = lv_event_get_target(e);
    bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    // Show/hide interval input
    if (interval_container != NULL) {
        if (is_checked) {
            lv_obj_clear_flag(interval_container, LV_OBJ_FLAG_HIDDEN);
        } else {
            lv_obj_add_flag(interval_container, LV_OBJ_FLAG_HIDDEN);
        }
    }
    
    ui_state_t* state = ui_state_get();
    ui_state_set_manual_repeat(is_checked, state->manual_interval);
}

// Interval textarea callback
static void interval_textarea_cb(lv_event_t* e) {
    lv_obj_t* ta = lv_event_get_target(e);
    const char* text = lv_textarea_get_text(ta);
    uint32_t interval = atoi(text);
    if (interval < 100) interval = 100; // Minimum 100ms
    
    ui_state_t* state = ui_state_get();
    ui_state_set_manual_repeat(state->manual_repeat, interval);
}

lv_obj_t* ui_manual_input_create(lv_obj_t* parent, int y_offset) {
    // Create main container
    manual_container = lv_obj_create(parent);
    lv_obj_set_size(manual_container, UI_SCREEN_WIDTH, LV_SIZE_CONTENT);
    lv_obj_align(manual_container, LV_ALIGN_TOP_MID, 0, y_offset);
    lv_obj_set_style_bg_opa(manual_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(manual_container, 0, 0);
    lv_obj_set_style_pad_all(manual_container, UI_PADDING_LARGE, 0);
    lv_obj_set_style_pad_row(manual_container, 12, 0);
    lv_obj_set_flex_flow(manual_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_add_flag(manual_container, LV_OBJ_FLAG_HIDDEN); // Hidden by default
    lv_obj_clear_flag(manual_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Back button
    lv_obj_t* back_btn = lv_btn_create(manual_container);
    lv_obj_set_width(back_btn, lv_pct(100));
    lv_obj_set_height(back_btn, 36);
    lv_obj_set_style_bg_color(back_btn, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_bg_color(back_btn, UI_COLOR_BG_HOVER, LV_STATE_PRESSED);
    lv_obj_set_style_border_width(back_btn, 0, 0);
    lv_obj_set_style_radius(back_btn, UI_RADIUS_SMALL, 0);
    lv_obj_add_event_cb(back_btn, back_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* back_label = lv_label_create(back_btn);
    lv_label_set_text(back_label, LV_SYMBOL_LEFT " 返回");
    lv_obj_set_style_text_color(back_label, UI_COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(back_label, &lv_font_montserrat_12, 0);
    lv_obj_center(back_label);
    
    // CAN ID Input
    lv_obj_t* id_label = lv_label_create(manual_container);
    lv_label_set_text(id_label, "CAN ID");
    lv_obj_set_style_text_color(id_label, UI_COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(id_label, &lv_font_montserrat_12, 0);
    
    id_textarea = lv_textarea_create(manual_container);
    lv_obj_set_width(id_textarea, lv_pct(100));
    lv_textarea_set_one_line(id_textarea, true);
    lv_textarea_set_placeholder_text(id_textarea, "例如: 0x123");
    lv_obj_set_style_bg_color(id_textarea, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_border_color(id_textarea, UI_COLOR_BORDER_LIGHT, 0);
    lv_obj_set_style_text_color(id_textarea, UI_COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(id_textarea, &lv_font_montserrat_12, 0);
    lv_obj_add_event_cb(id_textarea, id_textarea_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // DATA Input
    lv_obj_t* data_label = lv_label_create(manual_container);
    lv_label_set_text(data_label, "DATA");
    lv_obj_set_style_text_color(data_label, UI_COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(data_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_pad_top(data_label, UI_GAP_MEDIUM, 0);
    
    data_textarea = lv_textarea_create(manual_container);
    lv_obj_set_width(data_textarea, lv_pct(100));
    lv_obj_set_height(data_textarea, 60);
    lv_textarea_set_placeholder_text(data_textarea, "例如: [0x01, 0x02, 0x03]");
    lv_obj_set_style_bg_color(data_textarea, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_border_color(data_textarea, UI_COLOR_BORDER_LIGHT, 0);
    lv_obj_set_style_text_color(data_textarea, UI_COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(data_textarea, &lv_font_montserrat_12, 0);
    lv_obj_add_event_cb(data_textarea, data_textarea_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Repeat toggle
    lv_obj_t* repeat_row = lv_obj_create(manual_container);
    lv_obj_set_size(repeat_row, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(repeat_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(repeat_row, 0, 0);
    lv_obj_set_style_pad_all(repeat_row, 0, 0);
    lv_obj_set_style_pad_top(repeat_row, UI_GAP_MEDIUM, 0);
    lv_obj_set_flex_flow(repeat_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(repeat_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    lv_obj_t* repeat_label = lv_label_create(repeat_row);
    lv_label_set_text(repeat_label, "周期发送");
    lv_obj_set_style_text_color(repeat_label, UI_COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(repeat_label, &lv_font_montserrat_12, 0);
    
    repeat_switch = lv_switch_create(repeat_row);
    lv_obj_set_size(repeat_switch, 36, 20);
    lv_obj_set_style_bg_color(repeat_switch, UI_COLOR_DISABLED_BG, 0);
    lv_obj_set_style_bg_color(repeat_switch, UI_COLOR_CYAN_500, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_event_cb(repeat_switch, repeat_switch_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Interval input (hidden by default)
    interval_container = lv_obj_create(manual_container);
    lv_obj_set_size(interval_container, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(interval_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(interval_container, 0, 0);
    lv_obj_set_style_pad_all(interval_container, 0, 0);
    lv_obj_add_flag(interval_container, LV_OBJ_FLAG_HIDDEN);
    
    lv_obj_t* interval_label = lv_label_create(interval_container);
    lv_label_set_text(interval_label, "周期间隔 (ms)");
    lv_obj_set_style_text_color(interval_label, UI_COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(interval_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_pad_bottom(interval_label, UI_GAP_SMALL, 0);
    
    interval_textarea = lv_textarea_create(interval_container);
    lv_obj_set_width(interval_textarea, lv_pct(100));
    lv_textarea_set_one_line(interval_textarea, true);
    lv_textarea_set_text(interval_textarea, "1000");
    lv_textarea_set_accepted_chars(interval_textarea, "0123456789");
    lv_obj_set_style_bg_color(interval_textarea, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_border_color(interval_textarea, UI_COLOR_BORDER_LIGHT, 0);
    lv_obj_set_style_text_color(interval_textarea, UI_COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(interval_textarea, &lv_font_montserrat_12, 0);
    lv_obj_add_event_cb(interval_textarea, interval_textarea_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    return manual_container;
}

void ui_manual_input_show(void) {
    if (manual_container != NULL) {
        lv_obj_clear_flag(manual_container, LV_OBJ_FLAG_HIDDEN);
    }
    
    lv_obj_t* controls = ui_controls_get_container();
    if (controls != NULL) {
        lv_obj_add_flag(controls, LV_OBJ_FLAG_HIDDEN);
    }
}

lv_obj_t* ui_manual_input_get_container(void) {
    return manual_container;
}
