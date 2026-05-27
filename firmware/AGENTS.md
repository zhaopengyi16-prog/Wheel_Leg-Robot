# 固件目录 Codex 规则

- 不要擅自修改 `Core/`、`Drivers/`、`.ioc`、Keil 工程文件或启动文件。
- 不要清理、移动、复制或重命名旧 STM32 工程。
- 不要把 Keil/MDK 编译产物当作源码处理。
- 不要假设芯片外设配置、电机参数、FOC 参数、CAN ID 或控制周期。
- 修改固件前必须先说明目标、影响范围和验证方式。
- 读取固件时优先查看 `.ioc`、`Core/Inc/`、`Core/Src/`、`MDK-ARM/*.uvprojx`。
