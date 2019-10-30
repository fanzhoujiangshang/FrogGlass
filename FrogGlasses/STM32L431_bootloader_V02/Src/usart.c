/**
  ******************************************************************************
  * File Name          : USART.c
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

/* Includes ------------------------------------------------------------------*/
#include "usart.h"

/* USER CODE BEGIN 0 */
BT_DataStruct m_BT_Struct;
/* USER CODE END 0 */

/* USART1 init function */

void MX_USART1_UART_Init(void)
{
  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Peripheral clock enable */
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_USART1);
  
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  /**USART1 GPIO Configuration  
  PA9   ------> USART1_TX
  PA10   ------> USART1_RX 
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_9|LL_GPIO_PIN_10;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART1 interrupt Init */
  NVIC_SetPriority(USART1_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART1_IRQn);

  USART_InitStruct.BaudRate = 9600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART1, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART1);
  LL_USART_Enable(USART1);

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{
  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART2);
  
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  /**USART2 GPIO Configuration  
  PA2   ------> USART2_TX
  PA3   ------> USART2_RX 
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_2|LL_GPIO_PIN_3;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USART2 interrupt Init */
  NVIC_SetPriority(USART2_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART2_IRQn);

  USART_InitStruct.BaudRate = 115200;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART2, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART2);
  LL_USART_Enable(USART2);

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{
  LL_USART_InitTypeDef USART_InitStruct = {0};

  LL_GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* Peripheral clock enable */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);
  
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
  /**USART3 GPIO Configuration  
  PB10   ------> USART3_TX
  PB11   ------> USART3_RX 
  */
  GPIO_InitStruct.Pin = LL_GPIO_PIN_10|LL_GPIO_PIN_11;
  GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
  GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
  GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
  GPIO_InitStruct.Alternate = LL_GPIO_AF_7;
  LL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* USART3 interrupt Init */
  NVIC_SetPriority(USART3_IRQn, NVIC_EncodePriority(NVIC_GetPriorityGrouping(),0, 0));
  NVIC_EnableIRQ(USART3_IRQn);

  USART_InitStruct.BaudRate = 9600;
  USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
  USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
  USART_InitStruct.Parity = LL_USART_PARITY_NONE;
  USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
  USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
  USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
  LL_USART_Init(USART3, &USART_InitStruct);
  LL_USART_ConfigAsyncMode(USART3);
  LL_USART_Enable(USART3);

}

/* USER CODE BEGIN 1 */
//----------------------------------------------------------usart2
int fputc(int ch, FILE* stream)
{
	//uint8_t temp[1]={ch};
	//HAL_UART_Transmit(&huart2,temp,1,2);
	LL_USART_TransmitData8(USART2,ch);
	while (!LL_USART_IsActiveFlag_TXE(USART2));
	LL_USART_ClearFlag_TC(USART2);
	//LL_USART_TransmitData8(USART1,ch);
	return ch;
}
uint8_t RecData[100];
uint8_t RecIndex = 0;
//uint8_t RecIndex;
void MX_USART2_RecvCallback(void)
{
	uint8_t tmp;
	if(LL_USART_IsActiveFlag_RXNE(USART2))
	{
		tmp = LL_USART_ReceiveData8(USART2);
		if(RecIndex < 100)
		{
			RecData[RecIndex] = tmp;
			RecIndex++;
		}
	}
}
//----------------------------------------------------------usart1
void MX_USART1_SendData(u8* pData,u16 Len)
{
	u16 tmpIndex;
	for(tmpIndex = 0;tmpIndex < Len;tmpIndex++)
	{
		LL_USART_TransmitData8(USART1, pData[tmpIndex]);
		while (!LL_USART_IsActiveFlag_TXE(USART1));
	}
	LL_USART_ClearFlag_TC(USART1);
}

void MX_USART1_RecvCallback(void)
{
	uint8_t tmp;
	if(LL_USART_IsActiveFlag_RXNE(USART1))
	{
		tmp=LL_USART_ReceiveData8(USART1);
		MX_USART1_SendData(&tmp,1);
		//LL_USART_TransmitData8(USART1,tmp);
	}
}
//----------------------------------------------------------usart3 BLE
void BT_USART3_SendData(u8* pData,u16 Len)
{
	BT_BRTS(0);
	LL_mDelay(230);
	MX_USART3_SendData(pData,Len);
	LL_mDelay(5);
	BT_BRTS(1);
}
void MX_USART3_SendData(u8* pData,u16 Len)
{
	u16 tmpIndex;
	for(tmpIndex = 0;tmpIndex < Len;tmpIndex++)
	{
		LL_USART_TransmitData8(USART3, pData[tmpIndex]);
		while (!LL_USART_IsActiveFlag_TXE(USART3));
	}
	LL_USART_ClearFlag_TC(USART3);
}

void MX_USART3_RecvCallback(void)
{
	uint8_t tmp;
	while(LL_USART_IsActiveFlag_RXNE(USART3))
	{
		tmp=LL_USART_ReceiveData8(USART3);
		if(m_BT_Struct.m_Index < BT_BUFFER_SIZE - 1)
		{
			m_BT_Struct.m_DataBuffer[m_BT_Struct.m_Index] = tmp;
			m_BT_Struct.m_Index++;
			LL_USART_TransmitData8(USART2,tmp);
		}
	}
}

pBT_DataStruct get_BT_ReceiveData(void)
{
	return &m_BT_Struct;
}
//----------------------------------------------------------other
void User_USART_EnableINT(void)
{
 /* USART1 interrupt Init */
 	LL_USART_EnableIT_RXNE(USART1);
	//LL_USART_EnableIT_PE(USART1);
 /* USART2 interrupt Init */
	//LL_USART_EnableIT_RXNE(USART2);
 /* USART3 interrupt Init */
	LL_USART_EnableIT_RXNE(USART3);
/* SysTick interrupt enable */
	LL_SYSTICK_EnableIT();
}

void User_Usart_DisableINT(void)
{
 /* USART1 interrupt Init */
 	LL_USART_DisableIT_RXNE(USART1);
	//LL_USART_EnableIT_PE(USART1);
 /* USART2 interrupt Init */
	LL_USART_DisableIT_RXNE(USART2);
 /* USART3 interrupt Init */
	LL_USART_DisableIT_RXNE(USART3);
/* SysTick interrupt enable */
	LL_SYSTICK_DisableIT();
}

void MX_USART1_Test(void)
{
	u8 data[4];
	data[0] = 0x33;
	data[1] = 0x32;
	data[2] = 0x31;
	data[3] = 0x30;
	MX_USART1_SendData(data,4);
}
/* USER CODE END 1 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
