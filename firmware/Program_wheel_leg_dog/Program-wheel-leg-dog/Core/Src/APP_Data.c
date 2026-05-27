/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : APP_Data.c
  * @brief          : 全局结构体实例化区域
  ******************************************************************************
  * @attention
  *
  *
  *
  *
  ******************************************************************************    
  */
 #include "APP_Data.h"
 #include "BSP_servo.h"
 #include  "string.h"
/*——————————————————————————————————————————————————————————————————————————————*/

static uint8_t Recive_Flag_status = HAL_Recive_staus_OFF; /*!<set total recvie status*/
/*—————————————————————————————————————————全局结构体实例化区域—————————————————————————————*/


 /* 全局变量：保存来自上位机（香橙派）的最新的控制指令 */ 
 FDCAN_Data_RX_Buf_t g_Latest_RX_Data;      /*!<实例化FDcan下行总数据包>*/ 
 FDCAN_Data_TX_Buf_t g_Latest_TX_Data;      /*!<实例化FDcan上行总数据包>*/ 


 /**
  * @brief 板子状态位置初始化函数 
  * @note   
  */
void Front_Board_Data_Init()
{ 
  /*！<servo init */
  memset(&g_Latest_RX_Data, 0, sizeof(g_Latest_RX_Data)); 
  g_Latest_RX_Data.Servo1.Target_Pos = 2048;
  g_Latest_RX_Data.Servo2.Target_Pos = 2048;
  g_Latest_RX_Data.Servo1.ID = 1;
  g_Latest_RX_Data.Servo2.ID = 2;
}

void Back_Board_Data_Init()
{ 
  /*！<servo init */
  memset(&g_Latest_RX_Data, 0, sizeof(g_Latest_RX_Data)); 
  g_Latest_RX_Data.Servo1.Target_Pos = 2048;
  g_Latest_RX_Data.Servo2.Target_Pos = 2048;
  g_Latest_RX_Data.Servo1.ID = 3;
  g_Latest_RX_Data.Servo2.ID = 4;
}

 /**
  * @brief  数据反馈函数 
  * @note   
  */
void Board_Data_Recall_Task()
{
  if(Recive_Flag_status == HAL_Recive_staus_ON)
  {
    Recive_Flag_status = HAL_Recive_staus_OFF;

    /*!<servo back Data shift*/

    /*!<BLDC cmd Data shift*/

  }
    
}

 /**
  * @brief  指令下发函数
  * @note   
  */
 void Board_Main_Cmd_Task()
 {

  /*!<BLDC cmd Data shift*/
  
  /*!<seavor cmd Data shift*/
  
  BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1,&g_Latest_RX_Data.Servo2);/*!< fill data directly*/
	HAL_Delay(5);
 }


 /**
  * @brief  TEST
  * @note   
  */

