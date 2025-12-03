# CAN Signal Sender Panel - LVGL v9 UI

## Overview

This is a complete LVGL v9 implementation of the CAN Signal Sender Panel, converted from the React frontend. It maintains identical visual appearance and functionality while being optimized for ESP32-S3 embedded systems.

**Display Specifications:**
- Resolution: 172×640 pixels (portrait orientation)
- Color depth: RGB565 (16-bit)
- Target platform: ESP32-S3 with LVGL v9

## Project Structure

```
lvgl_ui/
├── ui_main.c/.h              # Main UI initialization
├── ui_header.c               # Header with connection toggle
├── ui_log_display.c          # Log display area
├── ui_controls.c             # Auto mode controls
├── ui_manual_input.c         # Manual input mode
├── ui_footer.c               # Footer with status & buttons
├── ui_state.c/.h             # State management
├── ui_binding.c/.h           # Data binding layer
├── ui_config.c/.h            # Configuration constants
├── globals.xml               # Global configuration
├── project.xml               # Project metadata
└── README.md                 # This file
```

## Quick Start

### 1. Integration into ESP32-S3 Project

```c
#include "lvgl.h"
#include "ui_main.h"
#include "ui_binding.h"

void app_main(void) {
    // Initialize LVGL (done by your display driver)
    lv_init();
    
    // Initialize display and input drivers
    // ... your driver init code ...
    
    // Initialize UI
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
    
    // Main loop
    while(1) {
        lv_timer_handler();
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}
```

### 2. Backend Callback Implementation

```c
// Example: Handle connection change
void backend_connection_handler(bool connected) {
    if (connected) {
        // Initialize CAN bus
        can_init();
        ESP_LOGI(TAG, "CAN bus connected");
    } else {
        // Deinitialize CAN bus
        can_deinit();
        ESP_LOGI(TAG, "CAN bus disconnected");
    }
}

// Example: Handle auto mode transmission
void backend_transmit_auto_handler(const char* scene, uint8_t category, 
                                   uint8_t function, bool repeat, uint32_t interval) {
    // Get function name
    const char* func_name = ui_config_get_function_name(category, function);
    
    // Build CAN message based on scene and function
    can_message_t msg = build_can_message(scene, category, function);
    
    if (repeat) {
        // Start repeating transmission
        start_periodic_transmission(&msg, interval);
    } else {
        // Send single message
        can_transmit(&msg);
        
        // Update UI after transmission
        ui_binding_update_transmission_status(false, false);
    }
    
    // Add log entry
    char log_msg[128];
    snprintf(log_msg, sizeof(log_msg), "%s - %s", scene, func_name);
    ui_binding_add_log("TX", log_msg);
}

// Example: Handle stop
void backend_stop_handler(void) {
    stop_periodic_transmission();
    ui_binding_update_transmission_status(false, false);
    ui_binding_add_log("TX", "停止发送");
}
```

### 3. Updating UI from Backend

```c
// Add a log message
ui_binding_add_log("RX", "CAN ID: 0x123 | Data: [0x01, 0x02, 0x03]");

// Update transmission status
ui_binding_update_transmission_status(true, false); // transmitting, not repeating

// Update connection status
ui_binding_update_connection_status(true);
```

## Data Binding Architecture

The UI uses a callback-based data binding system with clear separation between UI and backend logic.

### UI → Backend Flow

1. User interacts with UI (button click, switch toggle, etc.)
2. UI component calls `ui_binding_trigger_*()` function
3. Binding layer invokes registered callback
4. Backend handles the event

### Backend → UI Flow

1. Backend processes CAN messages or events
2. Backend calls `ui_binding_add_log()` or `ui_binding_update_*()` functions
3. Binding layer updates corresponding UI components
4. UI reflects the changes

### Callback Interface

All callbacks are defined in `ui_binding.h`:

```c
typedef struct {
    connection_callback_t on_connection_changed;
    transmit_auto_callback_t on_transmit_auto;
    transmit_manual_callback_t on_transmit_manual;
    stop_callback_t on_stop;
    scene_callback_t on_scene_selected;
    clear_logs_callback_t on_clear_logs;
} ui_callbacks_t;
```

**Callback Signatures:**

- `void on_connection_changed(bool connected)`
- `void on_transmit_auto(const char* scene, uint8_t category, uint8_t function, bool repeat, uint32_t interval)`
- `void on_transmit_manual(const char* can_id, const char* data, bool repeat, uint32_t interval)`
- `void on_stop(void)`
- `void on_scene_selected(const char* scene)`
- `void on_clear_logs(void)`

## State Management

The UI maintains centralized state in `ui_state.c`:

```c
typedef struct {
    bool is_connected;
    bool is_transmitting;
    bool is_repeating;
    char selected_scene[8];
    ui_category_t selected_category;
    uint8_t selected_function;
    ui_view_mode_t view_mode;
    char manual_id[32];
    char manual_data[128];
    bool manual_repeat;
    uint32_t manual_interval;
    uint16_t log_count;
} ui_state_t;
```

Access state via `ui_state_get()`:

```c
ui_state_t* state = ui_state_get();
if (state->is_connected) {
    // Do something
}
```

## Configuration

### Colors (RGB565)

All colors are defined in `ui_config.h` and `globals.xml`:

