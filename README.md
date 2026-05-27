# 轮腿机器人整机研发仓库

本仓库用于统一管理轮腿机器人整机研发资料，包括 STM32 电机控制与 FOC 固件、电子硬件、机械结构、ROS2 系统代码、标定与分析工具、自动化测试、实机实验记录与结果。

## 当前状态

- Git 初始化状态：已初始化，默认分支为 `main`。
- 当前首次提交：`87ecccb chore: initialize wheel-leg robot repository structure`。
- 云端第一状态范围：已在本地纳入当前正式资料、源码、工程文件、CAD、硬件资料和机器人描述包；已排除编译输出、缓存、临时文件、备份文件和个人配置。
- 远端策略：个人仓库使用 `origin`，合作仓库使用 `collab`；远端 URL 待确认。
- 云端推送状态：待提供远端 URL 后推送。
- STM32 工程：已存在于 `firmware/Program_wheel_leg_dog/Program-wheel-leg-dog/`，包含 CubeMX/Keil 工程文件、`Core/`、`Drivers/`、`MDK-ARM/`。
- STM32 编译产物：已在 `MDK-ARM/Program-wheel-leg-dog/` 下发现 `.o`、`.d`、`.crf`、`.axf`、`.hex`、`.map` 等文件，后续应由 `.gitignore` 排除。
- ROS2 工作区：`ros2_ws/` 当前为空。
- ROS2 描述包：当前在 `mechanical/robot1_description/` 下发现 `package.xml`、`setup.py`、`launch/`、`urdf/`、`meshes/`。
- 硬件资料：`hardware/` 已包含器件库、芯片数据手册、驱动板工程与生产相关资料。
- 机械资料：`mechanical/` 已包含 STEP/STP/IPT 文件、STL 网格和 ROS2 描述包。
- 硬件版本、机械版本、固件基线、ROS2 基线、整机参数：待确认。

## 目录导航

| 目录 | 用途 |
| --- | --- |
| `docs/` | 项目范围、架构、路线图、通信协议、版本兼容和系统基线文档 |
| `experiments/` | 实机实验记录、测试结果和复现实验模板 |
| `firmware/` | STM32 电机控制、FOC、板级驱动和嵌入式工程 |
| `hardware/` | 原理图、PCB、器件库、驱动板资料、接线、BOM 和数据手册 |
| `mechanical/` | CAD、结构件、装配资料、导出的网格和机器人描述资料 |
| `ros2_ws/` | ROS2 工作区，未来建议按 `src/` 组织功能包 |
| `tests/` | 自动化测试、硬件在环测试和回归测试入口 |
| `tools/` | 标定、数据分析、脚本工具和辅助程序 |

## 文档索引

- [文档目录说明](docs/README.md)
- [项目范围](docs/PROJECT_SCOPE.md)
- [系统架构](docs/ARCHITECTURE.md)
- [路线图](docs/ROADMAP.md)
- [通信协议](docs/COMMUNICATION_PROTOCOL.md)
- [版本兼容矩阵](docs/VERSION_COMPATIBILITY.md)
- [系统基线](docs/SYSTEM_BASELINE.md)
- [实验记录模板](experiments/EXPERIMENT_TEMPLATE.md)

## 下一步工作

1. 提供个人仓库和合作仓库 URL，用于配置 `origin` 与 `collab`。
2. 推送 `main` 到个人仓库和合作仓库。
3. 在云端仓库检查 LFS 文件、README 显示和分支保护设置。
4. 确认 STM32 工程中备份文件、用户配置文件和编译产物的长期保留策略。
5. 确认 `mechanical/robot1_description/` 是否未来迁入 `ros2_ws/src/`。
6. 补充硬件版本、机械版本、固件 commit、ROS2 commit 和整机系统基线。
7. 建立实验记录流程，并用 `experiments/EXPERIMENT_TEMPLATE.md` 记录每次实机测试。

## 协作约束

- 不假设硬件参数，未知信息写“待确认”。
- 不擅自移动旧工程、机械资料或硬件资料。
- 不提交编译产物、缓存、日志、私密配置和临时文件。
- 对固件源码、硬件设计源文件和机械 CAD 的修改应先确认目标与影响范围。
