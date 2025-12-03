/**
 * @file ui_main.c
 * @brief Main UI Initialization
 * 
 * Creates and initializes all UI components
 */

#include "lvgl.h"
#include "ui_config.h"
#include "ui_state.h"
#include "ui_binding.h"

// Component creation functions
extern lv_obj_t* ui_header_create(lv_obj_t* parent);
extern lv_obj_t* ui_log_display_create(lv_obj_t* parent, int y_offset);
extern lv_obj_t* ui_controls_create(lv_obj_t* parent, int y_offset);
extern lv_obj_t* ui_manual_input_create(lv_obj_t* parent, int y_offset);
extern lv_obj_t* ui_footer_create(lv_obj_t* parent);
extern void ui_log_update_status(bool connected);

// Global screen object
static lv_obj_t* main_screen = NULL;
static lv_obj_t* controls_container = NULL;

lv_obj_t* ui_controls_get_container(void) {
    return controls_container;
}

/**
 * @brief Initialize and create the main UI
 * @return Main screen object
 */
lv_obj_t* ui_init(void) {
    // Initialize state
    ui_state_init();
    
    // Initialize binding
    ui_binding_init();
    
    // Create main screen
    main_screen = lv_obj_create(NULL);
    lv_obj_set_size(main_screen, UI_SCREEN_WIDTH, UI_SCREEN_HEIGHT);
    lv_obj_set_style_bg_color(main_screen, UI_COLOR_BG_MAIN, 0);
    lv_obj_set_style_border_width(main_screen, 0, 0);
    lv_obj_set_style_pad_all(main_screen, 0, 0);
    lv_obj_clear_flag(main_screen, LV_OBJ_FLAG_SCROLLABLE);
    
    // Create header (top)
    ui_header_create(main_screen);
    
    // Create log display (below header)
    ui_log_display_create(main_screen, UI_HEADER_HEIGHT);
    
    // Create scrollable content area
    int content_y = UI_HEADER_HEIGHT + UI_LOG_HEIGHT + 60;
    int content_height = UI_SCREEN_HEIGHT - content_y - UI_FOOTER_HEIGHT;
    
    lv_obj_t* content_area = lv_obj_create(main_screen);
    lv_obj_set_size(content_area, UI_SCREEN_WIDTH, content_height);
    lv_obj_align(content_area, LV_ALIGN_TOP_MID, 0, content_y);
    lv_obj_set_style_bg_opa(content_area, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(content_area, 0, 0);
    lv_obj_set_style_pad_all(content_area, 0, 0);
    lv_obj_set_scrollbar_mode(content_area, LV_SCROLLBAR_MODE_AUTO);
    
    // Create auto mode controls
    controls_container = ui_controls_create(content_area, 0);
    
    // Create manual input mode (hidden initially)
    ui_manual_input_create(content_area, 0);
    
    // Create footer (bottom)
    ui_footer_create(main_screen);
    
    // Load the screen
    lv_scr_load(main_screen);
    
    return main_screen;
}

/**
 * @brief Get the main screen object
 * @return Main screen object
 */
lv_obj_t* ui_get_screen(void) {
    return main_screen;
}
