#include <string.h>
#include <stdio.h>
#include "keyTask.h"
#include "displayTask.h"
#include "update_ui.h"
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
/********************************************************************************	 
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
const unsigned char* updateLang_Ch[2]={
{"设备升级中"},
{"CDID:"}
};
const unsigned char* updateLang_En[2]={
{"UPDATING"},
{"CDID:"},
};


void update_ui(u8 step)
{
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{

		x = 46;
		y = 38;
		
		width = 14;
		high = 20;
		LCD_Show_Image(x, y, width, high, gImage_bl_connect);	

		
		x = 71;
		y = 39;
		width = 80;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,updateLang_Ch[0],16);	
		}
		else
		{
			Show_Str(x,y,width,high,updateLang_En[0],16);	 
		}

		x = 42;
		y = 80;
		width = 50;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,updateLang_Ch[1],16);	
		}
		else
		{
			Show_Str(x,y,width,high,updateLang_En[1],16);	 
		}

		//CDID:
		x = 81;
		y = 80;
		width = 80;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;

		const unsigned char* cDID ="A2B3123456";
		
		Show_Str(x,y,width,high,cDID,16);	
		
		//update progress base line

		x = 48;
		y = 66;
		LCD_Fill(x,	y,	x+100 ,	y + 3,	GRAY);

		flush_enable = 1;
	}


	//update progress current line
	x = 48;
	y = 66;
	if(step > 100)
	{
		step = 100;
	}
	LCD_Fill(x,	y,	x+step ,		y + 3,	WHITE);
	
}
 
 
