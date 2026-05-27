/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : BSP_fdcan.c
  * @brief          : Canfd communication agreement
  ******************************************************************************
  * @attention
  *
  *
  *
  *
  ******************************************************************************
  */

#include "BSP_fdcan.h" 
#include "fdcan.h"  //introduce fdcan handle 
#include <string.h>
/**
 * 
 */

/**
  * @brief  FDCAN 用户初始化函数 
  * @note   该函数主要功能为配置过滤器参数 写入硬件 启动FDCAN
  * @retval 
  */
void User_FDCAN_Init(void)
{
    FDCAN_FilterTypeDef sFilterConfig;

    /* 1. 配置过滤器参数 */
    sFilterConfig.IdType = FDCAN_EXTENDED_ID;       // Extended ID (29位)
    sFilterConfig.FilterIndex = 0;                  // 过滤器编号 0
    sFilterConfig.FilterType = FDCAN_FILTER_MASK;   // 掩码模式
    sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0; // 通过过滤器的消息存入 FIFO0
    sFilterConfig.FilterID1 = 0x0000;               // ID 校验值
    sFilterConfig.FilterID2 = 0x0000;               // 掩码 (0x0000 表示接收所有拓展 ID 的消息)

    /* 2. 将过滤器配置写入硬件 */
    if (HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK)
    {
        Error_Handler();
    }

    /* 3. 配置全局过滤器（处理没有匹配到上述过滤器的消息） */
    // 拒绝未匹配的标准 ID 和扩展 ID，拒绝远程帧
    HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE);

    /* 4. 启动 FDCAN 模块，正式接入总线 */
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK)
    {
        Error_Handler();
    }

    /* 5. 激活 RX FIFO0 新消息中断 */
    if (HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
    {
        Error_Handler();
    }
} 



/**
  * @brief  FDCAN 通用数据包发送函数
  * @param  Priority: 优先级 (如 PRIORITY_REALTIME)
  * @param  Cmd_Word: 指令码 (如 CMD_HARDWARE_ALARM)
  * @param  Target_ID: 目标 ID
  * @param  pPayload: 数据负载结构体指针
  * @retval HAL_StatusTypeDef
  */
HAL_StatusTypeDef BSP_FDCAN_Send_Packet(uint32_t Priority, uint32_t Cmd_Word, uint32_t Target_ID, FDCAN_Data_TX_Buf_t *pPayload)
{
    FDCAN_TxHeaderTypeDef TxHeader;

    /* 1. 动态组装 ID：所有参数均可控 */
    uint32_t ext_id = ((Priority  << SHIFT_PRIORITY)   | 
                       (Cmd_Word  << SHIFT_COMMAND)    | 
                       (Target_ID << SHIFT_TARGET_ID)  | 
                       (MY_Node_ID << SHIFT_SOURCE_ID));
    
    /* 2. 配置 Header (保持 64 字节和 BRS 加速) */
    TxHeader.Identifier          = ext_id;
    TxHeader.IdType              = FDCAN_EXTENDED_ID;
    TxHeader.TxFrameType         = FDCAN_DATA_FRAME;
    TxHeader.DataLength          = FDCAN_DLC_BYTES_64;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch       = FDCAN_BRS_ON;
    TxHeader.FDFormat            = FDCAN_FD_CAN;
    TxHeader.TxEventFifoControl  = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker       = 0;

    /* 3. 发送 */
    return HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, (uint8_t *)pPayload);
}

/**
  * @brief  FDCAN 接收 FIFO0 中断回调函数 
  * @note   此函数由 HAL 库在接收到新消息时自动调用 
  */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)
{
    FDCAN_RxHeaderTypeDef RxHeader;
    uint8_t RxData[64];

    if ((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
    {
        /* 1. 从硬件 FIFO 提取数据 */
        if (HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader, RxData) == HAL_OK)
        {
            /* 2. 长度校验：只处理 64 字节的控制指令 */
            if (RxHeader.DataLength == FDCAN_DLC_BYTES_64)
            {
                // ==============================================================
                // 【O(1) 复杂度核心】：直接通过指针映射将字节流还原为结构体
                // 这里我们使用 memcpy 保证数据原子性，防止在处理时数据被下一帧覆盖
                // ==============================================================
             memcpy(&g_Latest_RX_Data, RxData, 64);  
                
              //BSP_FDCAN_Send_Packet( PRIORITY_REALTIME,CMD_HARDWARE_ALARM,MY_Node_ID,&g_Latest_RX_Data);
                /* TODO: 在此处可以添加简单的 CRC 校验逻辑 */
                /* if (g_Latest_Host_Cmd.CRC_code == Calculate_CRC8(RxData, 63)) { ... } */
            }
        } 

        /* 3. 必须重新激活中断通知，否则只会触发一次 */ 
        HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0);
    }
}
