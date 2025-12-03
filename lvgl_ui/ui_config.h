/**
 * @file ui_config.h
 * @brief UI Configuration Constants
 * 
 * Defines all constants including colors, dimensions, fonts, and string arrays.
 * This corresponds to the globals.xml configuration.
 */

#ifndef UI_CONFIG_H
#define UI_CONFIG_H

#include "lvgl.h"

#ifdef __cplusplus
extern "C" {
#endif

// ==================== Dimensions ====================
#define UI_SCREEN_WIDTH  172
#define UI_SCREEN_HEIGHT 640

#define UI_HEADER_HEIGHT 48
#define UI_LOG_HEIGHT    155
#define UI_FOOTER_HEIGHT 80

// ==================== Colors (RGB565) ====================
// Background colors
#define UI_COLOR_BG_MAIN        lv_color_hex(0x0C0E14)  // gray-950
#define UI_COLOR_BG_CONTAINER   lv_color_hex(0x111827)  // gray-900
#define UI_COLOR_BG_INPUT       lv_color_hex(0x1F2937)  // gray-800
#define UI_COLOR_BG_HOVER       lv_color_hex(0x374151)  // gray-700
#define UI_COLOR_BLACK          lv_color_hex(0x000000)

// Border colors
#define UI_COLOR_BORDER_LIGHT   lv_color_hex(0x374151)  // gray-700
#define UI_COLOR_BORDER_MAIN    lv_color_hex(0x1F2937)  // gray-800

// Text colors
#define UI_COLOR_TEXT_PRIMARY   lv_color_hex(0xD1D5DB)  // gray-300
#define UI_COLOR_TEXT_SECONDARY lv_color_hex(0x9CA3AF)  // gray-400
#define UI_COLOR_TEXT_MUTED     lv_color_hex(0x6B7280)  // gray-500
#define UI_COLOR_TEXT_DISABLED  lv_color_hex(0x4B5563)  // gray-600
#define UI_COLOR_WHITE          lv_color_hex(0xFFFFFF)

// Accent colors
#define UI_COLOR_CYAN_600       lv_color_hex(0x0891B2)  // Primary buttons
#define UI_COLOR_CYAN_500       lv_color_hex(0x06B6D4)  // Primary hover
#define UI_COLOR_CYAN_400       lv_color_hex(0x22D3EE)  // Icons/highlights

// Status colors
#define UI_COLOR_GREEN_500      lv_color_hex(0x22C55E)  // Connected
#define UI_COLOR_GREEN_400      lv_color_hex(0x4ADE80)  // RX messages
#define UI_COLOR_RED_600        lv_color_hex(0xDC2626)  // Danger/stop
#define UI_COLOR_RED_500        lv_color_hex(0xEF4444)  // Danger hover

// Disabled state
#define UI_COLOR_DISABLED_BG    lv_color_hex(0x374151)  // gray-700
#define UI_COLOR_DISABLED_TEXT  lv_color_hex(0x6B7280)  // gray-500

// ==================== Font Sizes ====================
#define UI_FONT_SIZE_SMALL      10  // Log timestamps
#define UI_FONT_SIZE_NORMAL     12  // Most text
#define UI_FONT_SIZE_MEDIUM     14  // Headers

// ==================== Spacing ====================
#define UI_PADDING_SMALL        4
#define UI_PADDING_MEDIUM       8
#define UI_PADDING_LARGE        12

#define UI_GAP_SMALL            4
#define UI_GAP_MEDIUM           8

#define UI_RADIUS_SMALL         4
#define UI_RADIUS_MEDIUM        8

// ==================== Scene Options ====================
extern const char* UI_SCENES[];
extern const uint8_t UI_SCENES_COUNT;

// ==================== Function Categories ====================
extern const char* UI_CATEGORIES[];
extern const uint8_t UI_CATEGORIES_COUNT;

// ==================== Function Options ====================
// Display category functions
extern const char* UI_FUNCTIONS_DISPLAY[];
extern const uint8_t UI_FUNCTIONS_DISPLAY_COUNT;

// Sound category functions
extern const char* UI_FUNCTIONS_SOUND[];
extern const uint8_t UI_FUNCTIONS_SOUND_COUNT;

// Inspection category functions
extern const char* UI_FUNCTIONS_INSPECTION[];
extern const uint8_t UI_FUNCTIONS_INSPECTION_COUNT;

// ==================== Repeating Function Configuration ====================
/**
 * @brief Check if a function is a repeating function
 * @param category Category index
 * @param function Function index
 * @param interval Output: interval in ms (if repeating)
 * @return true if repeating, false otherwise
 */
bool ui_config_is_repeating_function(uint8_t category, uint8_t function, uint32_t* interval);

/**
 * @brief Get function name by category and index
 * @param category Category index
 * @param function Function index
 * @return Function name string
 */
const char* ui_config_get_function_name(uint8_t category, uint8_t function);

/**
 * @brief Get function count for a category
 * @param category Category index
 * @return Number of functions in category
 */
uint8_t ui_config_get_function_count(uint8_t category);

#ifdef __cplusplus
}
#endif

#endif // UI_CONFIG_H
