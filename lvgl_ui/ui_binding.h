/**
 * @file ui_binding.h
 * @brief Data Binding Layer for Backend Integration
 * 
 * Defines callback interfaces for connecting the LVGL UI to the backend.
 * UI events trigger callbacks, and backend can update UI through provided functions.
 */

#ifndef UI_BINDING_H
#define UI_BINDING_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Callback when connection state changes
 * @param connected true if connected, false if disconnected
 */
typedef void (*connection_callback_t)(bool connected);

/**
 * @brief Callback when transmit is requested in auto mode
 * @param scene Selected scene (e.g., "B", "BA")
 * @param category Category index (0=Display, 1=Sound, 2=Inspection)
 * @param function Function index within category
 * @param repeat true if this is a repeating function
 * @param interval Interval in ms (only relevant for repeating functions)
 */
typedef void (*transmit_auto_callback_t)(const char* scene, uint8_t category, 
                                         uint8_t function, bool repeat, uint32_t interval);

/**
 * @brief Callback when transmit is requested in manual mode
 * @param can_id CAN ID string (e.g., "0x123")
 * @param data Data string (e.g., "[0x01, 0x02, 0x03]")
 * @param repeat true if repeat is enabled
 * @param interval Repeat interval in ms
 */
typedef void (*transmit_manual_callback_t)(const char* can_id, const char* data,
                                           bool repeat, uint32_t interval);

/**
 * @brief Callback when stop is requested
 */
typedef void (*stop_callback_t)(void);

/**
 * @brief Callback when scene is selected (informational)
 * @param scene Scene identifier
 */
typedef void (*scene_callback_t)(const char* scene);

/**
 * @brief Callback when logs are cleared
 */
typedef void (*clear_logs_callback_t)(void);

/**
 * @brief Structure holding all registered callbacks
 */
typedef struct {
    connection_callback_t on_connection_changed;
    transmit_auto_callback_t on_transmit_auto;
    transmit_manual_callback_t on_transmit_manual;
    stop_callback_t on_stop;
    scene_callback_t on_scene_selected;
    clear_logs_callback_t on_clear_logs;
} ui_callbacks_t;

/**
 * @brief Initialize data binding system
 */
void ui_binding_init(void);

/**
 * @brief Register all callbacks at once
 * @param callbacks Pointer to callback structure
 */
void ui_binding_register_callbacks(const ui_callbacks_t* callbacks);

/**
 * @brief Register connection change callback
 * @param callback Callback function
 */
void ui_binding_register_connection_callback(connection_callback_t callback);

/**
 * @brief Register auto mode transmit callback
 * @param callback Callback function
 */
void ui_binding_register_transmit_auto_callback(transmit_auto_callback_t callback);

/**
 * @brief Register manual mode transmit callback
 * @param callback Callback function
 */
void ui_binding_register_transmit_manual_callback(transmit_manual_callback_t callback);

/**
 * @brief Register stop callback
 * @param callback Callback function
 */
void ui_binding_register_stop_callback(stop_callback_t callback);

/**
 * @brief Register scene selection callback
 * @param callback Callback function
 */
void ui_binding_register_scene_callback(scene_callback_t callback);

/**
 * @brief Register clear logs callback
 * @param callback Callback function
 */
void ui_binding_register_clear_logs_callback(clear_logs_callback_t callback);

/**
 * @brief Trigger connection changed event (called by UI)
 * @param connected New connection state
 */
void ui_binding_trigger_connection_changed(bool connected);

/**
 * @brief Trigger transmit auto event (called by UI)
 * @param scene Scene identifier
 * @param category Category index
 * @param function Function index
 * @param repeat Repeat enabled
 * @param interval Interval in ms
 */
void ui_binding_trigger_transmit_auto(const char* scene, uint8_t category,
                                      uint8_t function, bool repeat, uint32_t interval);

/**
 * @brief Trigger transmit manual event (called by UI)
 * @param can_id CAN ID string
 * @param data Data string
 * @param repeat Repeat enabled
 * @param interval Interval in ms
 */
void ui_binding_trigger_transmit_manual(const char* can_id, const char* data,
                                        bool repeat, uint32_t interval);

/**
 * @brief Trigger stop event (called by UI)
 */
void ui_binding_trigger_stop(void);

/**
 * @brief Trigger scene selected event (called by UI)
 * @param scene Scene identifier
 */
void ui_binding_trigger_scene_selected(const char* scene);

/**
 * @brief Trigger clear logs event (called by UI)
 */
void ui_binding_trigger_clear_logs(void);

// ==================== Backend → UI Functions ====================

/**
 * @brief Add a log message to the UI (called by backend)
 * @param type "TX" or "RX"
 * @param message Log message content
 */
void ui_binding_add_log(const char* type, const char* message);

/**
 * @brief Update transmission status (called by backend)
 * @param transmitting true if currently transmitting
 * @param repeating true if in repeating mode
 */
void ui_binding_update_transmission_status(bool transmitting, bool repeating);

/**
 * @brief Update connection status from backend
 * @param connected Connection state
 */
void ui_binding_update_connection_status(bool connected);

#ifdef __cplusplus
}
#endif

#endif // UI_BINDING_H
