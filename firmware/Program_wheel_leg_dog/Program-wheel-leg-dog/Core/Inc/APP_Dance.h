/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : APP_Dance.h
  * @brief          : 轮足机器狗舞蹈测试程序
  ******************************************************************************
  * @attention
  *
  * 【机器人结构】
  *   四足, 每腿通过连杆机构驱动 Z 轴方向运动.
  *   共 4 个 STS2302 舵机, 前后两块板各控制 2 个.
  *
  * 【连杆坐标约定 (以前板为例, 后板同理)】
  *   Z 轴正方向 = 腿伸出 (向下踩地):
  *     Servo1 (ID 1, 左前): Target_Pos 增大 → 腿伸出
  *     Servo2 (ID 2, 右前): Target_Pos 减小 → 腿伸出  ← 镜像安装
  *   Z 轴负方向 = 腿收起 (向上抬腿):
  *     Servo1: Target_Pos 减小
  *     Servo2: Target_Pos 增大
  *   中立站立位: 两舵机均为 2048
  *
  * 【板载分工】
  *   前板 (NODE_ID_FRONT_DRIVE = 0x01): Servo1→ID 1 (左前), Servo2→ID 2 (右前)
  *   后板 (NODE_ID_REAR_DRIVE  = 0x02): Servo1→ID 3 (左后), Servo2→ID 4 (右后)
  *
  * 【同步说明】
  *   两函数内所有 HAL_Delay 累计时长完全一致, 两板同时上电即可保持节拍同步.
  *
  ******************************************************************************
  */
#ifndef __APP_DANCE_H__
#define __APP_DANCE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* =========================================================================
 * 舞蹈动作入口函数声明
 * ========================================================================= */

/**
 * @brief  前板舞蹈任务 (Servo ID 1 & 2, 左前腿 + 右前腿)
 * @note   在 main() 的 while(1) 中调用, 函数内部包含 while(1) 永久循环.
 */
void Robot_Front_Dance_Task(void);

/**
 * @brief  后板舞蹈任务 (Servo ID 3 & 4, 左后腿 + 右后腿)
 * @note   在 main() 的 while(1) 中调用, 函数内部包含 while(1) 永久循环.
 */
void Robot_Rear_Dance_Task(void);

#ifdef __cplusplus
}
#endif

#endif /* __APP_DANCE_H__ */
