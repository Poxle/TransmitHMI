/**
 * @file ui_controls.c
 * @brief Auto Mode Control Panel Implementation
 * 
 * Scene selection, category/function dropdowns, and manual mode button
 */

#include "lvgl.h"
#include "ui_config.h"
#include "ui_state.h"
#include "ui_binding.h"

static lv_obj_t* controls_container = NULL;
static lv_obj_t* scene_buttons[6] = {NULL};
static lv_obj_t* category_dropdown = NULL;
static lv_obj_t* function_dropdown = NULL;
static lv_obj_t* manual_btn = NULL;

// Forward declaration
extern void ui_manual_input_show(void);

// Scene button callback
static void scene_btn_cb(lv_event_t* e) {
    lv_obj_t* btn = lv_event_get_target(e);
    uint32_t* idx = (uint32_t*)lv_event_get_user_data(e);
    
    if (idx == NULL || *idx >= UI_SCENES_COUNT) {
        return;
    }
    
    // Update state
    ui_state_set_scene(UI_SCENES[*idx]);
    
    // Update button styles
    for (uint8_t i = 0; i < UI_SCENES_COUNT; i++) {
        if (i == *idx) {
            lv_obj_set_style_bg_color(scene_buttons[i], UI_COLOR_CYAN_600, 0);
            lv_obj_set_style_text_color(lv_obj_get_child(scene_buttons[i], 0), UI_COLOR_WHITE, 0);
        } else {
            lv_obj_set_style_bg_color(scene_buttons[i], UI_COLOR_BG_INPUT, 0);
            lv_obj_set_style_text_color(lv_obj_get_child(scene_buttons[i], 0), UI_COLOR_TEXT_PRIMARY, 0);
        }
    }
    
    // Trigger binding
    ui_binding_trigger_scene_selected(UI_SCENES[*idx]);
}

// Category dropdown callback
static void category_dd_cb(lv_event_t* e) {
    lv_obj_t* dd = lv_event_get_target(e);
    uint16_t sel = lv_dropdown_get_selected(dd);
    
    ui_state_set_category((ui_category_t)sel);
    
    // Update function dropdown
    if (function_dropdown != NULL) {
        lv_dropdown_clear_options(function_dropdown);
        
        const char** functions = NULL;
        uint8_t count = 0;
        
        switch (sel) {
            case 0: // Display
                functions = UI_FUNCTIONS_DISPLAY;
                count = UI_FUNCTIONS_DISPLAY_COUNT;
                break;
            case 1: // Sound
                functions = UI_FUNCTIONS_SOUND;
                count = UI_FUNCTIONS_SOUND_COUNT;
                break;
            case 2: // Inspection
                functions = UI_FUNCTIONS_INSPECTION;
                count = UI_FUNCTIONS_INSPECTION_COUNT;
                break;
        }
        
        for (uint8_t i = 0; i < count; i++) {
            lv_dropdown_add_option(function_dropdown, functions[i], i);
        }
        lv_dropdown_set_selected(function_dropdown, 0);
    }
}

// Function dropdown callback
static void function_dd_cb(lv_event_t* e) {
    lv_obj_t* dd = lv_event_get_target(e);
    uint16_t sel = lv_dropdown_get_selected(dd);
    ui_state_set_function((uint8_t)sel);
}

// Manual button callback
static void manual_btn_cb(lv_event_t* e) {
    ui_state_set_view_mode(VIEW_MODE_MANUAL);
    ui_manual_input_show();
}

