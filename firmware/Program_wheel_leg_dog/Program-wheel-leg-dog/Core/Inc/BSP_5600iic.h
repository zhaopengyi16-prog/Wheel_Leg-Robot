#ifndef __BSP_5600IIC_H
#define __BSP_5600IIC_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h" // 包含 HAL 库和基础类型定义

#define AS5600_RESOLUTION 4096.0f     // 分辨率


uint16_t AS5600_Read_Raw_Angle_Soft(void);


#ifdef __cplusplus
}
#endif

#endif /* __AS5600_SOFT_I2C_H */