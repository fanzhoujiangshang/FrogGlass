#include <string.h>
#include <stdio.h>
#include "keyTask.h"
#include "displayTask.h"
#include "endDive_ui.h"
#include "font.h"
#include "text.h"
#include "global.h"

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
const unsigned char* endDiveLang_Ch[7]={
{"当前潜水模式状态"},
{"结束"},
{"取消"},
{"空气"},
{"自由潜"},
{"高氧"},
{"模式"}
};
const unsigned char* endDiveLang_En[6]={
{"DIVE STATUS"},
{"FINISH"},
{"BACK"},
{"AIR"},
{"MIX"},
{"NIX"},
};


void endDive_ui(u8 step)
{
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 42;
		y = 27;
		
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			width = 130;
			Show_Str(x,y,width,high,endDiveLang_Ch[0],16);	
		}
		else
		{
			width = 80;
			Show_Str(x,y,width,high,endDiveLang_En[0],16);	 
		}


		
		x = 82;
		width = 72;
		high = 20;
		y = 50;
		
		BACK_COLOR = BLACK;	
		POINT_COLOR = 0x1565;//GREEN;
		LCD_Show_Image(x, y, width, high, gImage_set_key);		
		y = 78;
		BACK_COLOR = BLACK;	
		POINT_COLOR =0xCEA7; //YELLOW;
		LCD_Show_Image(x, y, width, high, gImage_recording_key);

		x = 89;
		width = 32;
		high = 16;
		y = 53;
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		if(LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,endDiveLang_Ch[1],16);
		}
		else
		{
			Show_Str(x,y,50,high,endDiveLang_En[1],16);
		}
				
		y = 81;
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		
		if(LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,endDiveLang_Ch[2],16);	
		}
		else
		{
			Show_Str(x,y,width,high,endDiveLang_En[2],16);	
		}
			

		flush_enable = 1;
	}

	//current mode
	BACK_COLOR = BLACK;	
	POINT_COLOR = BLUE;	

	x = 32;
	width = 48;
	high = 16;
	y = 58;
	
	if(DIVE_MODE_AIR == g_nCurrentDiveMode) //air
	{
		if(LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,endDiveLang_Ch[3],16);
			y = 58 +18;;
			Show_Str(x,y,width,high,endDiveLang_Ch[6],16);
		}
		else
		{
			y= 58+8;
			Show_Str(x,y,50,high,endDiveLang_En[3],16);
		}
	}
	else if(DIVE_MODE_MIX == g_nCurrentDiveMode) //mix
	{
		if(LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			x=x-8;
			Show_Str(x,y,width,high,endDiveLang_Ch[4],16);
			x=x+8;
			y = 58 +18;
			Show_Str(x,y,width,high,endDiveLang_Ch[6],16);
		}
		else
		{
			y= 58+8;
			Show_Str(x,y,50,high,endDiveLang_En[4],16);
		}
	}
	else if(DIVE_MODE_NIX == g_nCurrentDiveMode)//nix
	{			
		if(LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,endDiveLang_Ch[5],16);
			y = 58 +18;
			Show_Str(x,y,width,high,endDiveLang_Ch[6],16);
		}
		else
		{
			y= 58+8;
			Show_Str(x,y,50,high,endDiveLang_En[5],16);
		}
	}


	
}
 
 
