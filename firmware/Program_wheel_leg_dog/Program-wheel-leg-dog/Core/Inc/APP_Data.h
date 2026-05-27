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
#ifndef __APP_Data_H__ 
#define __APP_Data_H__ 

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "main.h"
#include "BSP_Robot_types.h"

/* define ------------------------------------------------------------------*/

#define HAL_Recive_staus_ON 1 /*!<receive flag*/
#define HAL_Recive_staus_OFF 0

/* 外部声明区域 ------------------------------------------------------------------*/

extern FDCAN_Data_RX_Buf_t g_Latest_RX_Data;/*!<实例化FDcan总数据包>*/
extern FDCAN_Data_TX_Buf_t g_Latest_TX_Data;  



 void Front_Board_Data_Init();
 void Back_Board_Data_Init();
 void Board_Main_Cmd_Task();
 void Board_Data_Recall_Task();
 void Robot_Front_Main_Sequence(void);
 void Robot_Back_Main_Sequence(void);  


 
#ifdef __cplusplus
}
#endif

#endif 
