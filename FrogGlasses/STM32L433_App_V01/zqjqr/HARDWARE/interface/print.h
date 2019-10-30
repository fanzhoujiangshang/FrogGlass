/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __PRINT_H
#define __PRINT_H
#ifdef __cplusplus
 extern "C" {
#endif
#include "sys.h"
#include <stdio.h>
#include "main.h"
extern UART_HandleTypeDef huart2;

#define	PRINT_HEAP_SIZE				(200)
typedef struct
{
	u8 m_Head;
	u8 m_Tail;
	u8 m_Buffer[PRINT_HEAP_SIZE];
}PrintDataQueue;

typedef struct
{
	double m_UTC_Position;
	double m_Latitude;
	double m_Longitude;
	int32_t m_PositionFixIndicator;
	int32_t m_SatellitesUsed;
}GPS_RawData;
typedef struct
{
	u8 m_UTC_Hour;
	u8 m_UTC_Mins;
	u8 m_UTC_Secs;
	double m_Latitude_degree;
	double m_Longitude_degree;
}GPS_Data;

enum
{
	GPS_DATA_INIT,			// power on
	GPS_GPS_ONLY,			// turn other position method
	GPS_PERIODIC,			// set to period wakeup mode
	GPS_READY_WORK,		// ready for receive GPS data
	GPS_STANDBY_MODE,	// standby mode	
	GPS_MAX				// reserved
};


void Print_USART2_Init(void);
void USR_UART2_RxCpltCallback(void);
void GPS_ReceiveDataHandler(void);
void Uart2_SwitchToFaster(void);
void GPS_Handler(void);
void GPS_RecTask(void *param);
GPS_Data GetGPSPosition(void);
void GPS_getNewPostion(double* lat,double* lng);

#ifdef __cplusplus
}
#endif
#endif /*__PRINT_H  */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
