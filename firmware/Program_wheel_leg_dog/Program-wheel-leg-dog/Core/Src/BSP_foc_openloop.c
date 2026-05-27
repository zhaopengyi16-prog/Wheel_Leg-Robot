/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : BSP_foc_openloop.c
  * @brief          : foc
  ******************************************************************************
  * @attention
  *
  *
  *
  *
  ******************************************************************************
  */
 #include "BSP_foc_openloop.h"
 #include "BSP_5600iic.h"
 #include "adc.h"

 /**
 */
// 
float    current_elec_angle = 0.0f;   // 算出来的最终电角度 (弧度，0~2PI)

// 实例化电机对象，默认给 7 对极，初始推力给 0 (安全第一)
 FOC_Motor_t motor1 = {
    .pole_pairs = POLE_PAIRS,
    .target_Vq = 0.0f,
    .target_Vd = 0.0f
};

/**
 * @brief 6步驱动换相法
 */

void SixStep_Output(uint8_t step, float duty_cycle) 
{
    // 安全限幅
    if(duty_cycle > 1.0f) duty_cycle = 1.0f;
    if(duty_cycle < 0.0f) duty_cycle = 0.0f;

    uint16_t pwm_high = (uint16_t)(duty_cycle * TIM1_ARR_VALUE);
    uint16_t pwm_low  = 0; // 0 代表下桥臂 100% 导通 (接GND)

    // 核心法则：先关闭该悬空的相，再更新 PWM，最后打开该通电的相 (防止短路)
    switch(step) {
        case 0: // A通VCC, B通GND, C悬空
            PHASE_C_DISABLE();
            TIM1->CCR1 = pwm_high; TIM1->CCR2 = pwm_low;
            PHASE_A_ENABLE(); PHASE_B_ENABLE();
            break;
        case 1: // A通VCC, C通GND, B悬空
            PHASE_B_DISABLE();
            TIM1->CCR1 = pwm_high; TIM1->CCR3 = pwm_low;
            PHASE_A_ENABLE(); PHASE_C_ENABLE();
            break;
        case 2: // B通VCC, C通GND, A悬空
            PHASE_A_DISABLE();
            TIM1->CCR2 = pwm_high; TIM1->CCR3 = pwm_low;
            PHASE_B_ENABLE(); PHASE_C_ENABLE();
            break;
        case 3: // B通VCC, A通GND, C悬空
            PHASE_C_DISABLE();
            TIM1->CCR2 = pwm_high; TIM1->CCR1 = pwm_low;
            PHASE_B_ENABLE(); PHASE_A_ENABLE();
            break;
        case 4: // C通VCC, A通GND, B悬空
            PHASE_B_DISABLE();
            TIM1->CCR3 = pwm_high; TIM1->CCR1 = pwm_low;
            PHASE_C_ENABLE(); PHASE_A_ENABLE();
            break;
        case 5: // C通VCC, B通GND, A悬空
            PHASE_A_DISABLE();
            TIM1->CCR3 = pwm_high; TIM1->CCR2 = pwm_low;
            PHASE_C_ENABLE(); PHASE_B_ENABLE();
            break;
        default: // 紧急停车：全悬空
            PHASE_A_DISABLE(); PHASE_B_DISABLE(); PHASE_C_DISABLE();
            break;
    }
}
/**
 * @brief 6步驱动零点校准
 */
void SixStep_Align_Zero(void)
{
    // 为了和 FOC 坐标系保持一致，我们依然在 0 度（D轴）产生强大的电磁吸力
    // 做法：A相接30%推力，B和C相全部接GND。这会产生一个完美的 0度 磁场。
    TIM1->CCR1 = (uint16_t)(0.3f * TIM1_ARR_VALUE); // 30%的推力，如果太烫改到 0.2f
    TIM1->CCR2 = 0;
    TIM1->CCR3 = 0;

    PHASE_A_ENABLE();
    PHASE_B_ENABLE();
    PHASE_C_ENABLE();

    HAL_Delay(2000); // 等待2秒死死锁住

    uint32_t sum = 0;
    for(int i=0; i<10; i++){
        sum += (4095 - AS5600_Read_Raw_Angle_Soft());
        HAL_Delay(5);
    }
    motor1.zero_offset = sum / 10;

    // 校准完毕，立刻全悬空断电！
    PHASE_A_DISABLE();
    PHASE_B_DISABLE();
    PHASE_C_DISABLE();
    HAL_Delay(10);
}

