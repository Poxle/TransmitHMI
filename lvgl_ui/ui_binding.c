/**
 * @file ui_binding.c
 * @brief Data Binding Layer Implementation
 */

#include "ui_binding.h"
#include "ui_state.h"
#include <string.h>

// Registered callbacks
static ui_callbacks_t g_callbacks = {0};

// Forward declarations of UI update functions
extern void ui_log_add_message(const char* type, const char* message);
extern void ui_footer_update_status(bool transmitting, bool repeating);
extern void ui_header_update_connection(bool connected);

void ui_binding_init(void) {
    memset(&g_callbacks, 0, sizeof(ui_callbacks_t));
}

void ui_binding_register_callbacks(const ui_callbacks_t* callbacks) {
    if (callbacks != NULL) {
        memcpy(&g_callbacks, callbacks, sizeof(ui_callbacks_t));
    }
}

void ui_binding_register_connection_callback(connection_callback_t callback) {
    g_callbacks.on_connection_changed = callback;
}

void ui_binding_register_transmit_auto_callback(transmit_auto_callback_t callback) {
    g_callbacks.on_transmit_auto = callback;
}

void ui_binding_register_transmit_manual_callback(transmit_manual_callback_t callback) {
    g_callbacks.on_transmit_manual = callback;
}

void ui_binding_register_stop_callback(stop_callback_t callback) {
    g_callbacks.on_stop = callback;
}

void ui_binding_register_scene_callback(scene_callback_t callback) {
    g_callbacks.on_scene_selected = callback;
}

void ui_binding_register_clear_logs_callback(clear_logs_callback_t callback) {
    g_callbacks.on_clear_logs = callback;
}

// ==================== UI → Backend (Trigger Events) ====================

void ui_binding_trigger_connection_changed(bool connected) {
    ui_state_set_connected(connected);
    if (g_callbacks.on_connection_changed != NULL) {
        g_callbacks.on_connection_changed(connected);
    }
}

void ui_binding_trigger_transmit_auto(const char* scene, uint8_t category,
                                      uint8_t function, bool repeat, uint32_t interval) {
    if (g_callbacks.on_transmit_auto != NULL) {
        g_callbacks.on_transmit_auto(scene, category, function, repeat, interval);
    }
}

void ui_binding_trigger_transmit_manual(const char* can_id, const char* data,
                                        bool repeat, uint32_t interval) {
    if (g_callbacks.on_transmit_manual != NULL) {
        g_callbacks.on_transmit_manual(can_id, data, repeat, interval);
    }
}

void ui_binding_trigger_stop(void) {
    if (g_callbacks.on_stop != NULL) {
        g_callbacks.on_stop();
    }
}

void ui_binding_trigger_scene_selected(const char* scene) {
    if (g_callbacks.on_scene_selected != NULL) {
        g_callbacks.on_scene_selected(scene);
    }
}

void ui_binding_trigger_clear_logs(void) {
    if (g_callbacks.on_clear_logs != NULL) {
        g_callbacks.on_clear_logs();
    }
}

// ==================== Backend → UI (Update Functions) ====================

void ui_binding_add_log(const char* type, const char* message) {
    ui_state_increment_log_count();
    ui_log_add_message(type, message);
}

void ui_binding_update_transmission_status(bool transmitting, bool repeating) {
    ui_state_set_transmission(transmitting, repeating);
    ui_footer_update_status(transmitting, repeating);
}

void ui_binding_update_connection_status(bool connected) {
    ui_state_set_connected(connected);
    ui_header_update_connection(connected);
}
