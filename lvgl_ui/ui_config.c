/**
 * @file ui_config.c
 * @brief UI Configuration Implementation
 */

#include "ui_config.h"
#include <string.h>

// ==================== Scene Options ====================
const char* UI_SCENES[] = {
    "B",
    "BA",
    "IGP",
    "IGR",
    "ST",
    "ACC"
};
const uint8_t UI_SCENES_COUNT = sizeof(UI_SCENES) / sizeof(UI_SCENES[0]);

// ==================== Function Categories ====================
const char* UI_CATEGORIES[] = {
    "显示 (Display)",
    "声音 (Sound)",
    "检查 (Inspection)"
};
const uint8_t UI_CATEGORIES_COUNT = sizeof(UI_CATEGORIES) / sizeof(UI_CATEGORIES[0]);

// ==================== Function Options ====================
// Display category functions
const char* UI_FUNCTIONS_DISPLAY[] = {
    "启动发动机",
    "油门控制",
    "刹车控制"
};
const uint8_t UI_FUNCTIONS_DISPLAY_COUNT = sizeof(UI_FUNCTIONS_DISPLAY) / sizeof(UI_FUNCTIONS_DISPLAY[0]);

// Sound category functions
const char* UI_FUNCTIONS_SOUND[] = {
    "开启车灯",
    "解锁车门",
    "调节座椅"
};
const uint8_t UI_FUNCTIONS_SOUND_COUNT = sizeof(UI_FUNCTIONS_SOUND) / sizeof(UI_FUNCTIONS_SOUND[0]);

// Inspection category functions
const char* UI_FUNCTIONS_INSPECTION[] = {
    "激活ABS",
    "气囊检测",
    "胎压监测"
};
const uint8_t UI_FUNCTIONS_INSPECTION_COUNT = sizeof(UI_FUNCTIONS_INSPECTION) / sizeof(UI_FUNCTIONS_INSPECTION[0]);

// ==================== Repeating Function Configuration ====================
// Map of repeating functions: {category, function_index, interval_ms}
typedef struct {
    uint8_t category;
    uint8_t function;
    uint32_t interval;
} repeating_function_config_t;

static const repeating_function_config_t REPEATING_FUNCTIONS[] = {
    {1, 2, 2000},  // 调节座椅 (Sound category, index 2) - 2000ms
    {2, 1, 3000},  // 气囊检测 (Inspection category, index 1) - 3000ms
    {0, 1, 1500},  // 油门控制 (Display category, index 1) - 1500ms
};
static const uint8_t REPEATING_FUNCTIONS_COUNT = sizeof(REPEATING_FUNCTIONS) / sizeof(REPEATING_FUNCTIONS[0]);

bool ui_config_is_repeating_function(uint8_t category, uint8_t function, uint32_t* interval) {
    for (uint8_t i = 0; i < REPEATING_FUNCTIONS_COUNT; i++) {
        if (REPEATING_FUNCTIONS[i].category == category && 
            REPEATING_FUNCTIONS[i].function == function) {
            if (interval != NULL) {
                *interval = REPEATING_FUNCTIONS[i].interval;
            }
            return true;
        }
    }
    return false;
}

const char* ui_config_get_function_name(uint8_t category, uint8_t function) {
    switch (category) {
        case 0: // Display
            if (function < UI_FUNCTIONS_DISPLAY_COUNT) {
                return UI_FUNCTIONS_DISPLAY[function];
            }
            break;
        case 1: // Sound
            if (function < UI_FUNCTIONS_SOUND_COUNT) {
                return UI_FUNCTIONS_SOUND[function];
            }
            break;
        case 2: // Inspection
            if (function < UI_FUNCTIONS_INSPECTION_COUNT) {
                return UI_FUNCTIONS_INSPECTION[function];
            }
            break;
    }
    return "";
}

uint8_t ui_config_get_function_count(uint8_t category) {
    switch (category) {
        case 0: return UI_FUNCTIONS_DISPLAY_COUNT;
        case 1: return UI_FUNCTIONS_SOUND_COUNT;
        case 2: return UI_FUNCTIONS_INSPECTION_COUNT;
        default: return 0;
    }
}
