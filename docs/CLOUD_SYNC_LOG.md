# 云端同步记录

本文件记录整机项目“云端第一状态”的推送范围、远端仓库和推送结果。

## 第一状态定义

- 基线编号：`cloud-baseline-20260527`
- 本地分支：`main`
- 推送前 HEAD：`1ea6732d3cbdd5bfba455bffc97a30201b5cf18b`
- 已跟踪文件数：`1336`
- Git LFS 文件数：`58`
- 纳入范围：当前正式资料、源码、工程文件、CAD、硬件资料和机器人描述包。
- 排除范围：编译输出、缓存、临时文件、备份文件和个人配置。

## 远端仓库

| remote | 用途 | URL |
| --- | --- | --- |
| `origin` | 个人仓库 | `https://github.com/zhaopengyi16-prog/Wheel_Leg-Robot.git` |
| `collab` | 合作仓库 | `https://github.com/Kita-OvO/Wheeled_Robot_Dog.git` |

## 推送前检查

- 工作区状态：待推送前确认。
- LFS 状态：已启用，CAD、硬件、PDF、Office、ZIP、`.a/.lib` 等大文件通过 LFS 管理。
- 嵌套 Git 仓库：未发现旧工程内部嵌套 `.git/`。
- 风险项：云端仓库权限、LFS 配额、网络认证待推送时确认。

## 推送记录

| 时间 | 操作 | 目标 | 结果 | 备注 |
| --- | --- | --- | --- | --- |
| 2026-05-27 11:09:02 +08:00 | 准备推送 | `origin`、`collab` | 待执行 | 已收到两个仓库 URL |
