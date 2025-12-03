cmake_minimum_required(VERSION 3.15)
project(lvgl_signal_panel C)

set(CMAKE_C_STANDARD 11)

add_library(signal_logic src/logic.c)
target_include_directories(signal_logic PUBLIC include)

enable_testing()
add_executable(test_logic tests/test_logic.c)
target_link_libraries(test_logic signal_logic)
add_test(NAME logic_basic COMMAND test_logic)

if(DEFINED ENV{LVGL_DIR})
  add_executable(app src/main.c src/ui_signal_panel.c src/logic.c)
  target_include_directories(app PRIVATE include $ENV{LVGL_DIR})
endif()

