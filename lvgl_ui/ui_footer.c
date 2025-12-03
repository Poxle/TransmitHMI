/**
 * @file ui_footer.c
 * @brief Footer Component Implementation
 * 
 * Status indicator, STOP button, and TRANSMIT button
 */

#include "lvgl.h"
#include "ui_config.h"
#include "ui_state.h"
#include "ui_binding.h"

static lv_obj_t* footer_container = NULL;
static lv_obj_t* status_indicator = NULL;
static lv_obj_t* status_label = NULL;
static lv_obj_t* stop_btn = NULL;
static lv_obj_t* transmit_btn = NULL;

// STOP button callback
static void stop_btn_cb(lv_event_t* e) {
    ui_binding_trigger_stop();
    ui_state_set_transmission(false, false);
    ui_footer_update_status(false, false);
}

// TRANSMIT button callback
static void transmit_btn_cb(lv_event_t* e) {
    ui_state_t* state = ui_state_get();
    
    if (!state->is_connected) {
        return; // Can't transmit if not connected
    }
    
    if (state->view_mode == VIEW_MODE_AUTO) {
        // Auto mode
        uint32_t interval = 0;
        bool is_repeating = ui_config_is_repeating_function(
            state->selected_category, 
            state->selected_function, 
            &interval
        );
        
        ui_binding_trigger_transmit_auto(
            state->selected_scene,
            state->selected_category,
            state->selected_function,
            is_repeating,
            interval
        );
        
        ui_state_set_transmission(true, is_repeating);
        ui_footer_update_status(true, is_repeating);
        
    } else {
        // Manual mode
        if (state->manual_id[0] == '\0' || state->manual_data[0] == '\0') {
            return; // Need both ID and data
        }
        
        ui_binding_trigger_transmit_manual(
            state->manual_id,
            state->manual_data,
            state->manual_repeat,
            state->manual_interval
        );
        
        ui_state_set_transmission(true, state->manual_repeat);
        ui_footer_update_status(true, state->manual_repeat);
    }
}

