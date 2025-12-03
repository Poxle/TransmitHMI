# 项目移植说明

## 概述
- 将 `React/Vite` 的前端界面与交互逻辑移植到 `LVGL v9` 的 C 实现。
- 对应的核心界面为 `SignalPanel`，原始代码见 `src/components/signal-panel.tsx:1`。

## 组件映射
- 顶部指示与连接开关 → `lv_label` + `lv_switch`
- 日志窗口滚动显示 → `lv_list`
- 场景按钮网格 → 多个 `lv_button` + `lv_label`
- 类别与功能选择 → `lv_dropdown`
- 手动模式输入 → `lv_textarea`（ID、DATA、周期）、`lv_switch`（周期发送）
- 发送、停止按钮 → `lv_button`
- 状态指示（就绪/发送中/重复） → 可使用 `lv_led` 或按钮样式指示，本实现以按钮状态文本为主

## 逻辑分层
- 业务逻辑在 `lvgl_port/include/logic.h` 与 `lvgl_port/src/logic.c`，纯 C，不依赖 LVGL，便于测试与复用。
- UI 层在 `lvgl_port/include/ui_signal_panel.h` 与 `lvgl_port/src/ui_signal_panel.c`，负责创建 LVGL 组件并绑定事件，将用户操作委托给逻辑层。
- 定时器：使用 `lv_timer` 模拟原前端的 `setInterval/setTimeout`，在 UI 层调度，逻辑层只做状态与消息格式处理。

## 构建与运行
- 推荐工具链：LVGL 官方 PC 仿真（SDL2）或目标设备端口（如 ESP-IDF）。
- 提供 `CMakeLists.txt`，默认生成逻辑层测试用例；如设置环境变量 `LVGL_DIR` 指向 LVGL 源码目录，则额外生成 `app` 目标。
- 典型步骤：
  - Windows/PC 仿真：安装 `CMake` 与 `SDL2`，配置 `LVGL_DIR`，生成并构建 `app`。
  - ESP-IDF：将 `lvgl_port/src` 与 `include` 集成到 `main`，并配置显示与输入驱动。

## 内存与性能
- 日志采用固定容量环形缓冲（最大 256 条），避免无限增长。
- 避免动态分配，字符串以 `snprintf` 写入固定缓冲。
- 定时器按需创建与删除，停止时立即取消重复定时器与延时定时器。
- 界面更新仅在状态变更时刷新，减少重绘。

## 功能一致性
- 自动模式：类别/功能选择，重复功能按原间隔发送并回显 RX，非重复功能一次发送并 1s 回显。
- 手动模式：ID/DATA 输入，支持周期与一次性发送，回显 ACK。
- 连接开关：仅在连接状态下记录场景与传输日志。
- 停止：取消重复并记录 “停止发送”。

## 文件结构
- `lvgl_port/CMakeLists.txt`
- `lvgl_port/include/logic.h`
- `lvgl_port/src/logic.c`
- `lvgl_port/include/ui_signal_panel.h`
- `lvgl_port/src/ui_signal_panel.c`
- `lvgl_port/src/main.c`
- `lvgl_port/tests/test_logic.c`

## 后续集成
- 若需要硬件 CAN 或更复杂协议，可在逻辑层增加接口，并在 UI 回调中接入实际驱动。
- 可用 `lv_led` 与样式动画增强“发送中/重复”视觉反馈。

