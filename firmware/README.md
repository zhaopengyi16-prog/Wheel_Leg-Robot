# 固件目录

本目录用于保存 STM32 电机控制、FOC、板级驱动和相关嵌入式工程。

## 当前状态

- 已发现 STM32 工程：`Program_wheel_leg_dog/Program-wheel-leg-dog/`
- CubeMX 配置文件：`Program_wheel_leg_dog/Program-wheel-leg-dog/Program-wheel-leg-dog.ioc`
- CubeMX 工程标记：`Program_wheel_leg_dog/Program-wheel-leg-dog/.mxproject`
- 源码目录：`Program_wheel_leg_dog/Program-wheel-leg-dog/Core/`
- 驱动库目录：`Program_wheel_leg_dog/Program-wheel-leg-dog/Drivers/`
- Keil/MDK 工程目录：`Program_wheel_leg_dog/Program-wheel-leg-dog/MDK-ARM/`

## 编译产物注意事项

当前已在 `MDK-ARM/Program-wheel-leg-dog/` 下发现 `.o`、`.d`、`.crf`、`.axf`、`.hex`、`.map`、`.htm`、`.lnp`、`.dep` 等编译产物。这些文件不应作为源码长期纳入版本管理。

## 待确认

- 固件目标芯片和完整硬件参数：待确认。
- Keil/MDK 版本：待确认。
- STM32CubeMX 版本：待确认。
- 固件构建流程：待确认。
- 固件发布产物保存策略：待确认。
- `Core/*/Backup/*.bak` 是否保留：待确认。
