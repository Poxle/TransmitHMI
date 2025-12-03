/**
 * @file ui_state.c
 * @brief UI State Management Implementation
 */

#include "ui_state.h"
#include <string.h>

// Global UI state
static ui_state_t g_ui_state = {0};

void ui_state_init(void) {
    memset(&g_ui_state, 0, sizeof(ui_state_t));
    
    // Set defaults
    g_ui_state.is_connected = false;
    g_ui_state.is_transmitting = false;
    g_ui_state.is_repeating = false;
    
    strcpy(g_ui_state.selected_scene, "B");
    g_ui_state.selected_category = CATEGORY_DISPLAY;
    g_ui_state.selected_function = 0;
    
    g_ui_state.view_mode = VIEW_MODE_AUTO;
    
    g_ui_state.manual_id[0] = '\0';
    g_ui_state.manual_data[0] = '\0';
    g_ui_state.manual_repeat = false;
    g_ui_state.manual_interval = 1000;
    
    g_ui_state.log_count = 0;
}

ui_state_t* ui_state_get(void) {
    return &g_ui_state;
}

void ui_state_set_connected(bool connected) {
    g_ui_state.is_connected = connected;
}

void ui_state_set_transmission(bool transmitting, bool repeating) {
    g_ui_state.is_transmitting = transmitting;
    g_ui_state.is_repeating = repeating;
}

void ui_state_set_scene(const char* scene) {
    if (scene != NULL) {
        strncpy(g_ui_state.selected_scene, scene, sizeof(g_ui_state.selected_scene) - 1);
        g_ui_state.selected_scene[sizeof(g_ui_state.selected_scene) - 1] = '\0';
    }
}

void ui_state_set_category(ui_category_t category) {
    if (category < CATEGORY_COUNT) {
        g_ui_state.selected_category = category;
        // Reset function to first in category
        g_ui_state.selected_function = 0;
    }
}

void ui_state_set_function(uint8_t function_index) {
    g_ui_state.selected_function = function_index;
}

void ui_state_set_view_mode(ui_view_mode_t mode) {
    g_ui_state.view_mode = mode;
}

void ui_state_set_manual_id(const char* id) {
    if (id != NULL) {
        strncpy(g_ui_state.manual_id, id, sizeof(g_ui_state.manual_id) - 1);
        g_ui_state.manual_id[sizeof(g_ui_state.manual_id) - 1] = '\0';
    }
}

void ui_state_set_manual_data(const char* data) {
    if (data != NULL) {
        strncpy(g_ui_state.manual_data, data, sizeof(g_ui_state.manual_data) - 1);
        g_ui_state.manual_data[sizeof(g_ui_state.manual_data) - 1] = '\0';
    }
}

void ui_state_set_manual_repeat(bool repeat, uint32_t interval) {
    g_ui_state.manual_repeat = repeat;
    g_ui_state.manual_interval = interval;
}

void ui_state_increment_log_count(void) {
    g_ui_state.log_count++;
}

void ui_state_reset_log_count(void) {
    g_ui_state.log_count = 0;
}
