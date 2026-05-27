/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention 
  *
  * Copyright (c) 2026 STMicroelectronics. 
  * All rights reserved. 
  *
  * This software is licensed under terms that can be found in the LICENSE file 
  * in the root directory of this software component. 
  * If no LICENSE file comes with this software, it is provided AS-IS. 
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "fdcan.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "BSP_fdcan.h"
#include "BSP_servo.h"
#include "BSP_foc_openloop.h"
#include "BSP_5600iic.h"
#include "APP_Data.h"
#include "APP_Dance.h"
#include "APP_Jump.h"
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

/* USER CODE BEGIN PFP */
volatile float All_elc_angle;
volatile float open_loop_Vq = 0.0f;       // 1.5V 的拖拽力（对于22欧姆电机足够了）

// 定义联合体，利用内存共享快速转换 float 到 byte 数组
typedef union {
    float fdata[4];       // 存放 4 个浮点数 (共 16 字节)
    uint8_t bytes[20];    // 16字节数据 + 4字节协议帧尾 = 20 字节
} Vofa_Data_t;

Vofa_Data_t vofa_tx;

/**
 * @brief 串口发送函数 采用vofo+协议进行发送
 * @note  请在 main 的 while(1) 中调用，切勿放入定时器中断！
 */
void Verify_Text_VOFA(void)
{
  

    vofa_tx.fdata[0] = All_elc_angle; 
     vofa_tx.fdata[1] = motor1.DutyA;
    vofa_tx.fdata[2] = motor1.DutyB; 
    vofa_tx.fdata[3] = motor1.DutyC;     
    // 塞入 VOFA+ 的 JustFloat 协议帧尾
    vofa_tx.bytes[16] = 0x00;
    vofa_tx.bytes[17] = 0x00;
    vofa_tx.bytes[18] = 0x80;
    vofa_tx.bytes[19] = 0x7F;

    // 半双工串口发送
    //HAL_HalfDuplex_EnableTransmitter(&huart1);
    HAL_UART_Transmit(&huart1, vofa_tx.bytes, 20, 10); 
    //HAL_HalfDuplex_EnableReceiver(&huart1);
}

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
 
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_FDCAN1_Init();
  MX_USART1_UART_Init();
  MX_TIM1_Init();
  //MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  /*!<Hardware Init*/
  User_FDCAN_Init();
 // BSP_Servo_Start_DMA_Rx_Init();
  BSP_FOC_TIM1_Init(); 

  /*!<Data Init*/
  //Front_Board_Data_Init();
  //Back_Board_Data_Init();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
uint8_t step = 0;
  // 设定推力：4249 是你的 ARR 值，0.15f 是 15% 的占空比（如果没力气就改成 0.2f）
  uint16_t pwm_high = (uint16_t)(4249.0f * 0.65f);
  while (1)
  {
  // 纯底层寄存器操作：先关悬空相 -> 赋占空比 -> 开启通电相
      switch(step) {
          case 0: // A通PWM, B通GND, C悬空
              TIM1->CCER &= ~(TIM_CCER_CC3E | TIM_CCER_CC3NE); // 彻底关C相
              TIM1->CCR1 = pwm_high; TIM1->CCR2 = 0;           // A给动力, B接地
              TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE);  // 强开A相
              TIM1->CCER |= (TIM_CCER_CC2E | TIM_CCER_CC2NE);  // 强开B相
              break;
              
          case 1: // A通PWM, C通GND, B悬空
              TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE); // 彻底关B相
              TIM1->CCR1 = pwm_high; TIM1->CCR3 = 0;           // A给动力, C接地
              TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE);  // 强开A相
              TIM1->CCER |= (TIM_CCER_CC3E | TIM_CCER_CC3NE);  // 强开C相
              break;
              
          case 2: // B通PWM, C通GND, A悬空
              TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE); // 彻底关A相
              TIM1->CCR2 = pwm_high; TIM1->CCR3 = 0;           // B给动力, C接地
              TIM1->CCER |= (TIM_CCER_CC2E | TIM_CCER_CC2NE);  // 强开B相
              TIM1->CCER |= (TIM_CCER_CC3E | TIM_CCER_CC3NE);  // 强开C相
              break;
              
          case 3: // B通PWM, A通GND, C悬空
              TIM1->CCER &= ~(TIM_CCER_CC3E | TIM_CCER_CC3NE); // 彻底关C相
              TIM1->CCR2 = pwm_high; TIM1->CCR1 = 0;           // B给动力, A接地
              TIM1->CCER |= (TIM_CCER_CC2E | TIM_CCER_CC2NE);  // 强开B相
              TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE);  // 强开A相
              break;
              
          case 4: // C通PWM, A通GND, B悬空
              TIM1->CCER &= ~(TIM_CCER_CC2E | TIM_CCER_CC2NE); // 彻底关B相
              TIM1->CCR3 = pwm_high; TIM1->CCR1 = 0;           // C给动力, A接地
              TIM1->CCER |= (TIM_CCER_CC3E | TIM_CCER_CC3NE);  // 强开C相
              TIM1->CCER |= (TIM_CCER_CC1E | TIM_CCER_CC1NE);  // 强开A相
              break;
              
          case 5: // C通PWM, B通GND, A悬空
              TIM1->CCER &= ~(TIM_CCER_CC1E | TIM_CCER_CC1NE); // 彻底关A相
              TIM1->CCR3 = pwm_high; TIM1->CCR2 = 0;           // C给动力, B接地
              TIM1->CCER |= (TIM_CCER_CC3E | TIM_CCER_CC3NE);  // 强开C相
              TIM1->CCER |= (TIM_CCER_CC2E | TIM_CCER_CC2NE);  // 强开B相
              break;
      }

      // 切换到下一个状态
      step++;
      if (step > 5) {
          step = 0;
      }

      // 控制转速：30ms 延时比较稳。数字越小转得越快！
      HAL_Delay(30);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV4;
  RCC_OscInitStruct.PLL.PLLN = 85;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2; 
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2; 
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2; 
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK 
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

 // 【
// 定义全局变量供观察

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM1)
    {
	}
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {

  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the furce file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
