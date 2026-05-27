# 机械目录

本目录用于保存机械结构、CAD、加工、装配、导出网格和机器人描述相关资料。

## 当前结构

- 根目录下已存在 STEP/STP 文件。
- `驱动固定板-fix1/` 下存在 IPT 文件。
- `robot1_description/` 下存在 ROS2 Python 包结构，包含 `package.xml`、`setup.py`、`launch/`、`urdf/`、`meshes/` 等。

## 当前状态

- 机械版本：待确认。
- 装配基线：待确认。
- CAD 源文件版本：待确认。
- STL/URDF 导出关系：待确认。
- `robot1_description/` 是否迁入 `ros2_ws/src/`：待确认。

## 管理建议

- STEP/STP/STL/IPT 等大型机械文件应由 Git LFS 管理。
- CAD 与导出网格之间应记录导出时间、来源版本和单位。
- 机器人描述包迁移前应确认 ROS2 工作区组织方式。
