# ROS2 目录 Codex 规则

- 不要擅自创建、迁移或重命名 ROS2 包。
- 不要提交 `build/`、`install/`、`log/`。
- 不要假设 ROS2 发行版、消息接口、控制框架或启动流程。
- 修改 ROS2 代码前必须确认目标包、运行方式和验证命令。
- 如果需要引用 `mechanical/robot1_description/`，先说明它当前不在 `ros2_ws/src/` 下。