/**
 * @brief PWM send function 发波函数
 */
void Set_Motor_Voltage() 
{

    motor1.elec_angle = 0;
    motor1.target_Vd = 3.0f;
    motor1.target_Vq = 0.0f; 
    
    //inv_park 
    Inverse_Park_Transform(motor1.target_Vd,motor1.target_Vq,
    sinf(motor1.elec_angle),cosf(motor1.elec_angle),&motor1.Valpha,&motor1.Vbeta);

    //SVPWM
    SVPWM_Calc(
            motor1.Valpha,
            motor1.Vbeta,
            VBUS_VOLTAGE,
            &motor1.DutyA,
            &motor1.DutyB,
            &motor1.DutyC
        );

        if(motor1.DutyA > 1.0f) motor1.DutyA = 1.0f; else if(motor1.DutyA < 0.0f) motor1.DutyA = 0.0f;
        if(motor1.DutyB > 1.0f) motor1.DutyB = 1.0f; else if(motor1.DutyB < 0.0f) motor1.DutyB = 0.0f;
        if(motor1.DutyC > 1.0f) motor1.DutyC = 1.0f; else if(motor1.DutyC < 0.0f) motor1.DutyC = 0.0f;

        TIM1->CCR1 = (uint16_t)(motor1.DutyA * TIM1_ARR_VALUE); 
        TIM1->CCR2 = (uint16_t)(motor1.DutyB * TIM1_ARR_VALUE);
        TIM1->CCR3 = (uint16_t)(motor1.DutyC * TIM1_ARR_VALUE);
}

/**
 * @brief FOC零点校准
 */
void FOC_Align_Zero_Init(void)
{
    Set_Motor_Voltage();
    HAL_Delay(500); // 等待机械震荡彻底平息
    HAL_Delay(1500);

    uint32_t sum = 0;
    for(int i=0; i<10; i++){
        sum += (4095 - AS5600_Read_Raw_Angle_Soft()); // 👈 核心改动
        HAL_Delay(5);
    }
    motor1.zero_offset = sum / 10;
}

/**
 * @brief FOC硬件初始化函数 (在 main.c 中调用一次)
 */
void BSP_FOC_TIM1_Init()
{
    // 1. 使能DRV8316
    HAL_GPIO_WritePin(A_SLEEP_GPIO_Port, A_SLEEP_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(B_SLEEP_GPIO_Port, B_SLEEP_Pin, GPIO_PIN_SET);

    HAL_Delay(1);

    // ==========================================
    // [绝对关键新增]：启动 ADC 硬件自校准并开启注入模式
    // 必须在 TIM1 启动之前调用！
    // ==========================================
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
	HAL_ADCEx_InjectedStart(&hadc1);

    // 2. 只启动三个主通道
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_2);
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_3);

    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_1); 
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_2); 
    HAL_TIMEx_PWMN_Start(&htim1, TIM_CHANNEL_3);

    // 3. 互补通道全部删除，不调用HAL_TIMEx_PWMN_Start
    
    // 4. 手动置位MOE（主输出使能）
    __HAL_TIM_MOE_ENABLE(&htim1);

    // 5. CCR预装载
    TIM1->CCMR1 |= (TIM_CCMR1_OC1PE | TIM_CCMR1_OC2PE);
    TIM1->CCMR2 |=  TIM_CCMR2_OC3PE;

    // 调用新的六步换相零点校准！
   // SixStep_Align_Zero();
    // 6.零点校准
   // FOC_Align_Zero_Init();

    // 7. 启动中断
   // HAL_TIM_Base_Start_IT(&htim1);
   

}

/**
 * @brief  Clarke 变换 (恒幅值变换)
 * @param  Ia      输入 A 相电流
 * @param  Ib      输入 B 相电流
 * @param  *Ialpha 输出 Alpha 轴电流指针
 * @param  *Ibeta  输出 Beta 轴电流指针
 */
void Clarke_Transform(float Ia, float Ib, float *Ialpha, float *Ibeta) 
{
    *Ialpha = Ia;
    // 公式: Ibeta = (1/sqrt(3)) * Ia + (2/sqrt(3)) * Ib
    // 提取公因式优化后: Ibeta = (Ia + 2.0 * Ib) / sqrt(3)
    *Ibeta = (Ia + 2.0f * Ib) * ONE_BY_SQRT3; 
}

