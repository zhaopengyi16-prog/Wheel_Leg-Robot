# 系统基线

本文件用于记录某一时间点整机系统的可复现实物与软件状态。

## 基线信息

- 基线编号：cloud-baseline-20260527
- 建立日期：2026-05-27
- 记录人：待确认
- 目标用途：作为个人仓库和合作仓库的云端第一状态

## 云端第一状态范围

- 纳入：当前正式资料、源码、工程文件、CAD、硬件资料和机器人描述包。
- 排除：编译输出、缓存、临时文件、备份文件和个人配置。
- 个人仓库 remote：`origin`，URL 为 `https://github.com/zhaopengyi16-prog/Wheel_Leg-Robot.git`。
- 合作仓库 remote：`collab`，URL 为 `https://github.com/Kita-OvO/Wheeled_Robot_Dog.git`。
- 本地导入状态：已完成。
- 云端推送状态：准备推送。

## 本地导入提交

- 规则与 LFS：`0be2db9 chore: refine ignore and lfs rules for full project sync`
- 通信与参考文档：`34c2350 docs: import communication and reference documents`
- STM32 固件工程：`7b81b37 firmware: import stm32 wheel-leg dog project baseline`
- 硬件设计资料：`9c0313a hardware: import drive board design baseline`
- 机械 CAD 与机器人描述：`4ef80d6 mechanical: import cad and robot description baseline`

## 硬件基线

- 驱动板版本：待确认
- 主控/上位机硬件：待确认
- 电机型号：待确认
- 编码器/传感器：待确认
- 接线版本：待确认
- BOM 版本：待确认

## 机械基线

- 整机 CAD 版本：待确认
- 装配版本：待确认
- 关键结构件版本：待确认
- STL/URDF 导出版本：待确认

## 固件基线

- 固件工程路径：`firmware/Program_wheel_leg_dog/Program-wheel-leg-dog/`
- 固件导入 commit：`7b81b37`
- 固件版本：待确认
- 工具链版本：待确认
- CubeMX `.ioc` 文件：`Program-wheel-leg-dog.ioc`
- Keil 工程文件：`MDK-ARM/Program-wheel-leg-dog.uvprojx`

## ROS2 基线

- ROS2 工作区路径：`ros2_ws/`
- ROS2 commit：待确认
- ROS2 发行版：待确认
- 描述包路径：当前发现于 `mechanical/robot1_description/`

## 实验状态

- 最近一次实验编号：待确认
- 是否完成上电检查：待确认
- 是否完成通信检查：待确认
- 是否完成电机开环测试：待确认
- 是否完成闭环控制测试：待确认
- 是否完成整机运动测试：待确认

## 风险与限制

- 安全措施：待确认
- 供电限制：待确认
- 电流限制：待确认
- 已知问题：待确认
