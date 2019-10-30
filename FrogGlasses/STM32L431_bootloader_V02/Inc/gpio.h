/**
  ******************************************************************************
  * File Name          : gpio.h
  * Description        : This file contains all the functions prototypes for 
  *                      the gpio  
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __gpio_H
#define __gpio_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define IS_KEY_C_RELEASE()  	LL_GPIO_IsInputPinSet(GPIOB,LL_GPIO_PIN_9)	//¶ÁÈ¡×ó°´¼ü
#define IS_KEY_B_RELEASE() 	LL_GPIO_IsInputPinSet(GPIOA,LL_GPIO_PIN_0)	//¶ÁÈ¡ÓÒ°´¼ü
#define IS_KEY_A_RELEASE()  	LL_GPIO_IsInputPinSet(GPIOC,LL_GPIO_PIN_13)	//¶ÁÈ¡Ò¡¸Ë1°´¼ü

#define POWER_CTRL_PIN  	LL_GPIO_PIN_1
#define MOTOR_PIN  			LL_GPIO_PIN_2
#define BUZZER_PIN  			LL_GPIO_PIN_8
#define BT_BRTS_PIN  		LL_GPIO_PIN_12

#define POWER_CTRL(n)	(n?LL_GPIO_SetOutputPin(GPIOB,POWER_CTRL_PIN):LL_GPIO_ResetOutputPin(GPIOB,POWER_CTRL_PIN))
#define MOTOR(n)	(n?LL_GPIO_SetOutputPin(GPIOB,MOTOR_PIN):LL_GPIO_ResetOutputPin(GPIOB,MOTOR_PIN))
#define BUZZER(n)	(n?LL_GPIO_SetOutputPin(GPIOB,BUZZER_PIN):LL_GPIO_ResetOutputPin(GPIOB,BUZZER_PIN))
#define BT_BRTS(n)	(n?LL_GPIO_SetOutputPin(GPIOA,BT_BRTS_PIN):LL_GPIO_ResetOutputPin(GPIOA,BT_BRTS_PIN))

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */
void User_GPIO_Init_Selfdefine(void);
void User_GPIO_ReadyForStandby(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ pinoutConfig_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