/**
 * @brief  Park 变换
 * @param  Ialpha  输入 Alpha 轴电流
 * @param  Ibeta   输入 Beta 轴电流
 * @param  sin_th  电角度的正弦值 sin(theta)
 * @param  cos_th  电角度的余弦值 cos(theta)
 * @param  *Id     输出 d 轴电流指针 (直轴-发热)
 * @param  *Iq     输出 q 轴电流指针 (交轴-扭矩)
 */
void Park_Transform(float Ialpha, float Ibeta, float sin_th, float cos_th, float *Id, float *Iq) 
{
    *Id =  Ialpha * cos_th + Ibeta * sin_th;
    *Iq = -Ialpha * sin_th + Ibeta * cos_th;
}

/**
 * @brief  反 Park 变换
 * @param  Vd      输入 d 轴目标电压
 * @param  Vq      输入 q 轴目标电压
 * @param  sin_th  电角度的正弦值 sin(theta)
 * @param  cos_th  电角度的余弦值 cos(theta)
 * @param  *Valpha 输出 Alpha 轴电压指针
 * @param  *Vbeta  输出 Beta 轴电压指针
 */
void Inverse_Park_Transform(float Vd, float Vq, float sin_th, float cos_th, float *Valpha, float *Vbeta) 
{
    *Valpha = Vd * cos_th - Vq * sin_th;
    *Vbeta  = Vd * sin_th + Vq * cos_th;
}

/**
 * @brief  反 Clarke 变换
 * @param  Valpha  输入 Alpha 轴电压
 * @param  Vbeta   输入 Beta 轴电压
 * @param  *Va     输出 A 相目标电压指针
 * @param  *Vb     输出 B 相目标电压指针
 * @param  *Vc     输出 C 相目标电压指针
 */
void Inverse_Clarke_Transform(float Valpha, float Vbeta, float *Va, float *Vb, float *Vc) 
{
    *Va = Valpha;
    *Vb = -0.5f * Valpha + SQRT3_BY_2 * Vbeta;
    *Vc = -0.5f * Valpha - SQRT3_BY_2 * Vbeta;
}


/**
 * @brief  SVPWM 零序注入法计算模块 (Min-Max SVPWM)
 * @param  Valpha  输入: Alpha 轴目标电压 (反Park算出来的)
 * @param  Vbeta   输入: Beta 轴目标电压  (反Park算出来的)
 * @param  Vbus    输入: 当前系统的母线电压 (例如 12.0f)
 * @param  *DutyA  输出: A相占空比指针 (范围 0.0f ~ 1.0f)
 * @param  *DutyB  输出: B相占空比指针 (范围 0.0f ~ 1.0f)
 * @param  *DutyC  输出: C相占空比指针 (范围 0.0f ~ 1.0f)
 */
void SVPWM_Calc(float Valpha, float Vbeta, float Vbus, float *DutyA, float *DutyB, float *DutyC) 
{
    // 1. 软件防爆：限制矢量长度不超过正六边形的内切圆
    // 最大允许相电压幅值为 Vbus / sqrt(3)
    float limit = Vbus * 0.57735027f; 
    float V_mag = sqrtf(Valpha * Valpha + Vbeta * Vbeta);
    if (V_mag > limit) {
        Valpha = (Valpha / V_mag) * limit;
        Vbeta  = (Vbeta  / V_mag) * limit;
    }

    // 2. 虚拟三相相电压 (Inverse Clarke 的一部分)
    float Va = Valpha;
    float Vb = -0.5f * Valpha + 0.86602540f * Vbeta;
    float Vc = -0.5f * Valpha - 0.86602540f * Vbeta;

    // 3. 寻找最大值和最小值
    float Vmax = fmaxf(Va, fmaxf(Vb, Vc)); 
    float Vmin = fminf(Va, fminf(Vb, Vc)); 

    // 4. 计算零序共模电压 (这就是产生马鞍波的魔法)
    float Vcom = 0.5f * (Vmax + Vmin);

    // 5. 注入零序电压，并映射为 0.0~1.0 的占空比
    // 注意：+0.5f 表示占空比 50% 的时候，对于电机来说电压是 0 (相对中性点)
    *DutyA = 0.5f + (Va - Vcom) / Vbus;
    *DutyB = 0.5f + (Vb - Vcom) / Vbus;
    *DutyC = 0.5f + (Vc - Vcom) / Vbus;

    // 6. 最后的安全限幅 (绝对不能让占空比越界)
    *DutyA = fmaxf(0.0f, fminf(1.0f, *DutyA));
    *DutyB = fmaxf(0.0f, fminf(1.0f, *DutyB));
    *DutyC = fmaxf(0.0f, fminf(1.0f, *DutyC));
}



