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
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

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
#define DIN3_Pin GPIO_PIN_2
#define DIN3_GPIO_Port GPIOE
#define DIN3_EXTI_IRQn EXTI2_IRQn
#define DIN4_Pin GPIO_PIN_3
#define DIN4_GPIO_Port GPIOE
#define DIN4_EXTI_IRQn EXTI3_IRQn
#define DIN5_Pin GPIO_PIN_4
#define DIN5_GPIO_Port GPIOE
#define DIN5_EXTI_IRQn EXTI4_IRQn
#define DIN6_Pin GPIO_PIN_5
#define DIN6_GPIO_Port GPIOE
#define DIN6_EXTI_IRQn EXTI5_IRQn
#define DIN7_Pin GPIO_PIN_6
#define DIN7_GPIO_Port GPIOE
#define DIN7_EXTI_IRQn EXTI6_IRQn
#define PW_ETH_Pin GPIO_PIN_5
#define PW_ETH_GPIO_Port GPIOF
#define PW_IND_Pin GPIO_PIN_1
#define PW_IND_GPIO_Port GPIOH
#define PW_RS1_Pin GPIO_PIN_2
#define PW_RS1_GPIO_Port GPIOC
#define PW_RS2_Pin GPIO_PIN_3
#define PW_RS2_GPIO_Port GPIOC
#define CTS2_Pin GPIO_PIN_0
#define CTS2_GPIO_Port GPIOA
#define RTS2_Pin GPIO_PIN_1
#define RTS2_GPIO_Port GPIOA
#define RS1SEL_Pin GPIO_PIN_0
#define RS1SEL_GPIO_Port GPIOB
#define RTS3_Pin GPIO_PIN_1
#define RTS3_GPIO_Port GPIOB
#define LED0R_Pin GPIO_PIN_12
#define LED0R_GPIO_Port GPIOF
#define EX2_INT_Pin GPIO_PIN_13
#define EX2_INT_GPIO_Port GPIOF
#define EX2_INT_EXTI_IRQn EXTI13_IRQn
#define LED0G_Pin GPIO_PIN_14
#define LED0G_GPIO_Port GPIOF
#define PW_AUX_Pin GPIO_PIN_15
#define PW_AUX_GPIO_Port GPIOF
#define PW_P1_Pin GPIO_PIN_0
#define PW_P1_GPIO_Port GPIOG
#define RS1HF_Pin GPIO_PIN_1
#define RS1HF_GPIO_Port GPIOG
#define DIN8_Pin GPIO_PIN_7
#define DIN8_GPIO_Port GPIOE
#define DIN8_EXTI_IRQn EXTI7_IRQn
#define PW_RAM_Pin GPIO_PIN_8
#define PW_RAM_GPIO_Port GPIOE
#define RS2HF_Pin GPIO_PIN_9
#define RS2HF_GPIO_Port GPIOE
#define SPI2_SCK_Pin GPIO_PIN_13
#define SPI2_SCK_GPIO_Port GPIOB
#define SPI2_MOSI_Pin GPIO_PIN_15
#define SPI2_MOSI_GPIO_Port GPIOB
#define RS2SEL_Pin GPIO_PIN_10
#define RS2SEL_GPIO_Port GPIOD
#define PW_P0_Pin GPIO_PIN_7
#define PW_P0_GPIO_Port GPIOC
#define FRAM_CS_Pin GPIO_PIN_8
#define FRAM_CS_GPIO_Port GPIOA
#define PW_P2_Pin GPIO_PIN_9
#define PW_P2_GPIO_Port GPIOA
#define USB_INT_Pin GPIO_PIN_10
#define USB_INT_GPIO_Port GPIOA
#define USB_INT_EXTI_IRQn EXTI10_IRQn
#define BEEP_Pin GPIO_PIN_14
#define BEEP_GPIO_Port GPIOG
#define PW_ADC_Pin GPIO_PIN_15
#define PW_ADC_GPIO_Port GPIOG
#define SPI2_CS2_Pin GPIO_PIN_4
#define SPI2_CS2_GPIO_Port GPIOB
#define BOOT_Pin GPIO_PIN_3
#define BOOT_GPIO_Port GPIOH
#define DIN1_Pin GPIO_PIN_0
#define DIN1_GPIO_Port GPIOE
#define DIN1_EXTI_IRQn EXTI0_IRQn
#define DIN2_Pin GPIO_PIN_1
#define DIN2_GPIO_Port GPIOE
#define DIN2_EXTI_IRQn EXTI1_IRQn
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
