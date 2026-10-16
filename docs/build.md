# 编译与烧录

## 已知开发环境

- MCU：STM32F103C8T6
- IDE：Keil MDK 5
- 编译器：ARM Compiler 5，历史构建日志使用 5.06 update 5
- 固件库：STM32F10x Standard Peripheral Library 3.5.0
- 工程入口：`firmware/stm32/Project.uvprojx`

## 编译

1. 安装 Keil MDK 5。
2. 在 Keil Pack Installer 中安装 STM32F1 Device Family Pack。不要从本仓库下载或提交 Keil 安装包和 Device Pack。
3. 确认 ARM Compiler 5 已安装并可在工程设置中选择。
4. 打开 `firmware/stm32/Project.uvprojx`。
5. 选择 `Target 1`。
6. 执行 Build，检查输出中没有错误。

项目已经包含 CMSIS、启动代码和标准外设库源码，不需要把完整 STM32 教程资料包复制到工程目录。

## 烧录

推荐使用 ST-Link：

1. 连接 SWDIO、SWCLK、GND 和目标板电源。
2. 在 Keil 的 Debug 和 Utilities 设置中选择对应 ST-Link 驱动。
3. 编译后执行 Download。

也可以使用开发板支持的串口 Bootloader 和官方下载工具，但需要按开发板说明设置 BOOT0/BOOT1。

## 上电检查

1. 架空车轮后上电。
2. 数码管应显示 `1`。
3. 按一次模式键，显示切换为 `2`；再按一次切换为 `3`。
4. 在模式 1 检查左右循迹传感器是否能分别改变轮速。
5. 在模式 2 检查左右避障传感器方向是否与代码一致。
6. 在模式 3 验证遥控器五个方向/停止按键。
7. 如需串口调试，使用 9600 波特率、8 数据位、无校验、1 停止位。

