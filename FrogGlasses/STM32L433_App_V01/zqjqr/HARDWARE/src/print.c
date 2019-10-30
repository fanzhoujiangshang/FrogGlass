#include "print.h"
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"
#include "math.h"
//#include "arm_math.h"

UART_HandleTypeDef huart2;

#define	PRINT_CTRL_GPS					(0)		//	0 print  1 ctrl gps
#define	LOW_LEVEL_RW						(1)
#define	GPS_DATABUFFER_SIZE				(100)
uint8_t GPS_ReceiveBuffer[GPS_DATABUFFER_SIZE];
uint8_t GPS_RecDataIndex = 0;
uint8_t GPS_RecDataIndex_Old = 0;
PrintDataQueue Print_DataHeap;

#define	GPS_ONLY_GPS_ST			"$PMTK353,1,0,0,0,0*2A\r\n"

#define	GPS_PERIODIC_STB_0		"$PMTK225,0*2B\r\n"
#define	GPS_PERIODIC_STB_1		"$PMTK223,1,25,180000,60000*38\r\n"
#define	GPS_PERIODIC_STB_2		"$PMTK225,2,3000,12000,18000,72000*15\r\n"

#define	GPS_STANDBY_ST			"$PMTK161,0*28\r\n"
/* USART2 init function */

void Print_USART2_Init(void)
{
  huart2.Instance = USART2;
#if PRINT_CTRL_GPS  
  huart2.Init.BaudRate = 9600;
#else
  huart2.Init.BaudRate = 115200;
#endif
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
	Error_Handler();
  }

	SET_BIT(huart2.Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
	memset(GPS_ReceiveBuffer,0,GPS_DATABUFFER_SIZE);
	memset(Print_DataHeap.m_Buffer,0,PRINT_HEAP_SIZE);
	Print_DataHeap.m_Head = 0;
	Print_DataHeap.m_Tail = 0;
	GPS_RecDataIndex = 0;
	GPS_RecDataIndex_Old = 0;
}
void Uart2_SwitchToFaster(void)	/*串口初始化*/
{
  /* Disable the Peripheral */
  __HAL_UART_DISABLE(&huart2);

  CLEAR_BIT(huart2.Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
  huart2.Instance->BRR = 0x00000119;
  SET_BIT(huart2.Instance->CR1, USART_CR1_PEIE | USART_CR1_RXNEIE);
  
  __HAL_UART_ENABLE(&huart2);  
}
u8 PrintPop(u8* data)
{
	if(Print_DataHeap.m_Head == Print_DataHeap.m_Tail)
	{
		return 0;
	}
	else
	{
		*data = Print_DataHeap.m_Buffer[Print_DataHeap.m_Tail];
		Print_DataHeap.m_Tail++;
		Print_DataHeap.m_Tail %= PRINT_HEAP_SIZE;
		return 1;
	}
}
void PrintPush(u8 data)
{
	Print_DataHeap.m_Buffer[Print_DataHeap.m_Head] = data;
	Print_DataHeap.m_Head++;
	Print_DataHeap.m_Head %= PRINT_HEAP_SIZE;
}
int fputc(int ch, FILE *f)
{
#if LOW_LEVEL_RW
#if (PRINT_CTRL_GPS == 0)
	PrintPush(ch);
	SET_BIT(huart2.Instance->CR1, USART_CR1_TCIE);
#endif
#else
	HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0x0001);
#endif 
	return ch;
}

void GPS_SendData(u8*data, uint32_t size)
{
#if (PRINT_CTRL_GPS == 1)
	for(int i = 0;i < size;i++)
	{
		PrintPush(data[i]);
	}
	if(size > 0)
	{
		SET_BIT(huart2.Instance->CR1, USART_CR1_TCIE);
	}
#endif
}

int fgetc(FILE * f)
{
  uint8_t ch = 0;
  HAL_UART_Receive(&huart2, &ch, 1, 0xffff);
  return ch;
}

