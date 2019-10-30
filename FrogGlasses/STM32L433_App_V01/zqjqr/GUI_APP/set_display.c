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
const unsigned char* stepLanguage4[3]={
{"配对"},
{"语言"},
{"恢复"},
};
const unsigned char* setLang_En[3]={
{"PAIR"},
{"LANG"},
{"RESET"},
};

void set_ui(void)
{
#if OLED_SCREEN
	oled_showPicture(0,0,bmp_set,127,63);	
	show_str(14,46,stepLanguage4[0],12,12,1);	
	show_str(52,46,stepLanguage4[1],12,12,1);	
	show_str(80,46,stepLanguage4[2],12,12,1);		
#else
#if 0
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 31;
		y = 10;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_pair);
		y = 48;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_language);
		y = 86;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_restore);
		
		x = 67;
		y = 8;
		width = 88;
		high = 28;
		BACK_COLOR = BLACK;	
		POINT_COLOR = 0x0C1C;
		LCD_Show_Image(x, y, width, high, gImage_mode_key);	
		y = 46;
		BACK_COLOR = BLACK;	
		POINT_COLOR = 0x1565;//GREEN;
		LCD_Show_Image(x, y, width, high, gImage_set_key);		
		y = 84;
		BACK_COLOR = BLACK;	
		POINT_COLOR =0xCEA7; //YELLOW;
		LCD_Show_Image(x, y, width, high, gImage_recording_key);

		x = 90;
		y = 14;
		width = 32;
		high = 16;
		BACK_COLOR = 0x0C1C;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,stepLanguage4[0],16);	
		x = 90;		
		y = 53;
		width = 48;	
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		Show_Str(x,y,width,high,stepLanguage4[1],16);
		x = 90;		
		y = 90;
		width = 32;
		high = 16;		
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,stepLanguage4[2],16);	
		flush_enable = 1;
	}
#endif
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 36;
		y = 20;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_pair);
		y = 48;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_language);
		y = 76;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_restore);
		
		x = 82;
		y = 22;
		width = 72;
		high = 20;
		BACK_COLOR = BLACK;	
		POINT_COLOR = 0x0C1C;
		LCD_Show_Image(x, y, width, high, gImage_mode_key);	
		y = 50;
		BACK_COLOR = BLACK;	
		POINT_COLOR = 0x1565;//GREEN;
		LCD_Show_Image(x, y, width, high, gImage_set_key);		
		y = 78;
		BACK_COLOR = BLACK;	
		POINT_COLOR =0xCEA7; //YELLOW;
		LCD_Show_Image(x, y, width, high, gImage_recording_key);

		x = 89;
		y = 24;
		width = 32;
		high = 16;
		BACK_COLOR = 0x0C1C;	
		POINT_COLOR = BLACK;
				
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage4[0],16);
		}
		else
		{
			Show_Str(x,y,width,high,setLang_En[0],16);
		}
		y = 53;
		width = 48;	
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		//Show_Str(x,y,width,high,stepLanguage4[1],16);	
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage4[1],16);
		}
		else
		{
			Show_Str(x,y,width,high,setLang_En[1],16);
		}
		y = 81;
		width = 32;
		high = 16;		
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		//Show_Str(x,y,width,high,stepLanguage4[2],16);	
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage4[2],16);
		}
		else
		{
			Show_Str(x,y,width+8,high,setLang_En[2],16);
		}
		flush_enable = 1;
	}

#endif
#if KEY_HANDLER_NEW
#else
	/*按键处理*/
	u8 keyState = getKeyState();

	if(keyState == KEY_C_SHORT_PRESS)
	{

		vTaskDelay(PRESS_KEY_TIME);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_C_SHORT_PRESS){
			setShow_ui(MENU_UI);
		}else{
			setShow_ui(RESET_UI);
		}
		
	}else if(keyState==KEY_B_SHORT_PRESS){

		vTaskDelay(500);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_B_SHORT_PRESS){
			setShow_ui(MAIN_UI);					
		}else{
			setShow_ui(LANGUAGE_UI);
		}		
	}else if(keyState==KEY_A_SHORT_PRESS){

		vTaskDelay(500);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_A_SHORT_PRESS){

			setShow_ui(MENU_UI);			
		}else{
			setShow_ui(MATCH_UI);	
		}		
	}	
#endif	
}

