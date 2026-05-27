/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : BSP_fdcan.h
  * @brief          : Canfd communication agreement
  ******************************************************************************
  * @attention
  *
  *
  *
  *
  ******************************************************************************
  */
#ifndef __BSP_FDCAN_H__    
#define __BSP_FDCAN_H__    

#ifdef __cplusplus
extern "C" {
#endif


/* ========================================= */

#include "main.h"
#include "APP_Data.h"


/* ========================================================================= */
/** @defgroup FDCAN_Protocol_Config FDCAN Distributed Protocol Configuration
  * @brief 轮足机器人底盘分布式通信协议宏定义
  * @{
  */

/** @defgroup FDCAN_Node_IDs 节点设备地址分配
  * @{
  */
#define NODE_ID_HOST        ((uint32_t)0x00U) /*!< 香橙派 (主控设备) */
#define NODE_ID_FRONT_DRIVE ((uint32_t)0x01U) /*!< 前驱动板 (左侧/右侧均可根据需求配置) */
#define NODE_ID_REAR_DRIVE  ((uint32_t)0x02U) /*!< 后驱动板 */
#define NODE_ID_BROADCAST   ((uint32_t)0xFFU) /*!< 全局广播地址 */
/**
  * @}
  */
/** @defgroup FDCAN_MY_Node_ID 设定本机ID
 * @{
 */
#define MY_Node_ID NODE_ID_FRONT_DRIVE /*！<当前设备ID 编译时更改为本机ID即可>*/
 /**
  * @}
  */

/** @defgroup FDCAN_Command_Words 核心指令码 (Command)
  * @{
  */
#define CMD_ACTION_EXECUTE  ((uint32_t)0x00U) /*!< 动作执行指令 (下行) */
#define CMD_STATUS_QUERY    ((uint32_t)0x01U) /*!< 状态查询指令 (下行) */
#define CMD_REALTIME_FB     ((uint32_t)0x10U) /*!< 状态实时反馈 (上行) */
#define CMD_HARDWARE_ALARM  ((uint32_t)0x11U) /*!< 硬件故障报警 (上行) */
/**
  * @}
  */

/** @defgroup FDCAN_Priority_Levels 优先级等级定义 (3位: 0~7)
  * @note 数值越小，优先级越高。0 为最高优先级，7 为最低优先级。
  * @{
  */
#define PRIORITY_REALTIME   ((uint32_t)0x00U) /*!< 实时/紧急指令 (如：急停、避障) */
#define PRIORITY_CONTROL    ((uint32_t)0x01U) /*!< 常规控制指令 (如：运动控制) */
#define PRIORITY_FEEDBACK   ((uint32_t)0x02U) /*!< 状态反馈数据 (如：电机角度回传) */
#define PRIORITY_CONFIG     ((uint32_t)0x05U) /*!< 参数配置指令 (如：PID调参) */
#define PRIORITY_HEARTBEAT  ((uint32_t)0x07U) /*!< 低优先级心跳包 */
#define DEFAULT_PRIORITY    PRIORITY_CONTROL  /*!< 默认采用控制级优先级 */
/**
  * @}
  */

/** @defgroup FDCAN_ID_Shifts 29位扩展帧 ID 移位偏移量
  * @{
  */
#define SHIFT_PRIORITY      ((uint32_t)26U)   /*!< 优先级偏移位 [28:26] */
#define SHIFT_COMMAND       ((uint32_t)21U)   /*!< 命令字偏移位 [25:21] */
#define SHIFT_TARGET_ID     ((uint32_t)13U)   /*!< 目标地址偏移位 [20:13] */
#define SHIFT_SOURCE_ID     ((uint32_t)5U)    /*!< 发送方地址偏移位 [12:5] */
/**
  * @}
  */

 
 /**
  * @}
  */
/* ========================================================================= */



HAL_StatusTypeDef BSP_FDCAN_Send_Packet(uint32_t Priority, uint32_t Cmd_Word, uint32_t Target_ID, FDCAN_Data_TX_Buf_t *pPayload);

extern void User_FDCAN_Init(void);

/* ========================================= */

#ifdef __cplusplus
}
#endif

#endif 

