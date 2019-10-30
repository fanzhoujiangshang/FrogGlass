#include <stdio.h>
#include <string.h>
#include "match_ui.h"
#include <string.h>
#include <stdio.h>
#include "keyTask.h"
#include "displayTask.h"
#include "main_ui.h"
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
 * 调试界面代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

const unsigned char* stepLanguage6[3]={
{"设备未匹配"},
{"请通过APP"},
{"匹配"},
};

const unsigned char* MatchLang_En[2]={
{"NOT PAIR"},
{"PLEASE CHECK APP"},
};

void match_ui(void)
{
#if OLED_SCREEN
	oled_showPicture(0,0,bmp_match,127,63);	
	show_str(50,8,stepLanguage6[0],12,12,1);	
	show_str(50,24,stepLanguage6[1],12,12,1);	
	show_str(50,40,stepLanguage6[2],12,12,1);		
#else
#if 0
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 41;
		y = 23;
		width = 16;
		high = 25;
		LCD_Show_Image(x, y, width, high, gImage_mobile_phone);
		x = 81;
		y = 28;
		width = 18;
		high = 16;
		LCD_Show_Image(x, y, width, high, gImage_two_way);
		x = 123;		
		y = 32;
		width = 18;
		high = 8;
		LCD_Show_Image(x, y, width, high, gImage_frog_mirror);
		

		x = 41;
		y = 58;
		width = 80;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		Show_Str(x,y,width,high,stepLanguage6[0],16);
		x = 41;		
		y = 84;
		width = 96;
		high = 16;		
		Show_Str(x,y,width,high,stepLanguage6[1],16);	
		x = 113;		
		y = 84;	
		width = 64;
		high = 16;
		Show_Str(x,y,width,high,stepLanguage6[2],16);	
		flush_enable = 1;
	}
#endif
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 44;
		y = 23;
		width = 16;
		high = 25;
		LCD_Show_Image(x, y, width, high, gImage_mobile_phone);
		x = 81;
		y = 28;
		width = 18;
		high = 16;
		LCD_Show_Image(x, y, width, high, gImage_two_way);
		x = 118;		
		y = 32;
		width = 18;
		high = 8;
		LCD_Show_Image(x, y, width, high, gImage_frog_mirror);
		

		x = 44;
		y = 54;
		width = 80;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage6[0],16);
		}
		else
		{
			Show_Str(x,y,width,high,MatchLang_En[0],16);
		}
		

		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			x = 41;		
			y = 75;
			width = 96;
			high = 16;	
			Show_Str(x,y,width,high,stepLanguage6[1],16);	

			x = 113;		
			y = 75;	
			width = 64;
			high = 16;
			Show_Str(x,y,width,high,stepLanguage6[2],16);	
		}
		else
		{
			x = 41;		
			y = 80;
			width = 130;
			high = 16;
			
			Show_Str(x,y,width,high,MatchLang_En[1],16);
		}

		flush_enable = 1;
	}

#endif
#if KEY_HANDLER_NEW
#else
	/*按键处理*/
	u8 keyState = getKeyState();

	if(keyState == KEY_C_SHORT_PRESS){

		vTaskDelay(PRESS_KEY_TIME);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_C_SHORT_PRESS){
			setShow_ui(MENU_UI);
		}
		
	}else if(keyState==KEY_B_SHORT_PRESS){

		vTaskDelay(PRESS_KEY_TIME);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_B_SHORT_PRESS){
			setShow_ui(MAIN_UI);					
		}	
	}else if(keyState==KEY_A_SHORT_PRESS){

		vTaskDelay(PRESS_KEY_TIME);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_A_SHORT_PRESS){
			setShow_ui(SET_UI);			
		}		
	}	
#endif
}


