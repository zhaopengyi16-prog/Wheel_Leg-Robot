#include "BSP_5600iic.h"

/* =================================================================================
 * 1. 引脚和延时宏定义 (如果引脚改了，只在这里修改即可)
 * ================================================================================= */
// 根据 CubeMX 配置的引脚: SCL -> PC4, SDA -> PC5
#define SCL_PORT GPIOC
#define SCL_PIN  GPIO_PIN_4
#define SDA_PORT GPIOC
#define SDA_PIN  GPIO_PIN_5

#define SCL_H    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_SET)
#define SCL_L    HAL_GPIO_WritePin(SCL_PORT, SCL_PIN, GPIO_PIN_RESET)
#define SDA_H    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_SET)
#define SDA_L    HAL_GPIO_WritePin(SDA_PORT, SDA_PIN, GPIO_PIN_RESET)
#define SDA_READ HAL_GPIO_ReadPin(SDA_PORT, SDA_PIN)

/* AS5600 地址和寄存器定义 */
#define AS5600_ADDR_WRITE  (0x36 << 1)       // 写地址: 0x6C
#define AS5600_ADDR_READ   ((0x36 << 1) | 1) // 读地址: 0x6D
#define AS5600_ANGLE_REG   0x0E              // 角度寄存器首地址

/* =================================================================================
 * 2. 软件 I2C 底层时序实现 (内部私有函数，主程序不可见)
 * ================================================================================= */
static void SW_I2C_Delay(void) {
    // 适配 G431 (170MHz) 的微秒级延时，控制 I2C 频率在 400kHz 左右
    for(volatile uint32_t i = 0; i < 40; i++); 
}

static void SW_I2C_Start(void) {
    SDA_H; SCL_H; SW_I2C_Delay();
    SDA_L; SW_I2C_Delay();
    SCL_L; SW_I2C_Delay();
}

static void SW_I2C_Stop(void) {
    SDA_L; SCL_L; SW_I2C_Delay();
    SCL_H; SW_I2C_Delay();
    SDA_H; SW_I2C_Delay();
}

static uint8_t SW_I2C_WaitAck(void) {
    uint8_t ack = 0;
    SDA_H; SW_I2C_Delay(); // 主机释放数据线，交给从机拉低
    SCL_H; SW_I2C_Delay();
    ack = SDA_READ;        // 读取 SDA 状态 (0 为应答，1 为非应答)
    SCL_L; SW_I2C_Delay();
    return ack;
}

static void SW_I2C_SendByte(uint8_t byte) {
    for (int i = 0; i < 8; i++) {
        if (byte & 0x80) SDA_H;
        else SDA_L;
        byte <<= 1;
        SW_I2C_Delay();
        SCL_H; SW_I2C_Delay();
        SCL_L; SW_I2C_Delay();
    }
}

static uint8_t SW_I2C_ReadByte(uint8_t ack) {
    uint8_t byte = 0;
    SDA_H; // 主机释放数据线，准备接收从机数据
    for (int i = 0; i < 8; i++) {
        byte <<= 1;
        SCL_H; SW_I2C_Delay();
        if (SDA_READ) byte++;
        SCL_L; SW_I2C_Delay();
    }
    // 主机发送应答 (ACK=1 传低电平，NACK=0 传高电平)
    if (ack) SDA_L; else SDA_H;
    SW_I2C_Delay();
    SCL_H; SW_I2C_Delay();
    SCL_L; SW_I2C_Delay();
    return byte;
}

/* =================================================================================
 * 3. 供外部调用的应用层接口
 * ================================================================================= */
uint16_t AS5600_Read_Raw_Angle_Soft(void) {
    uint8_t msb, lsb;
    
    // 1. 发送 Start 信号和写地址
    SW_I2C_Start();
    SW_I2C_SendByte(AS5600_ADDR_WRITE);  
    if (SW_I2C_WaitAck()) { SW_I2C_Stop(); return 0xFFFF; } // 如果没应答，立刻退出报错
    
    // 2. 指向角度寄存器 0x0E
    SW_I2C_SendByte(AS5600_ANGLE_REG);       
    if (SW_I2C_WaitAck()) { SW_I2C_Stop(); return 0xFFFF; }
    
    // 3. 重新发送 Start 信号 (Restart) 和读地址
    SW_I2C_Start();              
    SW_I2C_SendByte(AS5600_ADDR_READ); 
    if (SW_I2C_WaitAck()) { SW_I2C_Stop(); return 0xFFFF; }
    
    // 4. 连续读取两个字节 (高8位和低4位)
    msb = SW_I2C_ReadByte(1);    // 读高位，回复 ACK(1) 告诉从机还要继续读
    lsb = SW_I2C_ReadByte(0);    // 读低位，回复 NACK(0) 告诉从机读完了
    
    // 5. 结束通讯
    SW_I2C_Stop();
    
    // 6. 拼接数据，屏蔽掉无用的最高 4 位
    return ((msb << 8) | lsb) & 0x0FFF; 
}