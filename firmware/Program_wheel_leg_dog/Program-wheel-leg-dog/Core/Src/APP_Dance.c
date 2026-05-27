/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : APP_Dance.c
  * @brief          : 轮足机器狗舞蹈测试程序
  ******************************************************************************
  * @attention
  *
  * 【连杆运动坐标约定】
  *
  *   以前板为例 (后板 ID 3/4 同理):
  *
  *     动作          Servo1 (左前)   Servo2 (右前, 镜像)
  *     ---------     -------------   -------------------
  *     中立站立       2048            2048
  *     微蹲蓄力       1950            2150   (偏移 ±100)
  *     完全伸出       2350            1750   (偏移 ±300)
  *     完全收腿       1750            2350   (偏移 ±300)
  *     左腿独立伸     2350            2048   (仅 Servo1 动)
  *     右腿独立伸     2048            1750   (仅 Servo2 动)
  *     左腿独立收     1750            2048
  *     右腿独立收     2048            2350
  *
  * 【舞蹈编排总览 (5 幕, 前后板 HAL_Delay 累计相同 → 节拍同步)】
  *
  *   幕1  亮相归中   缓慢回到中立站立位          1500 ms
  *   幕2  齐步跳动   四腿同步蹲起弹跳 × 4        3200 ms
  *   幕3  交替踢腿   左右腿轮流伸出踢腿 × 4      4000 ms
  *   幕4  前后波浪   前伸→后伸交替摇摆 × 4       5600 ms
  *   幕5  压轴冲刺   最大速度全程弹跳 × 6        2400 ms
  *   归位             回中立                      1000 ms
  *
  *   单轮合计: 17700 ms ≈ 18 s, 循环播放
  *
  ******************************************************************************
  */

#include "APP_Dance.h"
#include "APP_Data.h"
#include "BSP_servo.h"

/* =========================================================================
 * 前板舞蹈任务
 *   Servo1 → ID 1 (左前腿)
 *   Servo2 → ID 2 (右前腿, 镜像安装)
 * ========================================================================= */
