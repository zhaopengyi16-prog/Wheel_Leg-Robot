/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : BSP_servo.h
  * @brief          : Servo drive
  ******************************************************************************
  * @attention
  *
  *
  *
  *
  ******************************************************************************
  */
#ifndef __BSP_SERVO_H__   
#define __BSP_SERVO_H__    

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "BSP_Robot_types.h"

/* ========================================================================= */
/**
 * @defgroup 飞特通信协议宏定义协议 
 * @brief  参考官方协议 舵机SCS通信协议
 * @note
 * @{
 */

/**
 * @defgroup 常用指令宏定义
 * @{
 */
#define STS_HEADER               ((uint8_t)0xFFU)   /*!<固定帧头>*/
#define STS_INST_PING            ((uint8_t)0x01U)   /*!<查询指令>*/
#define STS_INST_READ            ((uint8_t)0x02U)   /*!<读指令>*/
#define STS_INST_WRITE           ((uint8_t)0x03U)   /*!<写指令>*/
#define STS_INST_SYNC_WRITE      ((uint8_t)0x83U)   /*!<同步写入>*/
#define STS_INST_SYNC_READ       ((uint8_t)0x82U)   /*!<同步读取>*/

/**
 * @}
 */



void BSP_Servo_Sync_Write_2(BSP_Servo_Cmd_t *p_Cmd1, BSP_Servo_Cmd_t *p_Cmd2);
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size);
void BSP_Servo_Start_DMA_Rx_Init(void);


/**
 * @}
 */

/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif 
