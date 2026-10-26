/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

UART_HandleTypeDef huart1;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */
/**
  * @brief  按 16 倍过采样计算 BRR（算法与 HAL 一致）
  */
static uint32_t UART1_CalcBRR(uint32_t pclk, uint32_t baud)
{
  uint32_t div = (pclk * 25U) / (4U * baud);      /* 采样时钟数 x100 */
  return ((div / 100U) << 4U) | ((((div % 100U) * 16U) + 50U) / 100U);
}

/**
  * @brief  最小化的 USART1 初始化（PA9=TX, PA10=RX, 8N1, 115200）
  *         直接操作寄存器，不依赖 HAL 状态与 printf，任何时候都能输出
  */
void UART1_RawInit(void)
{
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_USART1_CLK_ENABLE();

  /* PA9 = 复用推挽 50MHz(0xB)，PA10 = 浮空输入(0x4) */
  GPIOA->CRH = (GPIOA->CRH & ~(0xFFUL << 4)) | (0xBUL << 4) | (0x4UL << 8);

  USART1->CR1 = 0;
  USART1->CR2 = 0;
  USART1->CR3 = 0;
  USART1->BRR = UART1_CalcBRR(HAL_RCC_GetPCLK2Freq(), 115200U);
  USART1->CR1 = USART_CR1_UE | USART_CR1_TE | USART_CR1_RE;
}

void UART1_RawPutChar(char c)
{
  uint32_t guard = 1000000U;
  while (((USART1->SR & USART_SR_TXE) == 0U) && (--guard != 0U))
  {
  }
  USART1->DR = (uint32_t)(uint8_t)c;
}

void UART1_RawPuts(const char *s)
{
  while (*s != '\0')
  {
    UART1_RawPutChar(*s++);
  }
}

void UART1_RawPutHex(uint32_t value)
{
  static const char hex[] = "0123456789ABCDEF";
  UART1_RawPuts("0x");
  for (int32_t i = 28; i >= 0; i -= 4)
  {
    UART1_RawPutChar(hex[(value >> i) & 0xFU]);
  }
}

/**
  * @brief  检查 USART1 是否真正初始化成功（时钟、引脚、控制寄存器、波特率）
  * @retval HAL_OK: 初始化成功, HAL_ERROR: 初始化失败
  */
HAL_StatusTypeDef USART1_CheckInit(void)
{
  uint32_t usartdiv;
  uint32_t expected_brr;

  if (huart1.Instance != USART1)
  {
    return HAL_ERROR;
  }
  if (__HAL_RCC_USART1_IS_CLK_ENABLED() == 0U)
  {
    return HAL_ERROR;
  }
  if (huart1.gState != HAL_UART_STATE_READY)
  {
    return HAL_ERROR;
  }

  /* UE/TE/RE 必须全部置位 */
  if ((USART1->CR1 & (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE)) !=
      (USART_CR1_UE | USART_CR1_TE | USART_CR1_RE))
  {
    return HAL_ERROR;
  }

  /* PA9 = 复用推挽输出 50MHz(0xB)，PA10 = 浮空输入(0x4) */
  if (((GPIOA->CRH >> GPIO_CRH_MODE9_Pos) & 0x0FU) != 0x0BU)
  {
    return HAL_ERROR;
  }
  if (((GPIOA->CRH >> GPIO_CRH_MODE10_Pos) & 0x0FU) != 0x04U)
  {
    return HAL_ERROR;
  }

  /* BRR 必须与 PCLK2、波特率匹配（16 倍过采样） */
  usartdiv = (HAL_RCC_GetPCLK2Freq() + (huart1.Init.BaudRate / 2U)) / huart1.Init.BaudRate;
  expected_brr = ((usartdiv / 16U) << 4U) | (usartdiv % 16U);
  if (USART1->BRR != expected_brr)
  {
    return HAL_ERROR;
  }

  return HAL_OK;
}

/* USER CODE END 1 */