void Robot_Front_Dance_Task(void)
{
    g_Latest_RX_Data.Servo1.ID   = 1;
    g_Latest_RX_Data.Servo2.ID   = 2;
    g_Latest_RX_Data.Servo1.Mode = 0;  /* 位置模式 */
    g_Latest_RX_Data.Servo2.Mode = 0;

    while (1)
    {
        /* ================================================================
         * 幕1 — 亮相归中 (Intro)
         *   缓慢回到中立站立位, 让机器人稳定站好, 营造开场感.
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 80;
        g_Latest_RX_Data.Servo2.Accel         = 80;
        g_Latest_RX_Data.Servo1.Running_Speed = 400;
        g_Latest_RX_Data.Servo2.Running_Speed = 400;

        g_Latest_RX_Data.Servo1.Target_Pos = 2048;   /* 中立 */
        g_Latest_RX_Data.Servo2.Target_Pos = 2048;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(1500);

        /* ================================================================
         * 幕2 — 齐步跳动 (Sync Bounce) × 4
         *   两腿同步蹲下再弹起, 机器人整体上下律动.
         *   ▸ 蹲下: 两腿微收蓄力 → 完全伸出弹跳
         *   ▸ 收腿: 两腿完全收起模拟滞空
         *   ▸ 落地: 回中立位缓冲
         * ================================================================ */
        for (uint8_t i = 0; i < 4; i++)
        {
            /* --- 蹲下蓄力 --- */
            g_Latest_RX_Data.Servo1.Accel         = 100;
            g_Latest_RX_Data.Servo2.Accel         = 100;
            g_Latest_RX_Data.Servo1.Running_Speed = 500;
            g_Latest_RX_Data.Servo2.Running_Speed = 500;

            g_Latest_RX_Data.Servo1.Target_Pos = 1950;   /* 微收蓄力 */
            g_Latest_RX_Data.Servo2.Target_Pos = 2150;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(300);

            /* --- 爆发伸出 --- */
            g_Latest_RX_Data.Servo1.Accel         = 0;   /* 阶跃响应, 最大冲击力 */
            g_Latest_RX_Data.Servo2.Accel         = 0;
            g_Latest_RX_Data.Servo1.Running_Speed = 0;   /* 最大转速 */
            g_Latest_RX_Data.Servo2.Running_Speed = 0;

            g_Latest_RX_Data.Servo1.Target_Pos = 2350;   /* 完全伸出 */
            g_Latest_RX_Data.Servo2.Target_Pos = 1750;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(100);

            /* --- 空中收腿 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 1750;   /* 完全收起 */
            g_Latest_RX_Data.Servo2.Target_Pos = 2350;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);

            /* --- 落地缓冲 --- */
            g_Latest_RX_Data.Servo1.Accel         = 150;
            g_Latest_RX_Data.Servo2.Accel         = 150;
            g_Latest_RX_Data.Servo1.Running_Speed = 600;
            g_Latest_RX_Data.Servo2.Running_Speed = 600;

            g_Latest_RX_Data.Servo1.Target_Pos = 2048;   /* 回中立 */
            g_Latest_RX_Data.Servo2.Target_Pos = 2048;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);  /* 每轮合计 800 ms */
        }
        /* 幕2 合计: 4 × 800 = 3200 ms */

        /* ================================================================
         * 幕3 — 交替踢腿 (Alternating Kicks) × 4
         *   左右腿轮流独立伸出, 类似踢踏舞步.
         *   与后板相位一致: 两板同时踢左腿, 再同时踢右腿.
         *   ▸ 踢腿时对侧腿保持中立, 呈现单腿发力感
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 0;   /* 踢腿要脆, 阶跃响应 */
        g_Latest_RX_Data.Servo2.Accel         = 0;
        g_Latest_RX_Data.Servo1.Running_Speed = 0;
        g_Latest_RX_Data.Servo2.Running_Speed = 0;

        for (uint8_t i = 0; i < 4; i++)
        {
            /* --- 踢左腿 (仅 Servo1 伸出, Servo2 留中立) --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2400;   /* 左腿大幅伸出 */
            g_Latest_RX_Data.Servo2.Target_Pos = 2048;   /* 右腿原地不动 */
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(400);

            /* --- 左腿收回, 同时踢右腿 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2048;   /* 左腿回中 */
            g_Latest_RX_Data.Servo2.Target_Pos = 1650;   /* 右腿大幅伸出 (镜像方向减小) */
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(400);

            /* --- 右腿收回, 短暂归中 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2048;
            g_Latest_RX_Data.Servo2.Target_Pos = 2048;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);  /* 每轮合计 1000 ms */
        }
        /* 幕3 合计: 4 × 1000 = 4000 ms */

        /* ================================================================
         * 幕4 — 前后波浪 (Front-Rear Wave) × 4
         *   前板与后板反相运动: 前板腿伸出时后板腿同步收起, 交替摇摆.
         *   视觉效果: 机器人整体产生前俯后仰的波浪感.
         *
         *   前板此幕动作:
         *     Phase A: 两腿完全伸出 (与后板收腿同步)
         *     Phase B: 两腿完全收起 (与后板伸腿同步)
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 60;
        g_Latest_RX_Data.Servo2.Accel         = 60;
        g_Latest_RX_Data.Servo1.Running_Speed = 800;
        g_Latest_RX_Data.Servo2.Running_Speed = 800;

        for (uint8_t i = 0; i < 4; i++)
        {
            /* --- Phase A: 前板腿伸出 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2350;   /* 伸出 */
            g_Latest_RX_Data.Servo2.Target_Pos = 1750;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(700);

            /* --- Phase B: 前板腿收起 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 1750;   /* 收起 */
            g_Latest_RX_Data.Servo2.Target_Pos = 2350;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(700);  /* 每轮合计 1400 ms */
        }
        /* 幕4 合计: 4 × 1400 = 5600 ms */

        /* ================================================================
         * 幕5 — 压轴冲刺 (Finale Rush) × 6
         *   最大速度、最大加速度, 全程高频弹跳.
         *   节奏最密集的一幕, 用于收尾制造高潮.
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 0;   /* 阶跃, 最大冲击 */
        g_Latest_RX_Data.Servo2.Accel         = 0;
        g_Latest_RX_Data.Servo1.Running_Speed = 0;   /* 不限速 */
        g_Latest_RX_Data.Servo2.Running_Speed = 0;

        for (uint8_t i = 0; i < 6; i++)
        {
            /* --- 猛地伸出 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2350;
            g_Latest_RX_Data.Servo2.Target_Pos = 1750;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);

            /* --- 猛地收起 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 1750;
            g_Latest_RX_Data.Servo2.Target_Pos = 2350;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);  /* 每轮合计 400 ms */
        }
        /* 幕5 合计: 6 × 400 = 2400 ms */

        /* ================================================================
         * 归位 — 谢幕
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 80;
        g_Latest_RX_Data.Servo2.Accel         = 80;
        g_Latest_RX_Data.Servo1.Running_Speed = 400;
        g_Latest_RX_Data.Servo2.Running_Speed = 400;

        g_Latest_RX_Data.Servo1.Target_Pos = 2048;
        g_Latest_RX_Data.Servo2.Target_Pos = 2048;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(1000);

        /* 单轮合计: 1500+3200+4000+5600+2400+1000 = 17700 ms, 循环 */
    }
}


