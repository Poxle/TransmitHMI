/**
 * @file ui_header.c
 * @brief Header Component Implementation
 * 
 * Header with "CAN BUS TX" label, radio icon, and connection toggle switch
 */

#include "lvgl.h"
#include "ui_config.h"
#include "ui_state.h"
#include "ui_binding.h"

static lv_obj_t* header_container = NULL;
static lv_obj_t* conn_switch = NULL;

// Switch event callback
static void switch_event_cb(lv_event_t* e) {
    lv_obj_t* sw = lv_event_get_target(e);
    bool is_checked = lv_obj_has_state(sw, LV_STATE_CHECKED);
    
    // Trigger binding event
    ui_binding_trigger_connection_changed(is_checked);
}

lv_obj_t* ui_header_create(lv_obj_t* parent) {
    // Create header container
    header_container = lv_obj_create(parent);
    lv_obj_set_size(header_container, UI_SCREEN_WIDTH, UI_HEADER_HEIGHT);
    lv_obj_align(header_container, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(header_container, UI_COLOR_BG_CONTAINER, 0);
    lv_obj_set_style_bg_grad_color(header_container, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_bg_grad_dir(header_container, LV_GRAD_DIR_HOR, 0);
    lv_obj_set_style_border_width(header_container, 1, 0);
    lv_obj_set_style_border_color(header_container, UI_COLOR_BORDER_LIGHT, 0);
    lv_obj_set_style_border_side(header_container, LV_BORDER_SIDE_BOTTOM, 0);
    lv_obj_set_style_radius(header_container, 0, 0);
    lv_obj_set_style_pad_all(header_container, UI_PADDING_LARGE, 0);
    lv_obj_clear_flag(header_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create flex layout for header content
    lv_obj_set_flex_flow(header_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header_container, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Left side: Icon + Text
    lv_obj_t* left_container = lv_obj_create(header_container);
    lv_obj_set_size(left_container, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(left_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(left_container, 0, 0);
    lv_obj_set_style_pad_all(left_container, 0, 0);
    lv_obj_set_flex_flow(left_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(left_container, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(left_container, UI_GAP_MEDIUM, 0);
    
    // Radio icon (using a simple circle as placeholder - can be replaced with actual icon)
    lv_obj_t* icon = lv_obj_create(left_container);
    lv_obj_set_size(icon, 16, 16);
    lv_obj_set_style_radius(icon, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(icon, UI_COLOR_CYAN_400, 0);
    lv_obj_set_style_border_width(icon, 2, 0);
    lv_obj_set_style_border_color(icon, UI_COLOR_CYAN_400, 0);
    
    // Title label
    lv_obj_t* label = lv_label_create(left_container);
    lv_label_set_text(label, "CAN BUS TX");
    lv_obj_set_style_text_color(label, UI_COLOR_CYAN_400, 0);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_12, 0);
    
    // Right side: Connection switch
    conn_switch = lv_switch_create(header_container);
    lv_obj_set_size(conn_switch, 36, 20);
    lv_obj_set_style_bg_color(conn_switch, UI_COLOR_DISABLED_BG, 0);
    lv_obj_set_style_bg_color(conn_switch, UI_COLOR_GREEN_500, LV_PART_INDICATOR | LV_STATE_CHECKED);
    lv_obj_add_event_cb(conn_switch, switch_event_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    return header_container;
}

void ui_header_update_connection(bool connected) {
    if (conn_switch != NULL) {
        if (connected) {
            lv_obj_add_state(conn_switch, LV_STATE_CHECKED);
        } else {
            lv_obj_clear_state(conn_switch, LV_STATE_CHECKED);
        }
    }
}
