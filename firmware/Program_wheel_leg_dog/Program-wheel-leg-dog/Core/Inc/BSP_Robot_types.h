/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : BSP_Robot_types.h
  * @brief          : 全局数据块定义区域
  ******************************************************************************
  * @attention
  *
  *
  *
  *
  ******************************************************************************
  */
#ifndef __BSP_ROBOT_TYPES_H__
#define __BSP_ROBOT_TYPES_H__

#ifdef __cplusplus 
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h" 


/*****************上行数据块区域*************************/

 #pragma pack (push,1) /*!<强制字节对齐>*/ 
 
/** @defgroup 无刷电机状态块 
  * @{
  */

typedef struct  /*!<电机状态数据块，供占用16字节>*/ 
{
  uint8_t Mode;               /*!<电机当前模式 1*/ 
  uint8_t Occupy_byte[1];     /*!<占位字节 1*/ 
  float Actual_Angle;         /*!<当前角度 4*/ 
  float Actual_speed;         /*!<当前速度 4*/
  float Actual_current;       /*!<当前电流 4*/
  uint8_t Reserve[2];         /*!<预留位 2*/

}FD_CAN_BLDC_Status_t; 

/** 
  * @}
  */
 /** @defgroup 舵机状态块 
  * @{
  */

typedef struct  /*!<舵机状态数据块，供占用8字节>*/
{
    uint8_t  Servo_ID;       /*!<舵机ID>*/ 
    uint8_t  Error_Code;     /*!<错误码*/ 
    int16_t  Present_Pos;    /*!<当前位置*/ 
    int16_t  Present_Speed;  /*!<当前速度*/ 
    int16_t  Present_Load;   /*!<当前负载*/ 
}FD_CAN_SERVO_Status_t; 


/** 
  * @}
  */


  /** @defgroup 总体上行状态块 
  * @{
  */

typedef struct  /*!<上行数据包，64字节>*/ 
{
  uint8_t Mode;                 /*!<整板指令1*/
  uint8_t Occupy_byte[1];       /*!<占位字节 1-3*/
  FD_CAN_BLDC_Status_t Motor1;  /*!<第一个电机数据块16*/
  FD_CAN_BLDC_Status_t Motor2;  /*!<第二个电机数据块16*/
  FD_CAN_SERVO_Status_t Servo1; /*!<第一个舵机数据块8*/
  FD_CAN_SERVO_Status_t Servo2; /*!<第二个舵机数据块8*/
  uint8_t Error_code;           /*!<故障码>1*/
  uint8_t CRC_code;             /*!<CRC>1*/
  uint8_t Reserve[12];          /*!<Reserve>18*/
  
}FDCAN_Data_TX_Buf_t;


/** 
  * @}
  */
/*****************下行数据块区域*************************/

  /** @defgroup 舵机指令数据块 
  * @{
  */
 typedef struct 
 {
    uint8_t  ID;            /*!<舵机ID>*/
    uint8_t  Accel;         /*!<加速度 (寄存器 0x29)*/ 
    int16_t  Target_Pos;    /*!<目标位置 (寄存器 0x2A)*/ 
    uint16_t Running_Time;  /*!<运行时间 (寄存器 0x2E)*/ 
    uint16_t Running_Speed; /*!<运行速度 (寄存器 0x2C)*/ 
    uint8_t  Mode;          /*!<运行模式 (0x21)*/ 
    uint8_t  Reserved;     /*!<补位，凑成 10 字节偶数对齐*/ 
 }BSP_Servo_Cmd_t;
 

  /** @defgroup 总体下行状态块
  * @{
  */

typedef struct  /*!<下行数据包，64字节>*/
{
  uint8_t Mode;                 /*!<整板指令1*/
  uint8_t Occupy_byte[1];       /*!<占位字节 1-3*/
  FD_CAN_BLDC_Status_t Motor1;  /*!<第一个电机数据块16*/
  FD_CAN_BLDC_Status_t Motor2;  /*!<第二个电机数据块16*/
  BSP_Servo_Cmd_t Servo1;       /*!<第一个舵机数据块10*/
  BSP_Servo_Cmd_t Servo2;       /*!<第二个舵机数据块10*/
  uint8_t Error_code;           /*!<故障码>1*/
  uint8_t CRC_code;             /*!<CRC>1*/
  uint8_t Reserve[8];           /*!<Reserve>18*/
  
}FDCAN_Data_RX_Buf_t;

/** 
  * @}
  */

/** 
  * @}
  */

#pragma pack(pop)


#ifdef __cplusplus
}
#endif

#endif 