void USR_UART2_RxCpltCallback(void)
{
#if LOW_LEVEL_RW
	uint8_t ch = 0;
	if(READ_BIT(huart2.Instance->ISR, USART_ISR_TC) == (USART_ISR_TC))
	{
		if(PrintPop(&ch))
		{
			huart2.Instance->TDR = ch;
		}
		else
		{
			CLEAR_BIT(huart2.Instance->CR1, USART_CR1_TCIE);
		}
	}
	while(READ_BIT(huart2.Instance->ISR, USART_ISR_RXNE) == (USART_ISR_RXNE))
	{
		ch = huart2.Instance->RDR;
		if(GPS_RecDataIndex < GPS_DATABUFFER_SIZE)
		{
			GPS_ReceiveBuffer[GPS_RecDataIndex] = ch;
			GPS_RecDataIndex++;
		}
	}
#else
	uint8_t ch = 0;
	while(HAL_UART_Receive_IT(&huart2, (uint8_t *)&ch, 1) != HAL_OK){}
	//HAL_UART_Receive_IT(&huart2, &ch, 1, 0xffff);
	if(GPS_RecDataIndex < GPS_DATABUFFER_SIZE)
	{
		GPS_ReceiveBuffer[GPS_RecDataIndex] = ch;
		GPS_RecDataIndex++;
	}
#endif
}
//=============================================
//		GPS func
//=============================================
double Convert_DDMM_to_DD(double ddmm)
{
	int tmpDD;
	double tmpDouble;
	tmpDD = (int)ddmm;
	tmpDD /= 100;
	tmpDouble = ddmm  - tmpDD;
	tmpDouble /= 60;
	return (tmpDouble + tmpDD);
}
GPS_RawData m_GPS_RawData;
GPS_Data m_GPS_Data;
u8 m_Gps_WorkState = GPS_DATA_INIT;
u8 m_GPS_Countdown = 0;
void GPS_ReceiveDataHandler(void)
{
	u32 tmpValue;

	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_PE);//清标志	
	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_FE);	
	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_NE);	
	__HAL_UART_CLEAR_FLAG(&huart2, UART_FLAG_ORE);
	if(GPS_RecDataIndex == 0)
	{
		return;
	}
	if(GPS_RecDataIndex != GPS_RecDataIndex_Old)
	{
		GPS_RecDataIndex_Old = GPS_RecDataIndex;
	}
	else
	{
		GPS_ReceiveBuffer[GPS_DATABUFFER_SIZE - 1] = 0;	// prevent over run
		if(GPS_ReceiveBuffer[0] != '$')
		{
			goto CLR_RET;
		}

		if(strncmp((const char *)GPS_ReceiveBuffer,"$GPGGA",6) == 0)
		{
			sscanf((const char *)GPS_ReceiveBuffer,"$GPGGA,%lf,%lf,N,%lf,E,%d,%d,",&(m_GPS_RawData.m_UTC_Position),&(m_GPS_RawData.m_Latitude),&(m_GPS_RawData.m_Longitude),&(m_GPS_RawData.m_PositionFixIndicator),&(m_GPS_RawData.m_SatellitesUsed));
		}	
		else if(strncmp((const char *)GPS_ReceiveBuffer,"$GNGGA",6) == 0)
		{
			sscanf((const char *)GPS_ReceiveBuffer,"$GNGGA,%lf,%lf,N,%lf,E,%d,%d,",&(m_GPS_RawData.m_UTC_Position),&(m_GPS_RawData.m_Latitude),&(m_GPS_RawData.m_Longitude),&(m_GPS_RawData.m_PositionFixIndicator),&(m_GPS_RawData.m_SatellitesUsed));
		}
#if 0
		char tmpPrint[40];
		sprintf(tmpPrint,"Pos:%lf\r\n",m_GPS_Data.m_UTC_Position);
		GPS_SendData((u8*)tmpPrint,strlen(tmpPrint));
		sprintf(tmpPrint,"Lat:%lf\r\n",m_GPS_Data.m_Latitude);
		GPS_SendData((u8*)tmpPrint,strlen(tmpPrint));
		sprintf(tmpPrint,"Lon:%lf\r\n",m_GPS_Data.m_Longitude);
		GPS_SendData((u8*)tmpPrint,strlen(tmpPrint));
		sprintf(tmpPrint,"Num:%d\r\n",m_GPS_Data.m_SatellitesUsed);
		GPS_SendData((u8*)tmpPrint,strlen(tmpPrint));
#endif
		if(m_GPS_RawData.m_SatellitesUsed > 0)
		{
			tmpValue = (u32)(m_GPS_RawData.m_UTC_Position);
			m_GPS_Data.m_UTC_Secs= (u8)(tmpValue % 0xff);
			tmpValue >>= 8;
			m_GPS_Data.m_UTC_Mins = (u8)(tmpValue % 0xff);
			tmpValue >>= 8;
			m_GPS_Data.m_UTC_Hour = (u8)(tmpValue % 0xff);
			m_GPS_Data.m_Latitude_degree = Convert_DDMM_to_DD(m_GPS_RawData.m_Latitude);
			m_GPS_Data.m_Longitude_degree = Convert_DDMM_to_DD(m_GPS_RawData.m_Longitude);
		}
CLR_RET:
		GPS_RecDataIndex = 0;
		GPS_RecDataIndex_Old = 0;
		memset(GPS_ReceiveBuffer,0,GPS_DATABUFFER_SIZE);
		// parser here
	}
}
GPS_Data GetGPSPosition(void)
{
	return m_GPS_Data;
}
void GPS_Handler(void)
{
#if 1
	switch(m_Gps_WorkState)
	{
		case GPS_DATA_INIT:
			m_Gps_WorkState = GPS_GPS_ONLY;
			m_GPS_Countdown = 0;
			break;
		case GPS_GPS_ONLY:
			m_Gps_WorkState = GPS_PERIODIC;	//GPS_READY_WORK;//
			GPS_SendData(GPS_ONLY_GPS_ST,strlen(GPS_ONLY_GPS_ST));
			break;
		case GPS_PERIODIC:
			m_Gps_WorkState = GPS_READY_WORK;
			GPS_SendData(GPS_PERIODIC_STB_0,strlen(GPS_PERIODIC_STB_0));
			GPS_SendData(GPS_PERIODIC_STB_1,strlen(GPS_PERIODIC_STB_1));
			GPS_SendData(GPS_PERIODIC_STB_2,strlen(GPS_PERIODIC_STB_2));
			break;
		case GPS_READY_WORK:
			GPS_ReceiveDataHandler();
			break;
		case GPS_STANDBY_MODE:
			m_GPS_Countdown++;
			if(m_GPS_Countdown > 50)
			{
				m_Gps_WorkState = GPS_READY_WORK;
				m_GPS_Countdown = 0;
				GPS_SendData("work",4);
			}
			GPS_ReceiveDataHandler();
			break;
		default:
			break;
	}
#else
	GPS_ReceiveDataHandler();
#endif
}

void GPS_RecTask(void *param)
{
	while(1)
	{
		switch(m_Gps_WorkState)
		{
			default:
				vTaskDelay(100);
			case GPS_READY_WORK:
				vTaskDelay(10);
				GPS_Handler();
				break;
		}
	}
}
//#ifndef sin
//#define	sin(x)			arm_sin_f32(x)
//#endif
//#ifndef cos
//#define	cos(x)			arm_cos_f32(x)
//#endif
#ifndef Pi
#define Pi                 		3.14159265358979f
#endif
/* * 根据一点经纬度 距离 方位 求另一点经纬度 */
void getNewPostion(double lat,double lng,float distance,double angle,double *NewPosLat,  double *NewPosLng)
{    
	*NewPosLat = lat + (distance * cosf(angle * Pi / 180)) / 111;    
	*NewPosLng = lng + (distance * sinf(angle * Pi / 180)) / (111 * cosf(lat * Pi / 180));
}

void GPS_getNewPostion(double* lat,double* lng)
{
	*lat = 0;
	*lng = 0;
}
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
