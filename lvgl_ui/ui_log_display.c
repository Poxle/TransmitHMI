/**
 * @file ui_log_display.c
 * @brief Log Display Component Implementation
 * 
 * Scrollable log area showing TX/RX messages with timestamps
 */

#include "lvgl.h"
#include "ui_config.h"
#include "ui_state.h"
#include "ui_binding.h"
#include <stdio.h>
#include <time.h>

static lv_obj_t* log_container = NULL;
static lv_obj_t* log_textarea = NULL;
static lv_obj_t* clear_btn = NULL;
static lv_obj_t* status_label = NULL;

// Clear button callback
static void clear_btn_cb(lv_event_t* e) {
    if (log_textarea != NULL) {
        lv_textarea_set_text(log_textarea, "");
        ui_state_reset_log_count();
        ui_binding_trigger_clear_logs();
        
        // Show status message
        ui_state_t* state = ui_state_get();
        if (state->is_connected) {
            lv_label_set_text(status_label, "已连接 - 等待发送...");
            lv_obj_clear_flag(status_label, LV_OBJ_FLAG_HIDDEN);
        }
    }
}

lv_obj_t* ui_log_display_create(lv_obj_t* parent, int y_offset) {
    // Create main container
    log_container = lv_obj_create(parent);
    lv_obj_set_size(log_container, UI_SCREEN_WIDTH, UI_LOG_HEIGHT + 60);
    lv_obj_align(log_container, LV_ALIGN_TOP_MID, 0, y_offset);
    lv_obj_set_style_bg_color(log_container, UI_COLOR_BLACK, 0);
    lv_obj_set_style_border_width(log_container, 0, 0);
    lv_obj_set_style_pad_all(log_container, UI_PADDING_MEDIUM, 0);
    lv_obj_set_style_pad_row(log_container, UI_GAP_MEDIUM, 0);
    lv_obj_set_flex_flow(log_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(log_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create log text area
    log_textarea = lv_textarea_create(log_container);
    lv_obj_set_size(log_textarea, lv_pct(100), UI_LOG_HEIGHT);
    lv_textarea_set_text(log_textarea, "");
    lv_obj_set_style_bg_color(log_textarea, UI_COLOR_BG_MAIN, 0);
    lv_obj_set_style_border_color(log_textarea, UI_COLOR_BORDER_MAIN, 0);
    lv_obj_set_style_border_width(log_textarea, 1, 0);
    lv_obj_set_style_radius(log_textarea, UI_RADIUS_SMALL, 0);
    lv_obj_set_style_text_color(log_textarea, UI_COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(log_textarea, &lv_font_montserrat_10, 0);
    lv_obj_set_style_pad_all(log_textarea, UI_PADDING_MEDIUM, 0);
    lv_textarea_set_one_line(log_textarea, false);
    
    // Create status label (shown when no logs)
    status_label = lv_label_create(log_textarea);
    lv_label_set_text(status_label, "未连接");
    lv_obj_set_style_text_color(status_label, UI_COLOR_TEXT_DISABLED, 0);
    lv_obj_center(status_label);
    
    // Create clear button
    clear_btn = lv_btn_create(log_container);
    lv_obj_set_size(clear_btn, lv_pct(100), 32);
    lv_obj_set_style_bg_color(clear_btn, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_bg_color(clear_btn, UI_COLOR_BG_HOVER, LV_STATE_PRESSED);
    lv_obj_set_style_border_width(clear_btn, 0, 0);
    lv_obj_set_style_radius(clear_btn, UI_RADIUS_SMALL, 0);
    lv_obj_add_event_cb(clear_btn, clear_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* btn_label = lv_label_create(clear_btn);
    lv_label_set_text(btn_label, LV_SYMBOL_TRASH " 清空日志");
    lv_obj_set_style_text_color(btn_label, UI_COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_12, 0);
    lv_obj_center(btn_label);
    
    return log_container;
}

void ui_log_add_message(const char* type, const char* message) {
    if (log_textarea == NULL || type == NULL || message == NULL) {
        return;
    }
    
    // Hide status label once we have logs
    if (status_label != NULL) {
        lv_obj_add_flag(status_label, LV_OBJ_FLAG_HIDDEN);
    }
    
    // Get current time
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[16];
    snprintf(timestamp, sizeof(timestamp), "%02d:%02d:%02d", 
             tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
    
    // Format log entry
    char log_entry[256];
    snprintf(log_entry, sizeof(log_entry), "%s [%s] %s\n", 
             timestamp, type, message);
    
    // Add to textarea
    lv_textarea_add_text(log_textarea, log_entry);
    
    // Auto-scroll to bottom
    lv_obj_scroll_to_y(log_textarea, lv_obj_get_scroll_bottom(log_textarea), LV_ANIM_ON);
}

void ui_log_update_status(bool connected) {
    if (status_label == NULL) {
        return;
    }
    
    ui_state_t* state = ui_state_get();
    if (state->log_count == 0) {
        lv_obj_clear_flag(status_label, LV_OBJ_FLAG_HIDDEN);
        if (connected) {
            lv_label_set_text(status_label, "已连接 - 等待发送...");
            lv_obj_set_style_text_color(status_label, UI_COLOR_GREEN_400, 0);
        } else {
            lv_label_set_text(status_label, "未连接");
            lv_obj_set_style_text_color(status_label, UI_COLOR_TEXT_DISABLED, 0);
        }
        lv_obj_center(status_label);
    }
}