| Name | RGB565 | Hex Color | Usage |
|------|--------|-----------|-------|
| UI_COLOR_BG_MAIN | 0x0841 | #0C0E14 | Main background |
| UI_COLOR_BG_CONTAINER | 0x1082 | #111827 | Containers |
| UI_COLOR_BG_INPUT | 0x2124 | #1F2937 | Inputs/buttons |
| UI_COLOR_CYAN_600 | 0x0459 | #0891B2 | Primary buttons |
| UI_COLOR_CYAN_400 | 0x1677 | #22D3EE | Icons/highlights |
| UI_COLOR_GREEN_400 | 0x3DE8 | #4ADE80 | Success/RX |
| UI_COLOR_RED_600 | 0xE123 | #DC2626 | Danger/stop |

### Scenes

Six predefined scenes: **B, BA, IGP, IGR, ST, ACC**

### Function Categories

1. **显示 (Display)**
   - 启动发动机 (Start Engine)
   - 油门控制 (Throttle Control) - *Repeating: 1500ms*
   - 刹车控制 (Brake Control)

2. **声音 (Sound)**
   - 开启车灯 (Turn On Lights)
   - 解锁车门 (Unlock Doors)
   - 调节座椅 (Adjust Seat) - *Repeating: 2000ms*

3. **检查 (Inspection)**
   - 激活ABS (Activate ABS)
   - 气囊检测 (Airbag Check) - *Repeating: 3000ms*
   - 胎压监测 (Tire Pressure)

### Repeating Functions

Use `ui_config_is_repeating_function()` to check:

```c
uint32_t interval = 0;
bool is_repeating = ui_config_is_repeating_function(category, function, &interval);
if (is_repeating) {
    printf("This function repeats every %d ms\n", interval);
}
```

## Memory Considerations

### RAM Usage Estimate

- **LVGL Objects**: ~8KB (screens, containers, widgets)
- **State Data**: ~300 bytes
- **Log Buffer**: Depends on log length (textarea auto-manages)
- **Display Buffer**: 10752 bytes (172 * 640 / 10 for double buffering)

**Total**: ~20KB + log buffer

### PSRAM Recommendation

For smooth scrolling and larger log buffers, PSRAM is recommended but not required for basic operation.

## Build Configuration

### menuconfig Settings

```
Component config → LVGL Configuration
├── Color depth: 16 bit (RGB565)
├── Horizontal resolution: 172
├── Vertical resolution: 640
└── Enable PSRAM: Yes (recommended)
```

### CMakeLists.txt

```cmake
idf_component_register(
    SRCS 
        "lvgl_ui/ui_main.c"
        "lvgl_ui/ui_header.c"
        "lvgl_ui/ui_log_display.c"
        "lvgl_ui/ui_controls.c"
        "lvgl_ui/ui_manual_input.c"
        "lvgl_ui/ui_footer.c"
        "lvgl_ui/ui_state.c"
        "lvgl_ui/ui_binding.c"
        "lvgl_ui/ui_config.c"
    INCLUDE_DIRS 
        "lvgl_ui"
    REQUIRES 
        lvgl
)
```

## Testing

### LVGL Simulator (PC)

Test UI on PC before deploying to hardware:

```bash
# Clone LVGL simulator
git clone https://github.com/lvgl/lv_port_pc_eclipse.git
cd lv_port_pc_eclipse

# Copy UI files
cp -r ../lvgl_ui .

# Build and run
mkdir build && cd build
cmake ..
make
./bin/main
```

### Hardware Testing

1. Flash to ESP32-S3
2. Connect display (SPI/Parallel)
3. Verify touch input (if available)
4. Test all UI interactions
5. Monitor serial output for logs

## Troubleshooting

### Issue: Display shows garbage

- **Solution**: Check color depth setting (must be RGB565)
- Verify display driver initialization
- Check SPI/Parallel interface pins

### Issue: Touch not responding

- **Solution**: Ensure touch driver is properly initialized
- Calibrate touch coordinates if needed
- Check touch interrupt configuration

### Issue: Text not displaying

- **Solution**: Verify LVGL font files are included
- Check `LV_FONT_MONTSERRAT_*` defines in `lv_conf.h`
- Ensure fonts are enabled in menuconfig

### Issue: Out of memory

- **Solution**: Enable PSRAM in menuconfig
- Reduce log buffer size
- Use smaller display buffer (increase `/10` divisor)

## API Reference

### Initialization

- `lv_obj_t* ui_init(void)` - Initialize UI, returns screen object
- `lv_obj_t* ui_get_screen(void)` - Get main screen object

### Data Binding (Backend → UI)

- `void ui_binding_add_log(const char* type, const char* message)` - Add log entry
- `void ui_binding_update_transmission_status(bool transmitting, bool repeating)` - Update TX status
- `void ui_binding_update_connection_status(bool connected)` - Update connection status

### Data Binding (UI → Backend)

- `void ui_binding_register_callbacks(const ui_callbacks_t* callbacks)` - Register all callbacks
- `void ui_binding_register_connection_callback(connection_callback_t callback)` - Register single callback
- (See `ui_binding.h` for all callback registration functions)

### State Access

- `ui_state_t* ui_state_get(void)` - Get current state
- `void ui_state_set_*()` - Various state setters (see `ui_state.h`)

### Configuration

- `const char* ui_config_get_function_name(uint8_t category, uint8_t function)` - Get function name
- `uint8_t ui_config_get_function_count(uint8_t category)` - Get function count for category
- `bool ui_config_is_repeating_function(uint8_t category, uint8_t function, uint32_t* interval)` - Check if repeating

## License

This UI implementation is derived from the original React frontend project and follows the same license terms.

## Support

For issues or questions:
1. Check this README
2. Review `globals.xml` and `project.xml` for configuration details
3. Examine source code comments
4. Test in LVGL simulator first before hardware deployment
