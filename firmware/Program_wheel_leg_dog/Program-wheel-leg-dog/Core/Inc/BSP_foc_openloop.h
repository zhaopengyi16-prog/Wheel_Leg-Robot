#ifndef __BSP_FOC_OPENLOOP_H__
#define __BSP_FOC_OPENLOOP_H__


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "tim.h"
#include "gpio.h"
#include "BSP_5600iic.h"
#include <math.h>
#include <stdint.h>



//6步驱动法宏定义

#define PHASE_A_ENABLE()  (TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE))
#define PHASE_A_DISABLE() (TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE))

#define PHASE_B_ENABLE()  (TIM1->CCER |= (TIM_CCER_CC2E | TIM_CCER_CC2NE))
#define PHASE_B_DISABLE() (TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE))

#define PHASE_C_ENABLE()  (TIM1->CCER |= (TIM_CCER_CC3E | TIM_CCER_CC3NE))
#define PHASE_C_DISABLE() (TIM1->CCER &= ~(TIM_CCER_CC3E | TIM_CCER_CC3NE))

// 定义常量，避免单片机每次都在循环里算开方
#define SQRT3       1.73205081f
#define ONE_BY_SQRT3 0.57735027f // 1 / sqrt(3)
#define SQRT3_BY_2  0.86602540f  // sqrt(3) / 2
#define PI               3.1415926535f
#define TWO_PI           6.2831853071f
#define POLE_PAIRS       7            // 你的电机极对数 (假设为7，请根据实际修改！)
// 母线供电是 12V
#define VBUS_VOLTAGE 12.0f 
// 【重点注意】根据你 CubeMX 里 TIM1 的 ARR 值修改！
#define TIM1_ARR_VALUE 4249.0f

// ==========================================
// 准工业级 FOC 电机对象结构体
// ==========================================
typedef struct {
    uint8_t  pole_pairs;    // 极对数 (2208电机通常是 7)
    float    mech_angle;    // 机械角度 (弧度, 0~2PI)
    float    elec_angle;    // 电角度 (弧度, 0~2PI)
    float    target_Vq;     // 目标交轴电压 (推力，相当于油门)
    float    target_Vd;     // 目标直轴电压 (发热，设为0)
    float    Valpha;        // 算出的 Alpha 轴电压
    float    Vbeta;         // 算出的 Beta  轴电压
    float    DutyA;         // A相最终占空比 (0~1.0)
    float    DutyB;         // B相最终占空比
    float    DutyC;         // C相最终占空比
    float    zero_offset;   // 电角度零点偏置
} FOC_Motor_t;

extern FOC_Motor_t motor1; // 声明外部变量，让 main.c 也能用到

// 核心控制循环函数声明
void SVPWM_Calc(float Valpha, float Vbeta, float Vbus, float *DutyA, float *DutyB, float *DutyC);

void Inverse_Clarke_Transform(float Valpha, float Vbeta, float *Va, float *Vb, float *Vc);
void Inverse_Park_Transform(float Vd, float Vq, float sin_th, float cos_th, float *Valpha, float *Vbeta);
void Park_Transform(float Ialpha, float Ibeta, float sin_th, float cos_th, float *Id, float *Iq);
void Clarke_Transform(float Ia, float Ib, float *Ialpha, float *Ibeta);

float FOC_Get_Electrical_Angle(void);

void FOC_Text_Loop(float current_angle,float target_Vq) ;
void BSP_FOC_TIM1_Init();


void SixStep_Closed_Loop(float target_duty,float angle);
void SixStep_Output(uint8_t step, float duty_cycle);
#ifdef __cplusplus
}
#endif

#endif /* __FOC_OPENLOOP_H__ */