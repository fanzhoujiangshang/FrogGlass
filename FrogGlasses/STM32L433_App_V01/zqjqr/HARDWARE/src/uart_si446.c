#include <string.h>
#include "sys.h"
#include "uart_si446.h"
#include <stdio.h>
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "storageTask.h"
/********************************************************************************	 
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C)
 * All rights reserved
********************************************************************************/
#define UART446_DATA_TIMEOUT_MS 	1000
#define UART446_DATA_TIMEOUT_TICKS (UART446_DATA_TIMEOUT_MS / portTICK_RATE_MS)
#define SI446BUFF_LEN 50

UART_HandleTypeDef huart1;
u8 tmpData;

 /**
 * @return FLASE:环形缓冲区已满，写入失败;TRUE:写入成功
 * @note ? 往环形缓冲区写入u8类型的数据
 */
u8 Write_Si446_Send(u8* pdata, u8 size)
{
	for(int i = 0;i < size;i++)
	{
		while(huart1.gState != HAL_UART_STATE_READY);
		if(HAL_UART_Transmit_IT(&huart1,pdata + i,1) != HAL_OK)
		{
			return false;
		}
	}
	return true;
}
void uartSi446Init(void)	/*串口初始化*/
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
	printf("uartSi446Init err \n");	
  }
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&tmpData, 1); 
}


void USR_UART1_RxCpltCallback(void)
{
	while(HAL_UART_Receive_IT(&huart1, (uint8_t *)&tmpData, 1) != HAL_OK)	//一次处理完成之后，重新开启中断并设置RxXferCount为1
	{ 		
	}	
	PushInNewData(tmpData);
}

#define	SI446_LOWPASS_CNT				(2)
#define	SI446_LOOPCNT_MAX			(5)

u8 SI446_Oxygen;
u8 SI446_Power;
u8 SI446_DataBuffer[SI446BUFF_LEN];
u8 SI446_Index;
u8 SI446_Index_old;

bool uartsi446GetOxygen(u8 *Oxygen,u8 * Power)
{
	*Oxygen = SI446_Oxygen;
	*Power = SI446_Power;
	return true;
}
void PushInNewData(u8 data)
{
	if(SI446_Index < SI446BUFF_LEN - 1)
	{
		SI446_DataBuffer[SI446_Index] = data;
		SI446_Index++;
	}
}

void SI446_DataParser(void)
{
	u8 tmpSum = 0;
	u8 tmpDataLen;
	u16 tmpAddr;
	u16 tmpAddrRec;
	if(SI446_Index < SI446_DATA_OFFSET_MAX)
	{
		return;
	}
	if(SI446_DataBuffer[SI446_DATA_OFFSET_HEAD] != SI446_DATA_VALUE_HEAD)
	{
		return;
	}
	for(int i = 0;i < SI446_DATA_OFFSET_MAX - 1;i++)
	{
		tmpSum += SI446_DataBuffer[i];
	}
	if(tmpSum != SI446_DataBuffer[SI446_DATA_OFFSET_CHKSUM])
	{
		return;
	}
	/*
		lookup Address in Flash,If unexist,return;
	*/
	MX25Lxxx_ReadOxygenAddr(&tmpAddr);
	tmpAddrRec = SI446_DataBuffer[SI446_DATA_OFFSET_ADDH];
	tmpAddrRec <<= 8;
	tmpAddrRec |= SI446_DataBuffer[SI446_DATA_OFFSET_ADDL];
	if(tmpAddr == 0x0000)	//0x0000 not parse any data
	{
		return;
	}
	else if((tmpAddr != tmpAddrRec)&&(tmpAddr != 0xffff))	// 0xffff	handle any data received
	{
		return;
	}
	tmpDataLen = SI446_DataBuffer[SI446_DATA_OFFSET_DATALEN];
	switch(SI446_DataBuffer[SI446_DATA_OFFSET_DEVTYPE])
	{
		case SI446_DEV_OXYGEN:
			if(tmpDataLen == 3)
			{
				SI446_Power 	= 		SI446_DataBuffer[SI446_DATA_OFFSET_DATA_PWR];
				SI446_Oxygen 	= 		SI446_DataBuffer[SI446_DATA_OFFSET_DATA_OXYGEN];
			}
			break;
		default:
			break;
	}
}

void SI446_ReceiveDataHandler(void)
{
	__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_PE);//清标志	
	__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_FE);	
	__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_NE);	
	__HAL_UART_CLEAR_FLAG(&huart1, UART_FLAG_ORE);	
	//printf("index:%d-%d\r\n",SI446_Index,SI446_Index_old);
	if(SI446_Index == 0)
	{
		return;
	}
	else if(SI446_Index != SI446_Index_old)
	{
		SI446_Index_old = SI446_Index;
	}
	else
	{
		for(int i = 0;i < SI446_Index;i++)
		{
			printf("%02x-",SI446_DataBuffer[i]);
		}
		SI446_DataParser();		
		//printf("index:%d\r\n",SI446_Index);
		SI446_Index = 0;
		SI446_Index_old = 0;		
	}
}


