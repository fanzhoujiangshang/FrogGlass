#include <string.h>
#include "sys.h"
#include "uart_link.h"
#include <stdio.h>
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "print.h"

/********************************************************************************	 
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C)
 * All rights reserved
********************************************************************************/
UART_HandleTypeDef huart3;
uint8_t m_BT_CurrentBuff;
BT_DataStruct m_BT_Struct_A;
BT_DataStruct m_BT_Struct_B;
PrintDataQueue BT_DataHeap;

void BT_Uart3_Init(void)	/*串口初始化*/
{
 
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
	printf("BT_Uart3_Init err \n");	
  }
  SET_BIT(huart3.Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
  //9600		BRR:208d
  //57600	BRR:056d
  //115200	BRR:02b6
  printf("BRR:%04x\r\n",huart3.Instance->BRR);
}

void BT_Uart3_SwitchToFaster(void)	/*串口初始化*/
{
  /* Disable the Peripheral */
  __HAL_UART_DISABLE(&huart3);

  CLEAR_BIT(huart3.Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
  huart3.Instance->BRR = 0x000002b6;
  SET_BIT(huart3.Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
  
  __HAL_UART_ENABLE(&huart3);  
}

u8 BT_Pop(u8* data)
{
	if(BT_DataHeap.m_Head == BT_DataHeap.m_Tail)
	{
		return 0;
	}
	else
	{
		*data = BT_DataHeap.m_Buffer[BT_DataHeap.m_Tail];
		BT_DataHeap.m_Tail++;
		BT_DataHeap.m_Tail %= PRINT_HEAP_SIZE;
		return 1;
	}
}
void BT_Push(u8 data)
{
	BT_DataHeap.m_Buffer[BT_DataHeap.m_Head] = data;
	BT_DataHeap.m_Head++;
	BT_DataHeap.m_Head %= PRINT_HEAP_SIZE;
}

void USR_UART3_RxCpltCallback(void)
{
	u8 tmpData;
	if(READ_BIT(huart3.Instance->ISR, USART_ISR_TC) == (USART_ISR_TC))
	{
		if(BT_Pop(&tmpData))
		{
			huart3.Instance->TDR = tmpData;
		}
		else
		{
			CLEAR_BIT(huart3.Instance->CR1, USART_CR1_TCIE);
		}
	}
	while(READ_BIT(huart3.Instance->ISR, USART_ISR_RXNE) == (USART_ISR_RXNE))
	{
		tmpData = huart3.Instance->RDR;
		if(m_BT_CurrentBuff)
		{
			if(m_BT_Struct_B.m_Index < BT_BUFFER_SIZE - 1)
			{
				m_BT_Struct_B.m_DataBuffer[m_BT_Struct_B.m_Index] = tmpData;
				m_BT_Struct_B.m_Index++;
			}
		}
		else
		{
			if(m_BT_Struct_A.m_Index < BT_BUFFER_SIZE - 1)
			{
				m_BT_Struct_A.m_DataBuffer[m_BT_Struct_A.m_Index] = tmpData;
				m_BT_Struct_A.m_Index++;
			}
		}
		//printf("=%02x",tmpData);
	}
}




void BT_Uart3_Send_Data(uint8_t* data, uint32_t size)
{
	for(int i = 0;i < size;i++)
	{
		BT_Push(data[i]);
//		printf("%c",data[i]);
	}
//	printf("\r\n");
	if(size > 0)
	{
		SET_BIT(huart3.Instance->CR1, USART_CR1_TCIE);
	}
}

pBT_DataStruct get_BT_ReceiveData(void)
{
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_PE);//清标志	
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_FE);	
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_NE);	
	__HAL_UART_CLEAR_FLAG(&huart3, UART_FLAG_ORE);
	if(m_BT_CurrentBuff)		// B
	{
		return &m_BT_Struct_B;
	}
	else
	{
		return &m_BT_Struct_A;
	}
}

void BT_Switch_RecBuffer(pBT_DataStruct pST)
{
	if(pST == &m_BT_Struct_B)	// current buffer is B
	{	// switch to A buffer for data receive
		m_BT_CurrentBuff = 0;
		m_BT_Struct_A.m_Index = 0;
	}
	else
	{
		m_BT_CurrentBuff = 1;
		m_BT_Struct_B.m_Index = 0;
	}
}

