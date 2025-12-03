/**
 * @file backend_integration_example.c
 * @brief Example Backend Integration
 * 
 * This file demonstrates how to integrate the LVGL UI with a CAN bus backend.
 * Copy and modify this file for your specific CAN implementation.
 */

#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/twai.h" // ESP32 CAN driver

#include "lvgl.h"
#include "ui_main.h"
#include "ui_binding.h"
#include "ui_config.h"

static const char* TAG = "CAN_UI";

// CAN configuration (example)
#define CAN_TX_PIN GPIO_NUM_21
#define CAN_RX_PIN GPIO_NUM_22
#define CAN_BITRATE TWAI_TIMING_CONFIG_500KBITS()

// Periodic transmission timer
static TimerHandle_t periodic_timer = NULL;
static twai_message_t periodic_msg = {0};

// ==================== Backend Callback Implementations ====================

/**
 * @brief Handle connection state change
 */
void backend_connection_handler(bool connected) {
    if (connected) {
        // Initialize CAN bus
        twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_NORMAL);
        twai_timing_config_t t_config = CAN_BITRATE;
        twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();
        
        esp_err_t err = twai_driver_install(&g_config, &t_config, &f_config);
        if (err == ESP_OK) {
            twai_start();
            ESP_LOGI(TAG, "CAN bus started");
            ui_binding_add_log("TX", "CAN 总线已连接");
        } else {
            ESP_LOGE(TAG, "CAN driver install failed: %s", esp_err_to_name(err));
            ui_binding_update_connection_status(false);
            ui_binding_add_log("TX", "CAN 连接失败");
        }
    } else {
        // Stop CAN bus
        if (periodic_timer != NULL) {
            xTimerStop(periodic_timer, 0);
        }
        twai_stop();
        twai_driver_uninstall();
        ESP_LOGI(TAG, "CAN bus stopped");
        ui_binding_add_log("TX", "CAN 总线已断开");
    }
}

/**
 * @brief Build CAN message based on scene and function
 */
static twai_message_t build_can_message_from_function(const char* scene, uint8_t category, uint8_t function) {
    twai_message_t msg = {0};
    
    // Example: Build CAN ID based on scene and category
    // This is just an example - modify based on your CAN protocol
    uint32_t base_id = 0x100;
    
    if (strcmp(scene, "B") == 0) base_id = 0x100;
    else if (strcmp(scene, "BA") == 0) base_id = 0x200;
    else if (strcmp(scene, "IGP") == 0) base_id = 0x300;
    else if (strcmp(scene, "IGR") == 0) base_id = 0x400;
    else if (strcmp(scene, "ST") == 0) base_id = 0x500;
    else if (strcmp(scene, "ACC") == 0) base_id = 0x600;
    
    msg.identifier = base_id + (category << 4) + function;
    msg.data_length_code = 8;
    msg.extd = 0; // Standard frame
    msg.rtr = 0;  // Data frame
    
    // Example data payload
    msg.data[0] = scene[0];
    msg.data[1] = category;
    msg.data[2] = function;
    msg.data[3] = 0x00;
    msg.data[4] = 0x00;
    msg.data[5] = 0x00;
    msg.data[6] = 0x00;
    msg.data[7] = 0x00;
    
    return msg;
}

/**
 * @brief Periodic timer callback for repeating transmission
 */
static void periodic_timer_callback(TimerHandle_t timer) {
    // Send the periodic message
    esp_err_t err = twai_transmit(&periodic_msg, pdMS_TO_TICKS(100));
    
    if (err == ESP_OK) {
        // Log transmission
        char log_msg[64];
        snprintf(log_msg, sizeof(log_msg), "CAN ID: 0x%03X | Data: [...]", periodic_msg.identifier);
        ui_binding_add_log("TX", log_msg);
        
        // Simulate response after 500ms
        vTaskDelay(pdMS_TO_TICKS(500));
        ui_binding_add_log("RX", "ACK: OK");
    }
}

/**
 * @brief Handle auto mode transmission
 */
