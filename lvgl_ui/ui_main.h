/**
 * @file ui_main.h
 * @brief Main UI Header
 */

#ifndef UI_MAIN_H
#define UI_MAIN_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize and create the main UI
 * @return Main screen object
 */
lv_obj_t* ui_init(void);

/**
 * @brief Get the main screen object
 * @return Main screen object
 */
lv_obj_t* ui_get_screen(void);

/**
 * @brief Get controls container (for manual input mode switching)
 * @return Controls container object
 */
lv_obj_t* ui_controls_get_container(void);

// Component update functions (used by binding layer)
void ui_header_update_connection(bool connected);
void ui_log_add_message(const char* type, const char* message);
void ui_log_update_status(bool connected);
void ui_footer_update_status(bool transmitting, bool repeating);
void ui_footer_update_connection(bool connected);
void ui_manual_input_show(void);

#ifdef __cplusplus
}
#endif

#endif // UI_MAIN_H
