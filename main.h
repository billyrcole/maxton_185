/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

#include "hci_tl_interface.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "flashWrite.h"
int __io_putchar(int ch);
void print_backtrace_here();
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

typedef struct {
  uint8_t Buf;
  uint8_t Idx;
} MSGQUEUE_OBJ_t;


/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define PIN_DL_Pin GPIO_PIN_7
#define PIN_DL_GPIO_Port GPIOC
#define STEP_MTR_CK_Pin GPIO_PIN_9
#define STEP_MTR_CK_GPIO_Port GPIOC
#define BLE_RST_Pin GPIO_PIN_8
#define BLE_RST_GPIO_Port GPIOA
#define PIN_DHS_Pin GPIO_PIN_9
#define PIN_DHS_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define TM2_CH2_A_Pin GPIO_PIN_15
#define TM2_CH2_A_GPIO_Port GPIOA
#define STEP_MTR_CS_Pin GPIO_PIN_10
#define STEP_MTR_CS_GPIO_Port GPIOC
#define PIN_UL_Pin GPIO_PIN_4
#define PIN_UL_GPIO_Port GPIOB
#define PIN_UHS_Pin GPIO_PIN_6
#define PIN_UHS_GPIO_Port GPIOB
#define ENCD_INT_Z_Pin GPIO_PIN_8
#define ENCD_INT_Z_GPIO_Port GPIOB
#define ENCD_INT_Z_EXTI_IRQn EXTI9_5_IRQn
#define TM2_CH2_B_Pin GPIO_PIN_9
#define TM2_CH2_B_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

#define MOTORCS_Pin GPIO_PIN_10

//__attribute__ ((section ("<section-name>")))ads_buf[READBACK_LENGTH] = { 1, 2, 3, 4 };

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