/* =========================================================================
 * 后板舞蹈任务
 *   Servo1 → ID 3 (左后腿)
 *   Servo2 → ID 4 (右后腿, 镜像安装)
 *
 *   时序与前板完全对齐.
 *   幕4 波浪段相位与前板相反: 前板伸出时后板收腿, 产生整体俯仰感.
 * ========================================================================= */
void Robot_Rear_Dance_Task(void)
{
    g_Latest_RX_Data.Servo1.ID   = 3;
    g_Latest_RX_Data.Servo2.ID   = 4;
    g_Latest_RX_Data.Servo1.Mode = 0;
    g_Latest_RX_Data.Servo2.Mode = 0;

    while (1)
    {
        /* ================================================================
         * 幕1 — 亮相归中 (Intro)
         *   与前板时序完全对齐.
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 80;
        g_Latest_RX_Data.Servo2.Accel         = 80;
        g_Latest_RX_Data.Servo1.Running_Speed = 400;
        g_Latest_RX_Data.Servo2.Running_Speed = 400;

        g_Latest_RX_Data.Servo1.Target_Pos = 2048;
        g_Latest_RX_Data.Servo2.Target_Pos = 2048;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(1500);

        /* ================================================================
         * 幕2 — 齐步跳动 (Sync Bounce) × 4
         *   与前板同相: 四腿同时蹲起, 机器人整体律动.
         * ================================================================ */
        for (uint8_t i = 0; i < 4; i++)
        {
            /* --- 蹲下蓄力 --- */
            g_Latest_RX_Data.Servo1.Accel         = 100;
            g_Latest_RX_Data.Servo2.Accel         = 100;
            g_Latest_RX_Data.Servo1.Running_Speed = 500;
            g_Latest_RX_Data.Servo2.Running_Speed = 500;

            g_Latest_RX_Data.Servo1.Target_Pos = 1950;
            g_Latest_RX_Data.Servo2.Target_Pos = 2150;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(300);

            /* --- 爆发伸出 --- */
            g_Latest_RX_Data.Servo1.Accel         = 0;
            g_Latest_RX_Data.Servo2.Accel         = 0;
            g_Latest_RX_Data.Servo1.Running_Speed = 0;
            g_Latest_RX_Data.Servo2.Running_Speed = 0;

            g_Latest_RX_Data.Servo1.Target_Pos = 2350;
            g_Latest_RX_Data.Servo2.Target_Pos = 1750;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(100);

            /* --- 空中收腿 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 1750;
            g_Latest_RX_Data.Servo2.Target_Pos = 2350;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);

            /* --- 落地缓冲 --- */
            g_Latest_RX_Data.Servo1.Accel         = 150;
            g_Latest_RX_Data.Servo2.Accel         = 150;
            g_Latest_RX_Data.Servo1.Running_Speed = 600;
            g_Latest_RX_Data.Servo2.Running_Speed = 600;

            g_Latest_RX_Data.Servo1.Target_Pos = 2048;
            g_Latest_RX_Data.Servo2.Target_Pos = 2048;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);  /* 每轮合计 800 ms */
        }
        /* 幕2 合计: 3200 ms */

        /* ================================================================
         * 幕3 — 交替踢腿 (Alternating Kicks) × 4
         *   与前板同相: 两板同时踢左腿, 再同时踢右腿.
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 0;
        g_Latest_RX_Data.Servo2.Accel         = 0;
        g_Latest_RX_Data.Servo1.Running_Speed = 0;
        g_Latest_RX_Data.Servo2.Running_Speed = 0;

        for (uint8_t i = 0; i < 4; i++)
        {
            /* --- 踢左腿 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2400;
            g_Latest_RX_Data.Servo2.Target_Pos = 2048;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(400);

            /* --- 踢右腿 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2048;
            g_Latest_RX_Data.Servo2.Target_Pos = 1650;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(400);

            /* --- 归中 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2048;
            g_Latest_RX_Data.Servo2.Target_Pos = 2048;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);  /* 每轮合计 1000 ms */
        }
        /* 幕3 合计: 4000 ms */

        /* ================================================================
         * 幕4 — 前后波浪 (Front-Rear Wave) × 4
         *   ⚠️ 后板与前板相位相反:
         *     Phase A: 后板腿收起 (同时前板腿伸出)  → 机器人前俯
         *     Phase B: 后板腿伸出 (同时前板腿收起)  → 机器人后仰
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 60;
        g_Latest_RX_Data.Servo2.Accel         = 60;
        g_Latest_RX_Data.Servo1.Running_Speed = 800;
        g_Latest_RX_Data.Servo2.Running_Speed = 800;

        for (uint8_t i = 0; i < 4; i++)
        {
            /* --- Phase A: 后板腿收起 (前板同时伸出) --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 1750;   /* 收起 */
            g_Latest_RX_Data.Servo2.Target_Pos = 2350;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(700);

            /* --- Phase B: 后板腿伸出 (前板同时收起) --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2350;   /* 伸出 */
            g_Latest_RX_Data.Servo2.Target_Pos = 1750;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(700);  /* 每轮合计 1400 ms */
        }
        /* 幕4 合计: 5600 ms */

        /* ================================================================
         * 幕5 — 压轴冲刺 (Finale Rush) × 6
         *   与前板同相同步: 四腿同时全速弹跳.
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 0;
        g_Latest_RX_Data.Servo2.Accel         = 0;
        g_Latest_RX_Data.Servo1.Running_Speed = 0;
        g_Latest_RX_Data.Servo2.Running_Speed = 0;

        for (uint8_t i = 0; i < 6; i++)
        {
            /* --- 猛地伸出 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 2350;
            g_Latest_RX_Data.Servo2.Target_Pos = 1750;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);

            /* --- 猛地收起 --- */
            g_Latest_RX_Data.Servo1.Target_Pos = 1750;
            g_Latest_RX_Data.Servo2.Target_Pos = 2350;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

            HAL_Delay(200);  /* 每轮合计 400 ms */
        }
        /* 幕5 合计: 2400 ms */

        /* ================================================================
         * 归位 — 谢幕
         * ================================================================ */
        g_Latest_RX_Data.Servo1.Accel         = 80;
        g_Latest_RX_Data.Servo2.Accel         = 80;
        g_Latest_RX_Data.Servo1.Running_Speed = 400;
        g_Latest_RX_Data.Servo2.Running_Speed = 400;

        g_Latest_RX_Data.Servo1.Target_Pos = 2048;
        g_Latest_RX_Data.Servo2.Target_Pos = 2048;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);

        HAL_Delay(1000);

        /* 单轮合计: 1500+3200+4000+5600+2400+1000 = 17700 ms, 循环 */
    }
}
