# AGENTS.md

## 适用范围

本文件适用于 `D:\smart_car\province` 下的全部内容。后续 demo 改进默认以 `5.Demo` 为主要工作区。

## 项目概览

这是一个GD32F303RCT6（Cortex-M4, 256KB Flash, 48KB RAM）电磁循迹智能车 Keil MDK demo 集合。


- `5.Demo\`: 当前可直接改动和编译的 demo 集合。
- `2026基础车分享资料\`: 原始资料、文档、视频、工具包和一份完整资料拷贝，优先作为参考资料，不主动改动。

`5.Demo` 下每个 demo 基本采用相同结构：

- `CODE\`: 小车业务模块和传感器/执行器封装，例如 `car_init.c`、`car_motion.c`、`car_adc.c`、`encoder.c`、`xunxian.c`、`track_demo.c`。
- `USER\src\`: MCU 用户入口、启动相关源文件，重点是 `main.c` 和 `gd32f30x_it.c`。
- `USER\inc\`: 用户头文件，重点是 `main.h` 和中断头文件。
- `MDK\`: Keil uVision 工程文件，重点是 `Project.uvprojx`。
- `MDK\output\`、`MDK\list\`: 编译产物和列表文件，除非排查构建问题，不要手动编辑。
-  所有都使用UTF-8编码

公共库位于：

- `5.Demo\Libraries\hefei_libraries\`: GD32 外设封装，公共入口头文件为 `headfile.h`。
- `5.Demo\Libraries\hefei_peripheral\`: OLED、WS2812B、超声波、电机、TOF 等外设模块。
- `5.Demo\Libraries\Device\`: CMSIS 与 GD32F30x 标准外设库。

## 修改约定

- 优先在当前 demo 内做小范围改动；不要为了单个 demo 随意修改 `5.Demo\Libraries` 公共库（除非出现重大错误，如引脚错误---2026年引脚在"D:\smart_car\province\2026基础车分享资料\基础智能车（电磁）资料V2.3\3.学习文档"有改动记录）。
- 新增业务逻辑优先放在当前 demo 的 `CODE\*.c/.h` 中，入口调用放在 `USER\src\main.c`。
- 需要跨文件共享的宏、类型、函数声明放入对应模块头文件；只有确实需要全局公共可见时，再考虑更新 `Libraries\hefei_libraries\headfile.h`。
- 新增 `.c/.h` 后，必须同步加入当前 demo 的 `MDK\Project.uvprojx`，至少确认 `GD32F30X_HD` 目标包含新文件。
- 不要手动编辑 `MDK\output`、`MDK\list`、`.uvguix.*` 等个人配置或构建产物。
- `03-Car_uart_interrupt` 的 `CODE\car_init.c` 与其它工程不同，涉及中断时不要直接从其它 demo 覆盖。
- 保持现有路径关系和 include 顺序。当前工程 include path 通常先包含 `..\CODE`，再包含 `..\USER\inc`、`..\USER\src`、公共库路径。

## C 代码风格

- 延续现有 C 风格：函数名多为小写下划线或模块前缀，硬件封装函数保持已有命名。
- 优先使用 `stdint.h` 中的固定宽度类型；涉及中断或主循环共享状态时使用 `volatile`。
- 控制循环中避免长时间阻塞；新增 `delay_1ms()` 前确认不会破坏巡线、电机或串口响应。
- PWM、ADC、PID 输出必须做边界限制。电机 PWM 上限参考 `MOTOR_MAX_SPEED`，低速死区参考现有 `TRACK_PWM_DEADBAND`。
- 修改电机方向、ADC 通道、按键含义、OLED 坐标或外设初始化顺序时，在代码注释或提交说明中写清硬件假设。
- 尽量保持中文注释可读；编辑已有中文文件时注意不要破坏原文件编码和换行。
- 不使用中文拼音命名

## 构建与验证

推荐使用 Keil MDK/uVision5：

1. 打开 `5.Demo\<demo>\MDK\Project.uvprojx`。
2. 选择目标 `GD32F30X_HD`。
3. 执行 Rebuild。
4. 确认构建日志中为 `0 Error(s), 0 Warning(s)`。
5. 确认生成或更新 `MDK\output\Project.hex`。
6. target的ARM Compiler使用V6版本的keil以确保编译无错
如果本机 `UV4.exe` 已加入 PATH，也可以在对应 `MDK` 目录尝试命令行构建：

```powershell
UV4.exe -j0 -b Project.uvprojx -t GD32F30X_HD -o build.log
```

硬件验证建议：

- 第一次上车前先架空车轮，确认左右轮方向、停止逻辑和按键含义正确。
- 观察 OLED 上 ADC、误差、左右 PWM 和状态显示，确认传感器接线与通道顺序一致。
- 下载前确认目标芯片、启动文件和实际板卡一致；当前 demo 工程中常见目标为 `GD32F30X_HD`，设备配置指向 GD32F303/GD32F30x 系列。

## Agent 工作流程

- 删除`track_demo.c`
- 开始改动前先确认用户指定的 demo 目录；
- 先阅读 `USER\src\main.c`、相关 `CODE\*.c/.h`、`car_init.c` 和 `MDK\Project.uvprojx`，再动手。
- 修改前后都避免大范围格式化，减少对示例工程的噪声改动。
- 改完后汇报改动文件、验证方式、是否已完成 Keil 构建；如果无法在当前环境构建，要明确说明。
- 发现构建产物、用户配置或参考资料中有无关变化时，不要回滚，除非用户明确要求。
- 改完的地方要注释说明原因和改动
