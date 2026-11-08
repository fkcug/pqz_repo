# STM32F103 + FreeRTOS 模板工程

基于 **STM32F103C8T6** 的裸机 / 实时操作系统入门模板，用于测试与对比（test and contrast）多个外设驱动在 FreeRTOS 环境下的运行表现。

移植了标准外设库（SPL）与 FreeRTOS 内核，包含 LED、按键、OLED 三个演示模块，可作为后续项目的起手框架。

---

## 硬件平台

| 项目 | 参数 |
|---|---|
| MCU | STM32F103C8T6（Cortex-M3，中容量） |
| Flash / RAM | 64 KB / 20 KB |
| 主频 | 72 MHz（`configCPU_CLOCK_HZ` 已按 72 MHz 配置） |
| 调试器 | ST-Link / J-Link（Keil MDK 内选择） |

### 引脚分配

| 引脚 | 功能 | 说明 |
|---|---|---|
| PC13 | 板载 LED | 推挽输出，闪烁节奏由 `myTask` 控制 |
| PB8 | OLED SCL | 软件 I2C，开漏输出 |
| PB9 | OLED SDA | 软件 I2C，开漏输出 |

> OLED 为 SSD1306 驱动的 0.96 寸 128×64 屏，I2C 从机地址 `0x78`（7 位地址 `0x3C` 左移一位）。**PB8/PB9 必须依赖模块板载上拉电阻**，裸屏需外接 4.7 kΩ 上拉到 3.3 V，否则总线拉不起来。

---

## 软件组成

| 组件 | 版本 / 说明 |
|---|---|
| 标准外设库 SPL | V3.5.0 |
| FreeRTOS Kernel | V11.3.0 |
| 内存管理 | `heap_4.c`（带碎片合并，推荐） |
| 启动文件 | `startup_stm32f10x_md.s`（中容量） |
| IDE | Keil MDK-ARM 5 |

### FreeRTOS 关键配置

| 宏 | 值 | 含义 |
|---|---|---|
| `configTICK_RATE_HZ` | 1000 | 系统节拍 1 kHz，1 tick = 1 ms |
| `configTOTAL_HEAP_SIZE` | 4096 | 堆 4 KB，`xTaskCreate` 从此分配 |
| `configMAX_PRIORITIES` | 5 | 可用优先级 0 ~ 4 |
| `configMINIMAL_STACK_SIZE` | 128 | 空闲任务栈，单位是**字**（128 字 = 512 字节） |
| `configCHECK_FOR_STACK_OVERFLOW` | 2 | 开启栈溢出检测 |

---

## 目录结构

```
├── Start/          启动文件、CMSIS 内核、system_stm32f10x.c
├── Library/        STM32F10x 标准外设库 V3.5.0
├── System/         延时函数（Delay.c/h）
├── Freertos/
│   ├── inc/        内核头文件
│   ├── src/        内核源码（tasks.c / queue.c / list.c 等）
│   └── port/       移植层（port.c / heap_4.c）
├── Hardware/       板级驱动：LED / Key / OLED
├── User/           main.c、中断服务函数、外设配置头文件
├── Project.uvprojx Keil 工程文件
└── keilkill.bat    清理编译中间文件的批处理
```

---

## 快速开始

1. 用 Keil MDK 打开 `Project.uvprojx`
2. `Project → Options for Target → Debug` 选择你的调试器（ST-Link / J-Link）
3. `F7` 编译，`F8` 下载
4. 上电后：

   - PC13 LED **每 500 ms 翻转一次**（1 Hz 闪烁）
   - OLED 第一行显示 `FreeRtos!`，第二行显示 `System Ready`

编译产物（`Objects/`、`Listings/`）已被 `.gitignore` 排除，克隆后重新编译即可生成。需要彻底清理时可直接运行 `keilkill.bat`。

---

## 已知陷阱

### 1. 任务内禁止使用 `Delay_ms()`

`System/Delay.c` 的毫秒延时是**直接改写 `SysTick->LOAD / VAL / CTRL`** 实现的。调度器启动后 SysTick 归 FreeRTOS 所有，调用 `Delay_ms()` 会破坏 1 ms 心跳，导致所有 `vTaskDelay()` 的节拍错乱甚至系统卡死。

```c
/* 错误 */
Delay_ms(500);

/* 正确：tick = 1 ms，所以 500 tick = 500 ms */
vTaskDelay(500);
```

只有在 `vTaskStartScheduler()` **之前**（此时还没接管 SysTick）才可以安全使用 `Delay_ms()`。

### 2. 主函数里的代码必须在调度器启动前执行完

`vTaskStartScheduler()` 启动后不会返回，它后面的语句全部是死代码：

```c
OLED_Init();
OLED_ShowString(1, 1, "FreeRtos!");   /* 必须放在启动之前 */

xTaskCreate(myTask, "myTask", 128, NULL, 2, &myTaskHandler);
vTaskStartScheduler();                 /* 从此函数不再返回 */
```

如果需要在运行期间刷新屏幕，应把 OLED 操作放进任务里，而不是写在启动之后。

### 3. 栈大小的单位是“字”不是“字节”

`xTaskCreate` 的第三个参数以 **word（4 字节）** 为单位。`128` 意味着 512 字节栈空间，对于带局部数组或调用 `printf` 的任务偏小，容易触发栈溢出。

### 4. I2C 引脚为开漏输出

`OLED.c` 中 SCL/SDA 配置为 `GPIO_Mode_Out_OD`，输出高电平时是**高阻态**，依赖外部上拉。这是软件 I2C 的正确做法，但排查“屏幕无反应”时务必先确认上拉。

---

## 待办

- [ ] 增加按键任务，演示任务间同步（队列 / 信号量）
- [ ] OLED 刷新改为独立任务 + 计数显示
- [ ] 补充串口日志输出（USART1，重定向 `printf`）

---

## 许可

未指定开源许可证。当前仅用于学习、测试与技术验证，如需商用请自行补充许可证声明。
