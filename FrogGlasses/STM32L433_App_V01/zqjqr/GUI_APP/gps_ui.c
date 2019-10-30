#include <string.h>
#include <stdio.h>
#include "keyTask.h"
#include "displayTask.h"
#include "gps_ui.h"
#include "font.h"
#include "text.h"
#if OLED_SCREEN
#include "oled.h"
#else
#include "lcd.h"
#include "zqjqr_log.h"
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "print.h"
/********************************************************************************	 
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
extern GPS_RawData m_GPS_RawData;

void gps_ui(void)
{
	u8 x,y,width,high;	
	u8 tmpBuf[20];

	BACK_COLOR = BLACK;	
	POINT_COLOR = WHITE;
	//sscanf((const char *)GPS_ReceiveBuffer,"$GNGGA,%lf,%lf,N,%lf,E,%d,%d,",&(m_GPS_Data.m_UTC_Position),&(m_GPS_Data.m_Latitude),&(m_GPS_Data.m_Longitude),&(m_GPS_Data.m_PositionFixIndicator),&(m_GPS_Data.m_SatellitesUsed));

	x = 20;
	y = 20;		
	width = 140;
	high = 20;
	sprintf((char*)tmpBuf,"UTC:%lf",m_GPS_RawData.m_UTC_Position);
	Show_Str(x,y,width,high,tmpBuf,16);	

	x = 20;
	y = 38;		
	width = 140;
	high = 20;
	sprintf((char*)tmpBuf,"LAT:%lf",m_GPS_RawData.m_Latitude);
	Show_Str(x,y,width,high,tmpBuf,16);	

	x = 20;
	y = 56;		
	width = 140;
	high = 20;
	sprintf((char*)tmpBuf,"LON:%lf",m_GPS_RawData.m_Longitude);
	Show_Str(x,y,width,high,tmpBuf,16);	

	x = 20;
	y = 74;		
	width = 140;
	high = 20;
	sprintf((char*)tmpBuf,"PFI:%d",m_GPS_RawData.m_PositionFixIndicator);
	Show_Str(x,y,width,high,tmpBuf,16);	

	x = 20;
	y = 92;		
	width = 140;
	high = 20;
	sprintf((char*)tmpBuf,"SaU:%d",m_GPS_RawData.m_SatellitesUsed);
	Show_Str(x,y,width,high,tmpBuf,16);	

	flush_enable = 1;
}
 
 