void backend_transmit_auto_handler(const char* scene, uint8_t category, 
                                   uint8_t function, bool repeat, uint32_t interval) {
    const char* func_name = ui_config_get_function_name(category, function);
    
    // Build CAN message
    twai_message_t msg = build_can_message_from_function(scene, category, function);
    
    // Log the transmission
    char log_msg[128];
    const char* cat_name = "";
    if (category == 0) cat_name = "显示 (Display)";
    else if (category == 1) cat_name = "声音 (Sound)";
    else if (category == 2) cat_name = "检查 (Inspection)";
    
    snprintf(log_msg, sizeof(log_msg), "%s - %s", cat_name, func_name);
    ui_binding_add_log("TX", log_msg);
    
    if (repeat) {
        // Save message for periodic transmission
        periodic_msg = msg;
        
        // Create or restart timer
        if (periodic_timer == NULL) {
            periodic_timer = xTimerCreate("periodic_tx", pdMS_TO_TICKS(interval),
                                         pdTRUE, NULL, periodic_timer_callback);
        } else {
            xTimerChangePeriod(periodic_timer, pdMS_TO_TICKS(interval), 0);
        }
        xTimerStart(periodic_timer, 0);
        
        // Send first message immediately
        twai_transmit(&msg, pdMS_TO_TICKS(100));
        
        // Simulate response
        vTaskDelay(pdMS_TO_TICKS(500));
        ui_binding_add_log("RX", "CAN ID: 0x123 | Data: [0x01, 0x02, 0x03]");
        
    } else {
        // Single transmission
        esp_err_t err = twai_transmit(&msg, pdMS_TO_TICKS(100));
        
        if (err == ESP_OK) {
            // Simulate response after 1 second
            vTaskDelay(pdMS_TO_TICKS(1000));
            ui_binding_add_log("RX", "CAN ID: 0x123 | Data: [0x01, 0x02, 0x03]");
            ui_binding_update_transmission_status(false, false);
        } else {
            ESP_LOGE(TAG, "CAN transmit failed: %s", esp_err_to_name(err));
            ui_binding_add_log("TX", "发送失败");
            ui_binding_update_transmission_status(false, false);
        }
    }
}

/**
 * @brief Parse hex string to uint32_t
 */
static uint32_t parse_hex(const char* str) {
    uint32_t value = 0;
    sscanf(str, "0x%x", &value);
    return value;
}

/**
 * @brief Handle manual mode transmission
 */
void backend_transmit_manual_handler(const char* can_id, const char* data,
                                     bool repeat, uint32_t interval) {
    // Parse CAN ID
    uint32_t id = parse_hex(can_id);
    
    // Build message
    twai_message_t msg = {0};
    msg.identifier = id;
    msg.extd = 0;
    msg.rtr = 0;
    msg.data_length_code = 8;
    
    // For simplicity, just use example data
    // In real implementation, parse the data string
    for (int i = 0; i < 8; i++) {
        msg.data[i] = i + 1;
    }
    
    // Log
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "CAN ID: %s | Data: %s", can_id, data);
    ui_binding_add_log("TX", log_msg);
    
    if (repeat) {
        // Save and start periodic transmission
        periodic_msg = msg;
        
        if (periodic_timer == NULL) {
            periodic_timer = xTimerCreate("periodic_tx", pdMS_TO_TICKS(interval),
                                         pdTRUE, NULL, periodic_timer_callback);
        } else {
            xTimerChangePeriod(periodic_timer, pdMS_TO_TICKS(interval), 0);
        }
        xTimerStart(periodic_timer, 0);
        
        // Send first message
        twai_transmit(&msg, pdMS_TO_TICKS(100));
        
        vTaskDelay(pdMS_TO_TICKS(500));
        ui_binding_add_log("RX", "ACK: OK");
        
    } else {
        // Single transmission
        esp_err_t err = twai_transmit(&msg, pdMS_TO_TICKS(100));
        
        if (err == ESP_OK) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            ui_binding_add_log("RX", "ACK: OK");
            ui_binding_update_transmission_status(false, false);
        } else {
            ui_binding_add_log("TX", "发送失败");
            ui_binding_update_transmission_status(false, false);
        }
    }
}

/**
 * @brief Handle stop request
 */
void backend_stop_handler(void) {
    // Stop periodic transmission
    if (periodic_timer != NULL) {
        xTimerStop(periodic_timer, 0);
    }
    
    ui_binding_update_transmission_status(false, false);
    ui_binding_add_log("TX", "停止发送");
    ESP_LOGI(TAG, "Transmission stopped");
}

/**
 * @brief Handle scene selection (informational)
 */
void backend_scene_handler(const char* scene) {
    ESP_LOGI(TAG, "Scene selected: %s", scene);
    // Optionally log to UI
    // char log_msg[32];
    // snprintf(log_msg, sizeof(log_msg), "场景: %s", scene);
    // ui_binding_add_log("TX", log_msg);
}

/**
 * @brief Handle clear logs
 */
void backend_clear_logs_handler(void) {
    ESP_LOGI(TAG, "Logs cleared");
}

// ==================== Application Entry Point ====================

/**
 * @brief Initialize and run the application
 */
void app_main(void) {
    // Initialize LVGL and display driver
    // (This is platform-specific - add your display driver init here)
    
    lv_init();
    // ... display driver init ...
    // ... input driver init ...
    
    // Initialize UI
    ESP_LOGI(TAG, "Initializing UI...");
    ui_init();
    
    // Register backend callbacks
    ui_callbacks_t callbacks = {
        .on_connection_changed = backend_connection_handler,
        .on_transmit_auto = backend_transmit_auto_handler,
        .on_transmit_manual = backend_transmit_manual_handler,
        .on_stop = backend_stop_handler,
        .on_scene_selected = backend_scene_handler,
        .on_clear_logs = backend_clear_logs_handler
    };
    ui_binding_register_callbacks(&callbacks);
    
    ESP_LOGI(TAG, "UI initialized successfully");
    
    // Main LVGL task loop
    while (1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