void Robot_Front_Full_Test_Sequence(void)
{
    g_Latest_RX_Data.Servo1.ID = 1;
    g_Latest_RX_Data.Servo2.ID = 2;
    g_Latest_RX_Data.Servo1.Mode = 0;
    g_Latest_RX_Data.Servo2.Mode = 0;

    // ==========================================
    // 0. 初始状态：复位站立
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
    g_Latest_RX_Data.Servo1.Running_Speed = 800; g_Latest_RX_Data.Servo2.Running_Speed = 800;
    g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2048;
    BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
    HAL_Delay(1500);

    // ==========================================
    // 1. 两次起跳 (Jumps x2)
    // ==========================================
    for(int j=0; j<2; j++) {
        // 蓄力
        g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
        g_Latest_RX_Data.Servo1.Running_Speed = 600; g_Latest_RX_Data.Servo2.Running_Speed = 600;
        g_Latest_RX_Data.Servo1.Target_Pos = 1850; g_Latest_RX_Data.Servo2.Target_Pos = 2250;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1500);
        // 爆发
        g_Latest_RX_Data.Servo1.Accel = 0; g_Latest_RX_Data.Servo2.Accel = 0;
        g_Latest_RX_Data.Servo1.Running_Speed = 0; g_Latest_RX_Data.Servo2.Running_Speed = 0;
        g_Latest_RX_Data.Servo1.Target_Pos = 2800; g_Latest_RX_Data.Servo2.Target_Pos = 1300;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(80); // 冲量窗口
        // 空中收腿
        g_Latest_RX_Data.Servo1.Target_Pos = 1900; g_Latest_RX_Data.Servo2.Target_Pos = 2200;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(250);
        // 软着陆
        g_Latest_RX_Data.Servo1.Accel = 200; g_Latest_RX_Data.Servo2.Accel = 200;
        g_Latest_RX_Data.Servo1.Running_Speed = 1500; g_Latest_RX_Data.Servo2.Running_Speed = 1500;
        g_Latest_RX_Data.Servo1.Target_Pos = 1850; g_Latest_RX_Data.Servo2.Target_Pos = 2250;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1000);
        // 回归中位，准备下次
        g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
        g_Latest_RX_Data.Servo1.Running_Speed = 800; g_Latest_RX_Data.Servo2.Running_Speed = 800;
        g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2048;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1000);
    }

    // ==========================================
    // 2. 两次左右摇摆 (Sway x2)
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
    g_Latest_RX_Data.Servo1.Running_Speed = 600; g_Latest_RX_Data.Servo2.Running_Speed = 600;
    for(int i=0; i<2; i++) {
        g_Latest_RX_Data.Servo1.Target_Pos = 2250; g_Latest_RX_Data.Servo2.Target_Pos = 2250;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(600);
        g_Latest_RX_Data.Servo1.Target_Pos = 1850; g_Latest_RX_Data.Servo2.Target_Pos = 1850;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(600);
    }

    // ==========================================
    // 3. 两次流水波浪 (Wave x2)
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 40; g_Latest_RX_Data.Servo2.Accel = 40;
    g_Latest_RX_Data.Servo1.Running_Speed = 0; g_Latest_RX_Data.Servo2.Running_Speed = 0;
    const uint16_t wave[4] = {2200, 2048, 1896, 2048};
    for(int i=0; i<8; i++) { // 8个步长等于2个完整周期
        g_Latest_RX_Data.Servo1.Target_Pos = wave[i % 4];
        g_Latest_RX_Data.Servo2.Target_Pos = wave[(i + 1) % 4];
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(250);
    }

    // ==========================================
    // 4. 两次踢踏舞序列 (Tap Dance x2)
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 0; g_Latest_RX_Data.Servo2.Accel = 0; // 零延迟清脆感
    g_Latest_RX_Data.Servo1.Running_Speed = 0; g_Latest_RX_Data.Servo2.Running_Speed = 0;
    for(int set=0; set<2; set++) {
        for(int i=0; i<4; i++) { // 每组连踢4下
            g_Latest_RX_Data.Servo1.Target_Pos = 1950; g_Latest_RX_Data.Servo2.Target_Pos = 2048;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
            HAL_Delay(120);
            g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2150;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
            HAL_Delay(120);
        }
        HAL_Delay(300); // 组间停顿
    }
		

    // ==========================================
    // 5. 两次缓慢蹲起 (Slow Squat x2)
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 150; g_Latest_RX_Data.Servo2.Accel = 150;
    g_Latest_RX_Data.Servo1.Running_Speed = 400; g_Latest_RX_Data.Servo2.Running_Speed = 400;
    for(int i=0; i<2; i++) {
        g_Latest_RX_Data.Servo1.Target_Pos = 1850; g_Latest_RX_Data.Servo2.Target_Pos = 2250;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1500);
        g_Latest_RX_Data.Servo1.Target_Pos = 2300; g_Latest_RX_Data.Servo2.Target_Pos = 1796;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1500);
    }

    // ==========================================
    // 6. 两次俯卧撑动作 (Push-ups x2)
    // 前板下压，后板不动
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
    g_Latest_RX_Data.Servo1.Running_Speed = 600; g_Latest_RX_Data.Servo2.Running_Speed = 600;
    for(int i=0; i<2; i++) {
        g_Latest_RX_Data.Servo1.Target_Pos = 1850; g_Latest_RX_Data.Servo2.Target_Pos = 2250; // 下压
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1000);
        g_Latest_RX_Data.Servo1.Target_Pos = 2300; g_Latest_RX_Data.Servo2.Target_Pos = 1796; // 撑起
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1000);
    }

    // 结束序列：回归中位待机
    g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2048;
    BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
    HAL_Delay(1000);
}