/**
 * @brief  获取最新的电角度 (弧度制, 0~2PI)
 * @return 最终给 Inverse_Park 使用的电角度
 */
float FOC_Get_Electrical_Angle(void)
{
    // 1. 读取最新的 AS5600 原始值 (0~4095)
    uint16_t raw_angle = AS5600_Read_Raw_Angle_Soft();

    // 2. 【核心大招】：软件翻转编码器方向！与电机电磁方向强行统一！
    raw_angle = 4095 - raw_angle; // 👈 核心改动

    // 2. 减去零点偏移，得到实际相对读数
    int32_t relative_angle = (int32_t)raw_angle - (int32_t)motor1.zero_offset;
    
    // 3. 如果减成了负数，加上一圈 (4096) 把它拉正
    if(relative_angle < 0) {
        relative_angle += 4096;
    }

    // 4. 计算机械弧度 (Mechanical Radian)
    // 公式: (relative_angle / 4096.0f) * 2 * PI
    float mech_rad = ((float)relative_angle / AS5600_RESOLUTION) * TWO_PI;

    // 5. 乘以极对数，得到电弧度 (Electrical Radian)
    
    float elec_rad = mech_rad * POLE_PAIRS;

    // 6. 将电角度归一化到 0 ~ 2*PI 之间 (防止跑圈溢出)
    // fmodf 是 C 语言 math.h 里的浮点数取余函数
    elec_rad = fmodf(elec_rad, TWO_PI);
    
    // 如果取余后是负数(虽然理论上上面不会出现)，保险起见再加一次 TWO_PI
    if(elec_rad < 0) {
        elec_rad += TWO_PI;
    }

    // 7. 更新全局变量并返回
    current_elec_angle = elec_rad;
    return elec_rad;
}
/**
 * @brief Text函数 6步换相测试区
 */
void SixStep_Closed_Loop(float target_duty,float angle)
{
    // 1. 获取当前精确电角度
    float elec_angle = angle;

    // 2. 将电磁场强行推到转子的正前方 90 度位置 (PI / 2)
    // 这就是产生最大扭矩的“挂胡萝卜”大法！
    float target_angle = elec_angle + (PI / 2.0f);
    if(target_angle >= TWO_PI) target_angle -= TWO_PI;

    // 3. 计算目标角度属于 6 个扇区中的哪一个
    // 360度 / 6 = 60度 (PI / 3)
    uint8_t sector = (uint8_t)(target_angle / (PI / 3.0f));
    if(sector > 5) sector = 5; // 安全防溢出

    // 4. 根据扇区输出开关信号
    SixStep_Output(sector, target_duty);
}

/**
 * @brief Text函数 foc跑测试专用更改区域
 */

void FOC_Text_Loop(float current_angle,float target_Vq) 
{
    
   
    motor1.elec_angle = current_angle;
    motor1.target_Vd = 0.0f;
    motor1.target_Vq = target_Vq; 
    
    //inv_park 
    Inverse_Park_Transform(motor1.target_Vd,motor1.target_Vq, 
    sinf(motor1.elec_angle),cosf(motor1.elec_angle),&motor1.Valpha,&motor1.Vbeta); 

    //SVPWM
    SVPWM_Calc(

            motor1.Valpha,
            motor1.Vbeta,
            VBUS_VOLTAGE,               
            &motor1.DutyA, 
            &motor1.DutyB,
            &motor1.DutyC
        );
    
        if(motor1.DutyA > 1.0f) motor1.DutyA = 1.0f; else if(motor1.DutyA < 0.0f) motor1.DutyA = 0.0f;
        if(motor1.DutyB > 1.0f) motor1.DutyB = 1.0f; else if(motor1.DutyB < 0.0f) motor1.DutyB = 0.0f;
        if(motor1.DutyC > 1.0f) motor1.DutyC = 1.0f; else if(motor1.DutyC < 0.0f) motor1.DutyC = 0.0f;

        TIM1->CCR1 = (uint16_t)(motor1.DutyA * TIM1_ARR_VALUE);
        TIM1->CCR2 = (uint16_t)(motor1.DutyB * TIM1_ARR_VALUE);
        TIM1->CCR3 = (uint16_t)(motor1.DutyC * TIM1_ARR_VALUE);
    

}