lv_obj_t* ui_footer_create(lv_obj_t* parent) {
    // Create main container
    footer_container = lv_obj_create(parent);
    lv_obj_set_size(footer_container, UI_SCREEN_WIDTH, UI_FOOTER_HEIGHT);
    lv_obj_align(footer_container, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(footer_container, UI_COLOR_BG_CONTAINER, 0);
    lv_obj_set_style_border_width(footer_container, 1, 0);
    lv_obj_set_style_border_color(footer_container, UI_COLOR_BORDER_MAIN, 0);
    lv_obj_set_style_border_side(footer_container, LV_BORDER_SIDE_TOP, 0);
    lv_obj_set_style_radius(footer_container, 0, 0);
    lv_obj_set_style_pad_all(footer_container, UI_PADDING_LARGE, 0);
    lv_obj_set_style_pad_row(footer_container, UI_GAP_MEDIUM, 0);
    lv_obj_set_flex_flow(footer_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(footer_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Status row
    lv_obj_t* status_row = lv_obj_create(footer_container);
    lv_obj_set_size(status_row, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(status_row, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(status_row, 0, 0);
    lv_obj_set_style_pad_all(status_row, 0, 0);
    lv_obj_set_flex_flow(status_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    
    // Status indicator + label
    lv_obj_t* status_left = lv_obj_create(status_row);
    lv_obj_set_size(status_left, LV_SIZE_CONTENT, LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(status_left, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(status_left, 0, 0);
    lv_obj_set_style_pad_all(status_left, 0, 0);
    lv_obj_set_flex_flow(status_left, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_left, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_set_style_pad_column(status_left, UI_GAP_MEDIUM, 0);
    
    // Status indicator (circle)
    status_indicator = lv_obj_create(status_left);
    lv_obj_set_size(status_indicator, 12, 12);
    lv_obj_set_style_radius(status_indicator, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_bg_color(status_indicator, UI_COLOR_TEXT_DISABLED, 0);
    lv_obj_set_style_border_width(status_indicator, 0, 0);
    
    status_label = lv_label_create(status_left);
    lv_label_set_text(status_label, "就绪");
    lv_obj_set_style_text_color(status_label, UI_COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(status_label, &lv_font_montserrat_12, 0);
    
    // STOP button
    stop_btn = lv_btn_create(status_row);
    lv_obj_set_size(stop_btn, 60, 28);
    lv_obj_set_style_bg_color(stop_btn, UI_COLOR_DISABLED_BG, 0);
    lv_obj_set_style_border_width(stop_btn, 0, 0);
    lv_obj_set_style_radius(stop_btn, UI_RADIUS_SMALL, 0);
    lv_obj_add_state(stop_btn, LV_STATE_DISABLED);
    lv_obj_add_event_cb(stop_btn, stop_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* stop_label = lv_label_create(stop_btn);
    lv_label_set_text(stop_label, LV_SYMBOL_STOP " STOP");
    lv_obj_set_style_text_color(stop_label, UI_COLOR_TEXT_MUTED, 0);
    lv_obj_set_style_text_font(stop_label, &lv_font_montserrat_12, 0);
    lv_obj_center(stop_label);
    
    // TRANSMIT button
    transmit_btn = lv_btn_create(footer_container);
    lv_obj_set_size(transmit_btn, lv_pct(100), 40);
    lv_obj_set_style_bg_color(transmit_btn, UI_COLOR_DISABLED_BG, 0);
    lv_obj_set_style_bg_color(transmit_btn, UI_COLOR_CYAN_600, LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(transmit_btn, UI_COLOR_CYAN_500, LV_STATE_PRESSED);
    lv_obj_set_style_border_width(transmit_btn, 0, 0);
    lv_obj_set_style_radius(transmit_btn, UI_RADIUS_SMALL, 0);
    lv_obj_add_event_cb(transmit_btn, transmit_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* transmit_label = lv_label_create(transmit_btn);
    lv_label_set_text(transmit_label, LV_SYMBOL_UPLOAD " TRANSMIT");
    lv_obj_set_style_text_color(transmit_label, UI_COLOR_WHITE, 0);
    lv_obj_set_style_text_font(transmit_label, &lv_font_montserrat_12, 0);
    lv_obj_center(transmit_label);
    
    return footer_container;
}

void ui_footer_update_status(bool transmitting, bool repeating) {
    if (status_indicator == NULL || status_label == NULL || 
        stop_btn == NULL || transmit_btn == NULL) {
        return;
    }
    
    if (repeating) {
        // Repeating mode
        lv_obj_set_style_bg_color(status_indicator, UI_COLOR_GREEN_400, 0);
        lv_label_set_text(status_label, "重复");
        lv_obj_set_style_text_color(status_label, UI_COLOR_GREEN_400, 0);
        
        // Enable stop, disable transmit
        lv_obj_clear_state(stop_btn, LV_STATE_DISABLED);
        lv_obj_set_style_bg_color(stop_btn, UI_COLOR_RED_600, 0);
        lv_obj_set_style_text_color(lv_obj_get_child(stop_btn, 0), UI_COLOR_WHITE, 0);
        
        lv_obj_add_state(transmit_btn, LV_STATE_DISABLED);
        lv_obj_set_style_bg_color(transmit_btn, UI_COLOR_DISABLED_BG, 0);
        lv_obj_set_style_text_color(lv_obj_get_child(transmit_btn, 0), UI_COLOR_TEXT_MUTED, 0);
        
    } else if (transmitting) {
        // Single transmission
        lv_obj_set_style_bg_color(status_indicator, UI_COLOR_GREEN_400, 0);
        lv_label_set_text(status_label, "发送中");
        lv_obj_set_style_text_color(status_label, UI_COLOR_GREEN_400, 0);
        
        // Disable both buttons during transmission
        lv_obj_add_state(stop_btn, LV_STATE_DISABLED);
        lv_obj_set_style_bg_color(stop_btn, UI_COLOR_DISABLED_BG, 0);
        lv_obj_set_style_text_color(lv_obj_get_child(stop_btn, 0), UI_COLOR_TEXT_MUTED, 0);
        
        lv_obj_add_state(transmit_btn, LV_STATE_DISABLED);
        lv_obj_set_style_bg_color(transmit_btn, UI_COLOR_DISABLED_BG, 0);
        lv_obj_set_style_text_color(lv_obj_get_child(transmit_btn, 0), UI_COLOR_TEXT_MUTED, 0);
        
    } else {
        // Ready state
        lv_obj_set_style_bg_color(status_indicator, UI_COLOR_TEXT_DISABLED, 0);
        lv_label_set_text(status_label, "就绪");
        lv_obj_set_style_text_color(status_label, UI_COLOR_TEXT_SECONDARY, 0);
        
        // Disable stop, enable transmit
        lv_obj_add_state(stop_btn, LV_STATE_DISABLED);
        lv_obj_set_style_bg_color(stop_btn, UI_COLOR_DISABLED_BG, 0);
        lv_obj_set_style_text_color(lv_obj_get_child(stop_btn, 0), UI_COLOR_TEXT_MUTED, 0);
        
        ui_state_t* state = ui_state_get();
        if (state->is_connected) {
            lv_obj_clear_state(transmit_btn, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(transmit_btn, UI_COLOR_CYAN_600, 0);
            lv_obj_set_style_text_color(lv_obj_get_child(transmit_btn, 0), UI_COLOR_WHITE, 0);
        } else {
            lv_obj_add_state(transmit_btn, LV_STATE_DISABLED);
            lv_obj_set_style_bg_color(transmit_btn, UI_COLOR_DISABLED_BG, 0);
            lv_obj_set_style_text_color(lv_obj_get_child(transmit_btn, 0), UI_COLOR_TEXT_MUTED, 0);
        }
    }
}

void ui_footer_update_connection(bool connected) {
    ui_state_t* state = ui_state_get();
    if (!state->is_transmitting && !state->is_repeating) {
        ui_footer_update_status(false, false);
    }
}
