/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : APP_Jump.c
  * @brief          : 轮足机器狗垂直起跳测试程序
  ******************************************************************************
  * @attention
  *
  * 【跳跃逻辑】
  *   1. 四条腿同时压到最低 (深蹲保压)
  *   2. 瞬间爆发四条腿的力量 (Accel=0 / Speed=0 阶跃指令)
  *   3. 空中收腿
  *   4. 落地缓冲
  *
  * 【垂直方向保证】
  *   所有位置以 2048 精确对称:
  *     深蹲: S1 = 2048-300 = 1748,  S2 = 2048+300 = 2348  (偏移 ±300)
  *     蹬地: S1 = 2048+400 = 2448,  S2 = 2048-400 = 1648  (偏移 ±400)
  *   前后板 HAL_Delay 序列完全一致 → 四腿同时爆发 → 合力纯 Z 轴向上.
  *
  * 【调参说明】
  *   JUMP_HOLD_MS    深蹲保压时长, 越长四腿到位越充分.
  *   JUMP_IMPULSE_MS 冲量窗口: 跳不起来 → +10ms; 有前后偏 → -10ms.
  *   JUMP_AIRBORNE_MS 滞空时长: 跳得越高适当加大.
  *
  ******************************************************************************
  */

#include "APP_Jump.h"
#include "APP_Data.h"
#include "BSP_servo.h"

/* =========================================================================
 * 前板垂直起跳任务  (Servo ID 1 & 2)
 * ========================================================================= */
void Robot_Front_Jump_Task(void)
{
    g_Latest_RX_Data.Servo1.ID   = 1;
    g_Latest_RX_Data.Servo2.ID   = 2;
    g_Latest_RX_Data.Servo1.Mode = 0;
    g_Latest_RX_Data.Servo2.Mode = 0;

    /* 上电归中, 等待前后板完成初始化并对齐时序 */
    g_Latest_RX_Data.Servo1.Accel         = 80;
    g_Latest_RX_Data.Servo2.Accel         = 80;
    g_Latest_RX_Data.Servo1.Running_Speed = 500;
    g_Latest_RX_Data.Servo2.Running_Speed = 500;
    g_Latest_RX_Data.Servo1.Target_Pos    = JUMP_POS_CENTER;
    g_Latest_RX_Data.Servo2.Target_Pos    = JUMP_POS_CENTER;
    BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
    HAL_Delay(2000);

    while (1)
    {
        /* ================================================================
         * 阶段 1 — 四腿压到最低 (深蹲保压)
         *   以受控速度一步到位压到最深蹲位, 不分段.
         *   保压 JUMP_HOLD_MS, 确保四条腿都完全到达压缩位后再爆发.
         *   ⚠️ 不用 Speed=0: 防止冲过目标位置反弹, 破坏保压稳定性.
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 100;
        g_Latest_RX_Data.Servo2.Accel         = 100;
        g_Latest_RX_Data.Servo1.Running_Speed = 500;
        g_Latest_RX_Data.Servo2.Running_Speed = 500;

        g_Latest_RX_Data.Servo1.Target_Pos = JUMP_POS_CROUCH_S1;   /* 1748: 2048-300 */
        g_Latest_RX_Data.Servo2.Target_Pos = JUMP_POS_CROUCH_S2;   /* 2348: 2048+300 */
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(JUMP_HOLD_MS);   /* 充分保压, 前后板在此节点完全对齐 */

        /* ================================================================
         * 阶段 2 — 瞬间爆发蹬地
         *   Accel=0 / Speed=0: 驱动器以最大电流阶跃输出, 全力蹬地.
         *   对称蹬地: S1=+400, S2=-400, 合力严格垂直.
         *   冲量窗口 JUMP_IMPULSE_MS: 舵机处于最大加速阶段时地面反力最大.
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 0;
        g_Latest_RX_Data.Servo2.Accel         = 0;
        g_Latest_RX_Data.Servo1.Running_Speed = 0;
        g_Latest_RX_Data.Servo2.Running_Speed = 0;

        g_Latest_RX_Data.Servo1.Target_Pos = JUMP_POS_EXTEND_S1;   /* 2448: 2048+400 */
        g_Latest_RX_Data.Servo2.Target_Pos = JUMP_POS_EXTEND_S2;   /* 1648: 2048-400 */
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(JUMP_IMPULSE_MS);   /* ← 核心调参点 */

        /* ================================================================
         * 阶段 3 — 空中收腿
         *   以最大速度收回深蹲位, 减少空中姿态干扰.
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Target_Pos = JUMP_POS_CROUCH_S1;
        g_Latest_RX_Data.Servo2.Target_Pos = JUMP_POS_CROUCH_S2;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(JUMP_AIRBORNE_MS);

        /* ================================================================
         * 阶段 4 — 落地缓冲
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 150;
        g_Latest_RX_Data.Servo2.Accel         = 150;
        g_Latest_RX_Data.Servo1.Running_Speed = 800;
        g_Latest_RX_Data.Servo2.Running_Speed = 800;

        g_Latest_RX_Data.Servo1.Target_Pos = JUMP_POS_CENTER;
        g_Latest_RX_Data.Servo2.Target_Pos = JUMP_POS_CENTER;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(JUMP_COOLDOWN_MS);

        /* 每跳合计: HOLD+IMPULSE+AIRBORNE+COOLDOWN = 2000+80+320+1500 = 3900 ms */
    }
}


