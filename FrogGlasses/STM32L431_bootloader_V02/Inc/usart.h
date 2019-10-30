/**
  ******************************************************************************
  * File Name          : USART.h
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
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
#ifndef __usart_H
#define __usart_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "gpio.h"
/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
#define	DEBUG_PRINTF_EN		(1)
#define	BT_BUFFER_SIZE			(200)
typedef struct
{
	u8 m_Index;
	u8 m_Index_Old;
	u8 m_DataBuffer[BT_BUFFER_SIZE];
}BT_DataStruct,*pBT_DataStruct;
/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
void MX_USART1_Test(void);
void MX_USART1_RecvCallback(void);
void MX_USART1_SendData(u8* pData,u16 Len);
void MX_USART3_RecvCallback(void);
void MX_USART3_SendData(u8* pData,u16 Len);
void BT_USART3_SendData(u8* pData,u16 Len);
void MX_USART2_RecvCallback(void);
pBT_DataStruct get_BT_ReceiveData(void);
void User_USART_EnableINT(void);
void User_Usart_DisableINT(void);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
