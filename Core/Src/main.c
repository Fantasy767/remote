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
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <stdio.h>
#include "app_freertos_task.h"
#include "SI24R1.h"
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
volatile uint8_t g_boot_stage = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static uint8_t HSE_Probe(void);
static void SystemClock_Config_Hsi(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief  探测 8MHz 外部晶振是否起振（HSE 就绪返回 1）
  */
static uint8_t HSE_Probe(void)
{
  uint32_t timeout = 0x20000U;

  __HAL_RCC_HSE_CONFIG(RCC_HSE_ON);
  while ((__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) == RESET) && (--timeout != 0U))
  {
  }
  return (__HAL_RCC_GET_FLAG(RCC_FLAG_HSERDY) != RESET) ? 1U : 0U;
}

/**
  * @brief  HSE 不可用时的降级方案：HSI/2 x 16 = 64MHz
  */
static void SystemClock_Config_Hsi(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI_DIV2;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL16;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }

  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}
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
  /* 最早期的裸串口输出：不经过 printf/HAL 状态机，用于确认串口链路 */
  UART1_RawInit();
  g_boot_stage = 1;
  UART1_RawPuts("\r\n[BOOT] stage1: HAL_Init done, raw USART1 ready\r\n");
  /* USER CODE END Init */

  /* Configure the system clock */
  g_boot_stage = 2;
  if (HSE_Probe() != 0U)
  {
    UART1_RawPuts("[BOOT] HSE(8MHz) ready\r\n");
    SystemClock_Config();
  }
  else
  {
    __HAL_RCC_HSE_CONFIG(RCC_HSE_OFF);
    UART1_RawPuts("[BOOT] HSE(8MHz) FAILED -> fallback HSI 64MHz\r\n");
    SystemClock_Config_Hsi();
  }

  /* USER CODE BEGIN SysInit */
  /* 时钟已改变，按新的 PCLK2 重算 BRR */
  UART1_RawInit();
  g_boot_stage = 3;
  UART1_RawPuts("[BOOT] stage2: clock ok, SYSCLK=");
  UART1_RawPutHex(SystemCoreClock);
  UART1_RawPuts(" PCLK2=");
  UART1_RawPutHex(HAL_RCC_GetPCLK2Freq());
  UART1_RawPuts("\r\n");
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  g_boot_stage = 4;
  MX_GPIO_Init();
  g_boot_stage = 5;
  MX_DMA_Init();
  g_boot_stage = 6;
  MX_USART1_UART_Init();
  g_boot_stage = 7;
  MX_SPI1_Init();
  g_boot_stage = 8;
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  UART1_RawPuts("[BOOT] stage3: peripheral init done\r\n");
  g_boot_stage = 9;

  if (USART1_CheckInit() == HAL_OK)
  {
    UART1_RawPuts("[UART] USART1 init OK, BRR=");
    UART1_RawPutHex(USART1->BRR);
    UART1_RawPuts(" CR1=");
    UART1_RawPutHex(USART1->CR1);
    UART1_RawPuts("\r\n");
  }
  else
  {
    UART1_RawPuts("[UART] USART1 init FAILED, BRR=");
    UART1_RawPutHex(USART1->BRR);
    UART1_RawPuts(" CR1=");
    UART1_RawPutHex(USART1->CR1);
    UART1_RawPuts(" GPIOA_CRH=");
    UART1_RawPutHex(GPIOA->CRH);
    UART1_RawPuts(" state=");
    UART1_RawPutHex((uint32_t)huart1.gState);
    UART1_RawPuts("\r\n");
    Error_Handler();
  }

#if defined(__GNUC__)
  /* 关闭 stdout 缓冲，printf 立即输出（MicroLIB 不支持 setvbuf） */
  setvbuf(stdout, NULL, _IONBF, 0);
#endif
  //使用c语言标准串口
  debug_printf("hello remote\n");
  SI24R1_Init();
  UART1_RawPuts("[BOOT] stage4: SI24R1 init done, start FreeRTOS\r\n");
  //启动rtos，后续的代码就不再执行
  app_freertos_task();

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    /* 调度器返回说明创建任务失败（通常是堆不够） */
    UART1_RawPuts("[FATAL] vTaskStartScheduler() returned, check configTOTAL_HEAP_SIZE\r\n");
    HAL_Delay(1000);
    NVIC_SystemReset();
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  uint32_t caller = (uint32_t)__builtin_return_address(0);
  uint8_t stage = g_boot_stage;

  UART1_RawInit();
  __disable_irq();
  /* 循环重发，便于晚于启动才打开串口助手也能看到原因 */
  while (1)
  {
    UART1_RawPuts("\r\n[FATAL] Error_Handler: init failed, stage=");
    UART1_RawPutChar((char)('0' + (stage / 10U)));
    UART1_RawPutChar((char)('0' + (stage % 10U)));
    UART1_RawPuts(" caller=");
    UART1_RawPutHex(caller);
    UART1_RawPuts("\r\n");
    for (volatile uint32_t i = 0; i < 4000000U; i++)
    {
    }
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
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
