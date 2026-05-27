/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : BSP_servo.c
  * @brief          : servo drive
  ******************************************************************************
  * @attention
  *
  *
  *
  *
  ******************************************************************************
  */
 #include "BSP_servo.h"
 #include "usart.h"
 #include "string.h"
 #include "BSP_fdcan.h"
/*********************************************************************** */



/* DMA 接收大缓存 (足够装下舵机回传的一帧，比如 16 字节) */
uint8_t servo_dma_rx_buf[32]; 

/*********************************************************************** */


/**
 * @brief 校验函数 STS_Calc_Checksum
 * @brief  ~(ID + Length + Instruction + Params) & 0xFF
 * @param  packet: 校验数据包
 * @param  length：数据长度
 * 
 */
static uint8_t STS_Calc_Checksum(uint8_t *packet, uint8_t length)
{
    uint32_t sum = 0;
    // 从帧包的第3个字节(ID位)开始加，加到校验和的前一位 
    for (uint8_t i = 2; i < length - 1; i++) 
    {
        sum += packet[i];
    }
    return (uint8_t)(~sum);
}


/**
 * @brief 初始化函数 (在 main.c 中调用一次)
 */
void BSP_Servo_Start_DMA_Rx_Init(void)
{
    // 开启 DMA 并使能 IDLE 中断
    HAL_UARTEx_ReceiveToIdle_DMA(&huart1, servo_dma_rx_buf, sizeof(servo_dma_rx_buf));
}

/**
 * @brief  空闲中断回调函数 (当一帧数据接收完毕，串口线上无数据流动时，硬件自动触发)
 * @param  Size: 硬件自动计算出的这帧数据一共多少个字节
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
    if (huart->Instance == USART1) // 确保是舵机的串口
    {
        // 1. 最基础的长度和帧头过滤
        if (Size >= 6 && servo_dma_rx_buf[0] == 0xFF && servo_dma_rx_buf[1] == 0xFF)
        {
            uint8_t id = servo_dma_rx_buf[2];
            uint8_t len = servo_dma_rx_buf[3];
            
            // 2. 严谨的二次校验：算一下校验和
            uint8_t chk = 0;
            for(int i = 2; i < Size - 1; i++) chk += servo_dma_rx_buf[i];
            chk = ~chk;
            
            if (chk == servo_dma_rx_buf[Size - 1]) // 校验通过
            {
                /* 3. 执行【纯内存拓印】 */
                FD_CAN_SERVO_Status_t *pStatus = NULL;
                if (id == 1) pStatus = &g_Latest_TX_Data.Servo1;
                else if (id == 2) pStatus = &g_Latest_TX_Data.Servo2;
                
                if (pStatus != NULL && len == 0x08) // 确保是我们请求的那个长度
                {
                    pStatus->Servo_ID = id;
                    pStatus->Error_Code = servo_dma_rx_buf[4];
                    // 拓印位置、速度、负载 (6个连续字节)
                    memcpy(&(pStatus->Present_Pos), &servo_dma_rx_buf[5], 6);
                }
            }
        }  
        
        // 4. 重启下一次 DMA 接收 (极其关键，否则只能收一次)
        HAL_UARTEx_ReceiveToIdle_DMA(&huart1, servo_dma_rx_buf, sizeof(servo_dma_rx_buf));
    }
}



/**
 * @brief   双舵机同步写函数 (SYNC_WRITE)
 * @note    将业务层的数据块解包，映射到连续的物理寄存器发送
 * @param   p_Cmd1: 舵机1的控制数据块指针
 * @param   p_Cmd2: 舵机2的控制数据块指针
 */
/**
 * @brief   双舵机同步写函数 (SYNC_WRITE) - 【终极重构版】
 * @note    1. 严格对齐飞特官方寄存器：0x29(加), 0x2A(位), 0x2C(时), 0x2E(速)
 * 2. 加入单线半双工的强硬“回音消除”与缓冲区净化机制
 * @param   p_Cmd1: 舵机1的控制数据块指针
 * @param   p_Cmd2: 舵机2的控制数据块指针
 */
/**
 * @brief   双舵机同步写函数 (SYNC_WRITE) - 【纯硬件半双工版】
 * @note    适用于 CubeMX 开启了 Half-duplex Single-wire 的配置
 */
void BSP_Servo_Sync_Write_2(BSP_Servo_Cmd_t *p_Cmd1, BSP_Servo_Cmd_t *p_Cmd2)
{
    uint8_t tx_buf[24]; 
    
    // 1. 帧头与指令
    tx_buf[0] = 0xFF; tx_buf[1] = 0xFF; tx_buf[2] = 0xFE; 
    tx_buf[3] = 0x14; tx_buf[4] = 0x83; tx_buf[5] = 0x29; tx_buf[6] = 0x07; 

    // 2. 舵机1数据 (加 -> 位 -> 时 -> 速)
    tx_buf[7]  = p_Cmd1->ID;
    tx_buf[8]  = p_Cmd1->Accel;     
    tx_buf[9]  = p_Cmd1->Target_Pos & 0xFF;         
    tx_buf[10] = (p_Cmd1->Target_Pos >> 8) & 0xFF;  
    tx_buf[11] = p_Cmd1->Running_Time & 0xFF;       
    tx_buf[12] = (p_Cmd1->Running_Time >> 8) & 0xFF;
    tx_buf[13] = p_Cmd1->Running_Speed & 0xFF;      
    tx_buf[14] = (p_Cmd1->Running_Speed >> 8) & 0xFF;

    // 3. 舵机2数据
    tx_buf[15] = p_Cmd2->ID;
    tx_buf[16] = p_Cmd2->Accel;
    tx_buf[17] = p_Cmd2->Target_Pos & 0xFF;
    tx_buf[18] = (p_Cmd2->Target_Pos >> 8) & 0xFF;
    tx_buf[19] = p_Cmd2->Running_Time & 0xFF;
    tx_buf[20] = (p_Cmd2->Running_Time >> 8) & 0xFF;
    tx_buf[21] = p_Cmd2->Running_Speed & 0xFF;
    tx_buf[22] = (p_Cmd2->Running_Speed >> 8) & 0xFF;

    // 4. 校验和
    tx_buf[23] = STS_Calc_Checksum(tx_buf, 24);

    // ==========================================
    // 5. 优雅的硬件级收发切换
    // ==========================================
    // 告诉单片机：我要占领总线当发送方了 (内部自动切断RX)
    HAL_HalfDuplex_EnableTransmitter(&huart1);
    
    // 阻塞发送
    HAL_UART_Transmit(&huart1, tx_buf, 24, 10);
    
    // 发送完毕，立刻交出总线，变回接收方倾听
    HAL_HalfDuplex_EnableReceiver(&huart1);
    
    // 注意：因为你在 main 初始化里调用了一次 ReceiveToIdle_DMA，
    // 且刚才的发送没有触发 DMA 错误，所以不需要在这里重新启动 DMA。它一直在后台安静工作。
}

