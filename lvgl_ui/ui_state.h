/**
 * @file ui_state.h
 * @brief UI State Management for CAN Signal Sender Panel
 * 
 * Centralized state management for the LVGL v9 UI.
 * Maintains all application state including connection status,
 * transmission state, mode selection, and user inputs.
 */

#ifndef UI_STATE_H
#define UI_STATE_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Function categories for auto mode
 */
typedef enum {
    CATEGORY_DISPLAY = 0,      // 显示 (Display)
    CATEGORY_SOUND = 1,        // 声音 (Sound)
    CATEGORY_INSPECTION = 2,   // 检查 (Inspection)
    CATEGORY_COUNT
} ui_category_t;

/**
 * @brief View mode selection
 */
typedef enum {
    VIEW_MODE_AUTO = 0,    // Auto mode with scene/function selection
    VIEW_MODE_MANUAL = 1   // Manual CAN ID/Data input mode
} ui_view_mode_t;

/**
 * @brief Main UI state structure
 */
typedef struct {
    // Connection state
    bool is_connected;
    
    // Transmission state
    bool is_transmitting;
    bool is_repeating;
    
    // Auto mode state
    char selected_scene[8];           // Current scene (B, BA, IGP, etc.)
    ui_category_t selected_category;  // Current category
    uint8_t selected_function;        // Index of selected function in category
    
    // View mode
    ui_view_mode_t view_mode;
    
    // Manual mode state
    char manual_id[32];               // CAN ID input (e.g., "0x123")
    char manual_data[128];            // Data input (e.g., "[0x01, 0x02]")
    bool manual_repeat;               // Repeat enabled
    uint32_t manual_interval;         // Repeat interval in ms
    
    // Log count
    uint16_t log_count;
} ui_state_t;

/**
 * @brief Initialize UI state with default values
 */
void ui_state_init(void);

/**
 * @brief Get pointer to current UI state
 * @return Pointer to global UI state
 */
ui_state_t* ui_state_get(void);

/**
 * @brief Set connection state
 * @param connected true if connected, false otherwise
 */
void ui_state_set_connected(bool connected);

/**
 * @brief Set transmission state
 * @param transmitting true if transmitting
 * @param repeating true if in repeating mode
 */
void ui_state_set_transmission(bool transmitting, bool repeating);

/**
 * @brief Set selected scene
 * @param scene Scene identifier (e.g., "B", "BA", "IGP")
 */
void ui_state_set_scene(const char* scene);

/**
 * @brief Set selected category
 * @param category Category enum value
 */
void ui_state_set_category(ui_category_t category);

/**
 * @brief Set selected function index
 * @param function_index Index of function in current category
 */
void ui_state_set_function(uint8_t function_index);

/**
 * @brief Set view mode
 * @param mode View mode (auto or manual)
 */
void ui_state_set_view_mode(ui_view_mode_t mode);

/**
 * @brief Set manual mode CAN ID
 * @param id CAN ID string
 */
void ui_state_set_manual_id(const char* id);

/**
 * @brief Set manual mode data
 * @param data Data string
 */
void ui_state_set_manual_data(const char* data);

/**
 * @brief Set manual repeat settings
 * @param repeat Enable/disable repeat
 * @param interval Interval in milliseconds
 */
void ui_state_set_manual_repeat(bool repeat, uint32_t interval);

/**
 * @brief Increment log count
 */
void ui_state_increment_log_count(void);

/**
 * @brief Reset log count
 */
void ui_state_reset_log_count(void);

#ifdef __cplusplus
}
#endif

#endif // UI_STATE_H