void Robot_Back_Full_Test_Sequence(void)
{
    g_Latest_RX_Data.Servo1.ID = 3;
    g_Latest_RX_Data.Servo2.ID = 4;
    g_Latest_RX_Data.Servo1.Mode = 0;
    g_Latest_RX_Data.Servo2.Mode = 0;

    // ==========================================
    // 0. 初始状态：复位站立
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
    g_Latest_RX_Data.Servo1.Running_Speed = 800; g_Latest_RX_Data.Servo2.Running_Speed = 800;
    g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2048;
    BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
    HAL_Delay(1500);

    // ==========================================
    // 1. 两次起跳
    // ==========================================
    for(int j=0; j<2; j++) {
        g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
        g_Latest_RX_Data.Servo1.Running_Speed = 600; g_Latest_RX_Data.Servo2.Running_Speed = 600;
        g_Latest_RX_Data.Servo1.Target_Pos = 2250; // 3号对应2号
        g_Latest_RX_Data.Servo2.Target_Pos = 1850; // 4号对应1号
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1500);
        
        g_Latest_RX_Data.Servo1.Accel = 0; g_Latest_RX_Data.Servo2.Accel = 0;
        g_Latest_RX_Data.Servo1.Running_Speed = 0; g_Latest_RX_Data.Servo2.Running_Speed = 0;
        g_Latest_RX_Data.Servo1.Target_Pos = 1300; 
        g_Latest_RX_Data.Servo2.Target_Pos = 2800; 
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(80); 
        
        g_Latest_RX_Data.Servo1.Target_Pos = 2200; 
        g_Latest_RX_Data.Servo2.Target_Pos = 1900; 
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(250);
        
        g_Latest_RX_Data.Servo1.Accel = 200; g_Latest_RX_Data.Servo2.Accel = 200;
        g_Latest_RX_Data.Servo1.Running_Speed = 1500; g_Latest_RX_Data.Servo2.Running_Speed = 1500;
        g_Latest_RX_Data.Servo1.Target_Pos = 2250; 
        g_Latest_RX_Data.Servo2.Target_Pos = 1850; 
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1000);
        
        g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
        g_Latest_RX_Data.Servo1.Running_Speed = 800; g_Latest_RX_Data.Servo2.Running_Speed = 800;
        g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2048;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1000);
    }

    // ==========================================
    // 2. 两次左右摇摆
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
    g_Latest_RX_Data.Servo1.Running_Speed = 600; g_Latest_RX_Data.Servo2.Running_Speed = 600;
    for(int i=0; i<2; i++) {
        g_Latest_RX_Data.Servo1.Target_Pos = 2250; // 3号
        g_Latest_RX_Data.Servo2.Target_Pos = 2250; // 4号
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(600);
        g_Latest_RX_Data.Servo1.Target_Pos = 1850; 
        g_Latest_RX_Data.Servo2.Target_Pos = 1850; 
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(600);
    }

    // ==========================================
    // 3. 两次流水波浪 
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 40; g_Latest_RX_Data.Servo2.Accel = 40;
    g_Latest_RX_Data.Servo1.Running_Speed = 0; g_Latest_RX_Data.Servo2.Running_Speed = 0;
    const uint16_t wave[4] = {2200, 2048, 1896, 2048};
    for(int i=0; i<8; i++) {
        g_Latest_RX_Data.Servo2.Target_Pos = wave[(i + 2) % 4]; // 4号滞后180度
        g_Latest_RX_Data.Servo1.Target_Pos = wave[(i + 3) % 4]; // 3号滞后270度
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(250);
    }

    // ==========================================
    // 4. 两次踢踏舞序列
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 0; g_Latest_RX_Data.Servo2.Accel = 0;
    g_Latest_RX_Data.Servo1.Running_Speed = 0; g_Latest_RX_Data.Servo2.Running_Speed = 0;
    for(int set=0; set<2; set++) {
        for(int i=0; i<4; i++) {
            g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 1950;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
            HAL_Delay(120);
            g_Latest_RX_Data.Servo1.Target_Pos = 2150; g_Latest_RX_Data.Servo2.Target_Pos = 2048;
            BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
            HAL_Delay(120);
        }
        HAL_Delay(300);
    }

		
		
    // ==========================================
    // 5. 两次缓慢蹲起
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 150; g_Latest_RX_Data.Servo2.Accel = 150;
    g_Latest_RX_Data.Servo1.Running_Speed = 400; g_Latest_RX_Data.Servo2.Running_Speed = 400;
    for(int i=0; i<2; i++) {
        g_Latest_RX_Data.Servo1.Target_Pos = 2250; g_Latest_RX_Data.Servo2.Target_Pos = 1850;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1500);
        g_Latest_RX_Data.Servo1.Target_Pos = 1796; g_Latest_RX_Data.Servo2.Target_Pos = 2300;
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1500);
    }

    // ==========================================
    // 6. 两次俯卧撑动作 (后板配合策略)
    // 俯卧撑时，后退需要保持站立或微调姿态，绝不能乱动！
    // ==========================================
    g_Latest_RX_Data.Servo1.Accel = 80; g_Latest_RX_Data.Servo2.Accel = 80;
    g_Latest_RX_Data.Servo1.Running_Speed = 600; g_Latest_RX_Data.Servo2.Running_Speed = 600;
    for(int i=0; i<2; i++) {
        // 配合前板下压，后板保持中位
        g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2048; 
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1000);
        // 配合前板撑起，后板保持中位
        g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2048; 
        BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
        HAL_Delay(1000);
    }

    // 结束序列
    g_Latest_RX_Data.Servo1.Target_Pos = 2048; g_Latest_RX_Data.Servo2.Target_Pos = 2048;
    BSP_Servo_Sync_Write_2(&g_Latest_RX_Data.Servo1, &g_Latest_RX_Data.Servo2);
    HAL_Delay(1000);
}

// ==========================================
// 2. 完整执行序列 (Sequence) - 前板
// ==========================================


 /**
  * @brief  前板主函数
  * @note   
  */
void Robot_Front_Main_Sequence(void) 
{
  //Robot_Front_Full_Test_Sequence();
	
	
}


// ==========================================
// 2. 完整执行序列 (Sequence) - 后板
// ==========================================
 /**
  * @brief  后板主函数
  * @note   
  */
void Robot_Back_Main_Sequence(void)
{
  //Robot_Back_Full_Test_Sequence();
	
	//Board_Main_Cmd_Task();
}