lv_obj_t* ui_controls_create(lv_obj_t* parent, int y_offset) {
    // Create main container
    controls_container = lv_obj_create(parent);
    lv_obj_set_size(controls_container, UI_SCREEN_WIDTH, LV_SIZE_CONTENT);
    lv_obj_align(controls_container, LV_ALIGN_TOP_MID, 0, y_offset);
    lv_obj_set_style_bg_opa(controls_container, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(controls_container, 0, 0);
    lv_obj_set_style_pad_all(controls_container, UI_PADDING_LARGE, 0);
    lv_obj_set_style_pad_row(controls_container, 16, 0);
    lv_obj_set_flex_flow(controls_container, LV_FLEX_FLOW_COLUMN);
    lv_obj_clear_flag(controls_container, LV_OBJ_FLAG_SCROLLABLE);
    
    // Scene Selection Section
    lv_obj_t* scene_label = lv_label_create(controls_container);
    lv_label_set_text(scene_label, "场景发送 (SCENE)");
    lv_obj_set_style_text_color(scene_label, UI_COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(scene_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_pad_bottom(scene_label, UI_GAP_MEDIUM, 0);
    
    // Scene button grid
    lv_obj_t* scene_grid = lv_obj_create(controls_container);
    lv_obj_set_size(scene_grid, lv_pct(100), LV_SIZE_CONTENT);
    lv_obj_set_style_bg_opa(scene_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_style_border_width(scene_grid, 0, 0);
    lv_obj_set_style_pad_all(scene_grid, 0, 0);
    lv_obj_set_layout(scene_grid, LV_LAYOUT_GRID);
    
    static lv_coord_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static lv_coord_t row_dsc[] = {LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_CONTENT, LV_GRID_TEMPLATE_LAST};
    lv_obj_set_grid_dsc_array(scene_grid, col_dsc, row_dsc);
    lv_obj_set_style_pad_column(scene_grid, UI_GAP_SMALL, 0);
    lv_obj_set_style_pad_row(scene_grid, UI_GAP_SMALL, 0);
    
    // Create scene buttons
    static uint32_t scene_indices[6];
    for (uint8_t i = 0; i < UI_SCENES_COUNT; i++) {
        scene_indices[i] = i;
        scene_buttons[i] = lv_btn_create(scene_grid);
        lv_obj_set_grid_cell(scene_buttons[i], LV_GRID_ALIGN_STRETCH, i % 2, 1,
                             LV_GRID_ALIGN_STRETCH, i / 2, 1);
        lv_obj_set_style_radius(scene_buttons[i], UI_RADIUS_SMALL, 0);
        lv_obj_set_style_bg_color(scene_buttons[i], 
                                  (i == 0) ? UI_COLOR_CYAN_600 : UI_COLOR_BG_INPUT, 0);
        lv_obj_set_style_bg_color(scene_buttons[i], UI_COLOR_BG_HOVER, LV_STATE_PRESSED);
        lv_obj_set_style_border_width(scene_buttons[i], 0, 0);
        lv_obj_set_height(scene_buttons[i], 32);
        lv_obj_add_event_cb(scene_buttons[i], scene_btn_cb, LV_EVENT_CLICKED, &scene_indices[i]);
        
        lv_obj_t* btn_label = lv_label_create(scene_buttons[i]);
        lv_label_set_text(btn_label, UI_SCENES[i]);
        lv_obj_set_style_text_color(btn_label, 
                                    (i == 0) ? UI_COLOR_WHITE : UI_COLOR_TEXT_PRIMARY, 0);
        lv_obj_set_style_text_font(btn_label, &lv_font_montserrat_12, 0);
        lv_obj_center(btn_label);
    }
    
    // Function Selection Section
    lv_obj_t* function_label = lv_label_create(controls_container);
    lv_label_set_text(function_label, "功能发送 (FUNCTION)");
    lv_obj_set_style_text_color(function_label, UI_COLOR_TEXT_SECONDARY, 0);
    lv_obj_set_style_text_font(function_label, &lv_font_montserrat_12, 0);
    lv_obj_set_style_pad_bottom(function_label, UI_GAP_SMALL, 0);
    lv_obj_set_style_pad_top(function_label, UI_GAP_MEDIUM, 0);
    
    // Category dropdown
    category_dropdown = lv_dropdown_create(controls_container);
    lv_obj_set_width(category_dropdown, lv_pct(100));
    lv_dropdown_set_options(category_dropdown, "显示 (Display)\n声音 (Sound)\n检查 (Inspection)");
    lv_obj_set_style_bg_color(category_dropdown, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_border_color(category_dropdown, UI_COLOR_BORDER_LIGHT, 0);
    lv_obj_set_style_text_color(category_dropdown, UI_COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(category_dropdown, &lv_font_montserrat_12, 0);
    lv_obj_set_style_pad_ver(category_dropdown, 6, 0);
    lv_obj_add_event_cb(category_dropdown, category_dd_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Function dropdown
    function_dropdown = lv_dropdown_create(controls_container);
    lv_obj_set_width(function_dropdown, lv_pct(100));
    lv_dropdown_set_options(function_dropdown, "启动发动机\n油门控制\n刹车控制");
    lv_obj_set_style_bg_color(function_dropdown, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_border_color(function_dropdown, UI_COLOR_BORDER_LIGHT, 0);
    lv_obj_set_style_text_color(function_dropdown, UI_COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(function_dropdown, &lv_font_montserrat_12, 0);
    lv_obj_set_style_pad_ver(function_dropdown, 6, 0);
    lv_obj_set_style_pad_top(function_dropdown, UI_GAP_MEDIUM, 0);
    lv_obj_add_event_cb(function_dropdown, function_dd_cb, LV_EVENT_VALUE_CHANGED, NULL);
    
    // Manual input button
    manual_btn = lv_btn_create(controls_container);
    lv_obj_set_width(manual_btn, lv_pct(100));
    lv_obj_set_height(manual_btn, 36);
    lv_obj_set_style_bg_color(manual_btn, UI_COLOR_BG_INPUT, 0);
    lv_obj_set_style_bg_color(manual_btn, UI_COLOR_BG_HOVER, LV_STATE_PRESSED);
    lv_obj_set_style_border_width(manual_btn, 0, 0);
    lv_obj_set_style_radius(manual_btn, UI_RADIUS_SMALL, 0);
    lv_obj_set_style_pad_top(manual_btn, 8, 0);
    lv_obj_add_event_cb(manual_btn, manual_btn_cb, LV_EVENT_CLICKED, NULL);
    
    lv_obj_t* manual_label = lv_label_create(manual_btn);
    lv_label_set_text(manual_label, "手动输入");
    lv_obj_set_style_text_color(manual_label, UI_COLOR_TEXT_PRIMARY, 0);
    lv_obj_set_style_text_font(manual_label, &lv_font_montserrat_12, 0);
    lv_obj_center(manual_label);
    
    return controls_container;
}
