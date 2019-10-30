#include <string.h>
#include <stdio.h>
#include "keyTask.h"
#include "displayTask.h"
#include "main_ui.h"
#include "font.h"
#include "text.h"
//#include "language_ui.h"
#if OLED_SCREEN
#include "oled.h"
#else
#include "lcd.h"
#include "zqjqr_log.h"
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "storageTask.h"
/********************************************************************************	 
 * 调试界面代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
const unsigned char* stepLanguage7[3]={
{"界面语言选择"},
{"CN"},
{"中文"},
};
const unsigned char* LanguageSelect[2][3]={
	{
		{"LANGUAGE"},
		{"En"},
		{"中文"},
	},
	{
		{"界面语言选择"},
		{"En"},
		{"中文"},
	}	
};

void language_ui(void)
{
#if OLED_SCREEN
	show_str(34,20,stepLanguage7[0],12,12,1);	
	show_str(52,46,stepLanguage7[1],12,12,1);	
	show_str(80,46,stepLanguage7[2],12,12,1);		
#else
#if 0
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 41;
		y = 14;
		width = 96;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		Show_Str(x,y,width,high,stepLanguage7[0],16);	

		
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

	
		x = 92;		
		y = 53;
		width = 32;
		high = 16;
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		Show_Str(x,y,width,high,stepLanguage7[1],16);
		x = 92;		
		y = 90;
		width = 32;
		high = 16;		
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,stepLanguage7[2],16);	
		flush_enable = 1;
	}
#endif
	u8 x,y,width,high;	
	uint8_t tmpLanguage;

	if(flush_enable == 0)
	{
		MX25Lxxx_ReadLanguage(&tmpLanguage);
		if(tmpLanguage == LANGUAGE_CHI)
		{
			x = 41;
			y = 25;
			width = 96;
			high = 16;
			BACK_COLOR = BLACK;	
			POINT_COLOR = GRAY;
			Show_Str(x,y,width,high,LanguageSelect[1][0],16);	

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
			width = 32;
			high = 16;
			BACK_COLOR = 0x1565;	
			POINT_COLOR = BLACK;	
			Show_Str(x,y,width,high,LanguageSelect[1][1],16);
			x = 89;		
			y = 82;
			width = 32;
			high = 16;		
			BACK_COLOR = 0xCEA7;	
			POINT_COLOR = BLACK;
			Show_Str(x,y,width,high,LanguageSelect[1][2],16);	
		}
		else
		{
			x = 20;
			y = 25;
			width = 140;
			high = 16;
			BACK_COLOR = BLACK;	
			POINT_COLOR = GRAY;
			Show_Str(x,y,width,high,LanguageSelect[0][0],16);	

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
	
			x = 92;		
			y = 54;
			width = 32;
			high = 16;
			BACK_COLOR = 0x1565;	
			POINT_COLOR = BLACK;	
			Show_Str(x,y,width,high,LanguageSelect[0][1],16);
			x = 89;		
			y = 82;
			width = 32;
			high = 16;		
			BACK_COLOR = 0xCEA7;	
			POINT_COLOR = BLACK;
			Show_Str(x,y,width,high,LanguageSelect[0][2],16);	
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
	}	else if(keyState==KEY_B_SHORT_PRESS){

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

