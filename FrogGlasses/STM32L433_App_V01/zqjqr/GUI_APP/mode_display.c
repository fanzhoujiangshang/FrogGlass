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
#include "storageTask.h"
#include "FreeRTOS.h"
#include "task.h"


/********************************************************************************	 
 * 调试界面代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
const unsigned char* stepLanguage3[2][3]={
{"空气","自由潜","高氧"},
{"AIR","MIX","NIX"},
};
void mode_ui(void)
{
#if OLED_SCREEN
	oled_showPicture(0,0,bmp_menu,127,63);	
	show_str(16,46,stepLanguage3[0][0],12,12,1);	
	show_str(50,46,stepLanguage3[0][1],12,12,1);	
	show_str(92,46,stepLanguage3[0][2],12,12,1);
#else
#if 0
	u8 x,y,width,high;	

	if(flush_enable == 0)
	{
		x = 31;
		y = 10;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_scuba);
		y = 48;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_freestyle);
		y = 86;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_hyperoxia);
		
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
		Show_Str(x,y,width,high,stepLanguage3[0][0],16);	
		x = 76;		
		y = 53;
		width = 48;	
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		Show_Str(x,y,width,high,stepLanguage3[0][1],16);
		x = 90;		
		y = 90;
		width = 32;
		high = 16;		
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,stepLanguage3[0][2],16);	
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
		LCD_Show_Image(x, y, width, high, gImage_scuba);
		y = 48;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_freestyle);
		y = 76;
		width = 24;
		high = 24;
		LCD_Show_Image(x, y, width, high, gImage_hyperoxia);
		
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
		y = 25;
		width = 32;
		high = 16;
		BACK_COLOR = 0x0C1C;	
		POINT_COLOR = BLACK;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage3[0][0],16);	
		}
		else
		{
			Show_Str(x,y,width,high,stepLanguage3[1][0],16);
		}
		x = 88;		
		y = 53;
		width = 48;	
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;			
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage3[0][1],16);	
		}
		else
		{
			Show_Str(x,y,width,high,stepLanguage3[1][1],16);
		}
		x = 89;		
		y = 81;
		width = 32;
		high = 16;		
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage3[0][2],16);	
		}
		else
		{
			Show_Str(x,y,width,high,stepLanguage3[1][2],16);
		}
		flush_enable = 1;
	}

#endif
#if KEY_HANDLER_NEW
#else
	/*按键处理*/
	u8 keyState = getKeyState();

	if(keyState == KEY_A_SHORT_PRESS)
	{
		vTaskDelay(PRESS_KEY_TIME);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_A_SHORT_PRESS){
			setShow_ui(MENU_UI);
		}else{
			setShow_ui(TIME_UI);
		}
		
	}else if(keyState==KEY_B_SHORT_PRESS){

		vTaskDelay(PRESS_KEY_TIME);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_B_SHORT_PRESS){
			setShow_ui(MAIN_UI);					
		}else{
			setShow_ui(TIME_UI);
		}		
	}else if(keyState==KEY_C_SHORT_PRESS){
		vTaskDelay(PRESS_KEY_TIME);	
		u8 keyState1 = getKeyState();
		
		if(keyState1 == KEY_C_SHORT_PRESS){
			setShow_ui(MAIN_UI);			
		}else{
			setShow_ui(TIME_UI);	
		}		
	}	
#endif	
}