/* =========================================================================
 * 后板垂直起跳任务  (Servo ID 3 & 4)
 *   所有位置值、所有 HAL_Delay 与前板完全一致.
 * ========================================================================= */
void Robot_Rear_Jump_Task(void)
{
    g_Latest_RX_Data.Servo1.ID   = 3;
    g_Latest_RX_Data.Servo2.ID   = 4;
    g_Latest_RX_Data.Servo1.Mode = 0;
    g_Latest_RX_Data.Servo2.Mode = 0;

    g_Latest_RX_Data.Servo1.Accel         = 80;
    g_Latest_RX_Data.Servo2.Accel         = 80;
    g_Latest_RX_Data.Servo1.Running_Speed = 500;
    g_Latest_RX_Data.Servo2.Running_Speed = 500;
    g_Latest_RX_Data.Servo1.Target_Pos    = JUMP_POS_CENTER;
    g_Latest_RX_Data.Servo2.Target_Pos    = JUMP_POS_CENTER;
    BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
    HAL_Delay(2000);

    while (1)
    {
        /* 阶段 1 — 四腿压到最低 */
        g_Latest_RX_Data.Servo1.Accel         = 100;
        g_Latest_RX_Data.Servo2.Accel         = 100;
        g_Latest_RX_Data.Servo1.Running_Speed = 500;
        g_Latest_RX_Data.Servo2.Running_Speed = 500;

        g_Latest_RX_Data.Servo1.Target_Pos = JUMP_POS_CROUCH_S1;
        g_Latest_RX_Data.Servo2.Target_Pos = JUMP_POS_CROUCH_S2;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(JUMP_HOLD_MS);

        /* 阶段 2 — 瞬间爆发蹬地 */
        g_Latest_RX_Data.Servo1.Accel         = 0;
        g_Latest_RX_Data.Servo2.Accel         = 0;
        g_Latest_RX_Data.Servo1.Running_Speed = 0;
        g_Latest_RX_Data.Servo2.Running_Speed = 0;

        g_Latest_RX_Data.Servo1.Target_Pos = JUMP_POS_EXTEND_S1;
        g_Latest_RX_Data.Servo2.Target_Pos = JUMP_POS_EXTEND_S2;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(JUMP_IMPULSE_MS);

        /* 阶段 3 — 空中收腿 */
        g_Latest_RX_Data.Servo1.Target_Pos = JUMP_POS_CROUCH_S1;
        g_Latest_RX_Data.Servo2.Target_Pos = JUMP_POS_CROUCH_S2;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(JUMP_AIRBORNE_MS);

        /* 阶段 4 — 落地缓冲 */
        g_Latest_RX_Data.Servo1.Accel         = 150;
        g_Latest_RX_Data.Servo2.Accel         = 150;
        g_Latest_RX_Data.Servo1.Running_Speed = 800;
        g_Latest_RX_Data.Servo2.Running_Speed = 800;

        g_Latest_RX_Data.Servo1.Target_Pos = JUMP_POS_CENTER;
        g_Latest_RX_Data.Servo2.Target_Pos = JUMP_POS_CENTER;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(JUMP_COOLDOWN_MS);
    }
}
