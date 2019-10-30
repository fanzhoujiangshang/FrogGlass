#include <string.h>
#include <stdio.h>
#include "keyTask.h"
#include "displayTask.h"
#include "main_ui.h"
#include "font.h"
#include "text.h"
#include "reset_ui.h"
#if OLED_SCREEN
#include "oled.h"
#else
#include "lcd.h"
#include "zqjqr_log.h"
#endif
#include "storageTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "radiolink.h"

/********************************************************************************	 
 * 调试界面代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
const unsigned char* ResetLang_Chi[5]={
{"是否恢复出厂"},
{"是"},
{"否"},
{"恢复出厂"},
{"版本"}
};
const unsigned char* ResetLang_En[5]={
{"RESET ALL ?"},
{"YES"},
{"NO"},
{"RESET"},
{"Version"}
};

void reset_ui(void)
{
	uint8_t tmpStr[10];
#if OLED_SCREEN
	show_str(14,20,ResetLang_Chi[0],12,12,1);	
	show_str(52,46,ResetLang_Chi[1],12,12,1);	
	show_str(80,46,ResetLang_Chi[2],12,12,1);	
#else
#if 0
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 26;
		y = 14;
		width = 128;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		Show_Str(x,y,width,high,ResetLang_Chi[0],16);	

		
		x = 67;
		y = 46;
		width = 88;
		high = 28;
		BACK_COLOR = BLACK;	
		POINT_COLOR = 0x1565;//GREEN;
		LCD_Show_Image(x, y, width, high, gImage_set_key);		
		y = 84;
		BACK_COLOR = BLACK;	
		POINT_COLOR =0xCEA7; //YELLOW;
		LCD_Show_Image(x, y, width, high, gImage_recording_key);

	
		x = 95;		
		y = 53;
		width = 16;
		high = 16;
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		Show_Str(x,y,width,high,ResetLang_Chi[1],16);
		x = 95;		
		y = 90;
		width = 16;
		high = 16;		
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,ResetLang_Chi[2],16);	
		flush_enable = 1;
	}
#endif
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 26;
		y = 25;
		width = 128;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,ResetLang_Chi[0],16);	
		}
		else
		{
			Show_Str(x,y,width,high,ResetLang_En[0],16);	
		}
		
		x = 82;
		y = 51;
		width = 72;
		high = 20;
		BACK_COLOR = BLACK;	
		POINT_COLOR = 0x1565;//GREEN;
		LCD_Show_Image(x, y, width, high, gImage_set_key);		
		y = 79;
		BACK_COLOR = BLACK;	
		POINT_COLOR =0xCEA7; //YELLOW;
		LCD_Show_Image(x, y, width, high, gImage_recording_key);

	
		x = 89;		
		y = 54;
		width = 16;
		high = 16;
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,ResetLang_Chi[1],16);
		}
		else
		{
			Show_Str(x,y,width,high,ResetLang_En[1],16);
		}
		x = 89;		
		y = 82;
		width = 16;
		high = 16;		
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,ResetLang_Chi[2],16);	
		}
		else
		{
			Show_Str(x,y,width,high,ResetLang_En[2],16);
		}
		//-----------------------------------------------------version
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			x = 35;
			y = 64;
			width = 32;
			high = 16;		
			Show_Str(x,y,width,high,ResetLang_Chi[4],16);	
		}
		else
		{
			x = 35;
			y = 64;		
			width = 42;
			high = 12;
			Show_Str(x,y,width,high,ResetLang_En[4],12);	
		}	
		x = 35;
		y = 85;
		width = 64;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = 0x0c1c;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			sprintf((char*)tmpStr,"V%d.%d",FW_VERSION_PART_0,FW_VERSION_PART_1);
			Show_Str(x,y,width,high,tmpStr,12);	
		}
		else
		{
			sprintf((char*)tmpStr,"V%d.%d",FW_VERSION_PART_0,FW_VERSION_PART_1);
			Show_Str(x,y,width,high,tmpStr,12);	
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
		}else{
			setShow_ui(SET_UI);
		}
		
	}else if(keyState==KEY_B_SHORT_PRESS){

		vTaskDelay(PRESS_KEY_TIME);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_B_SHORT_PRESS){
			setShow_ui(MAIN_UI);					
		}else{
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

