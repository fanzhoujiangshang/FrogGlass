#include <string.h>
#include <stdio.h>
#include "keyTask.h"
#include "displayTask.h"
#include "depth_ui.h"
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
#include "rtc.h"

/********************************************************************************	 
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
const unsigned char* stepLanguage10[3]={
{"最大"},
{"深度"},
{"确定"},
};
const unsigned char* DiveDepthLang_En[4]={
{"MAX"},
{"DEP"},
{"OK"},
{"DEPTH"},
};

void depth_ui_new(u8 step)
{
	u8 x,y,width,high,tmp;	

	if(flush_enable == 0)
	{
		x = 32;
		y = 22;
		width = 20;
		high = 20;
		LCD_Show_Image(x, y, width, high, gImage_depth_max);

		y = 50;
		width = 64;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage10[0],16);		
		}
		else
		{
			Show_Str(x,y,width,high,DiveDepthLang_En[3],16);		
		}
		y = 66;
		width = 32;
		high = 16;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage10[1],16);		
		}
		else
		{
			Show_Str(x,y,width,high,DiveDepthLang_En[0],16);		
		}
				
	
		
		x = 76;
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

		x = 83;
		y = 24;
		width = 32;
		high = 16;
		BACK_COLOR = 0x0C1C;	
		POINT_COLOR = BLACK;
		
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage10[2],16);	
		}
		else
		{
			Show_Str(x,y,width,high,DiveDepthLang_En[2],16);		
		}
		x = 83;		
		y = 53;
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage10[1],16);	
			LCD_ShowChar(116, 51, '+', 16);
		}
		else
		{
			Show_Str(x,y,width,high,DiveDepthLang_En[1],16);
			LCD_ShowChar(108, 51, '+', 16);
		}
		
		y = 81;
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		if( LANGUAGE_CH == m_ConfigParams.m_Language)
		{
			Show_Str(x,y,width,high,stepLanguage10[1],16);	
			LCD_ShowChar(116, 81, '-', 16);
		}
		else
		{
			Show_Str(x,y,width,high,DiveDepthLang_En[1],16);
			LCD_ShowChar(108, 81, '-', 16);
		}		
		flush_enable = 1;
	}
	
	x = 32;
	y = 86;
	width = 6;
	high = 9;
	tmp = step%100;	
	LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/100]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%100/10]);
	LCD_Show_Image(x+2*width+4, y, width, high, gImage_num_16[tmp%10]);	

}
void depth_ui(void)
{
#if OLED_SCREEN
	int x,y,i,j;
	static int step = 20;

	oled_showPicture(0,0,bmp_max_depth,127,63);	

	x = 48;
	y = 16;
	i = 12;
	j = step%100;
	oled_showNum(x,y,(u8)(step/100),1,12,24);				//时间
	oled_showNum(x+i,y,(u8)(j/10),1,12,24);		
	oled_showNum(x+2*i,y,(u8)j%10,1,12,24);			
#else
#if 0
	u8 x,y,width,high,tmp;	
	static int step = 20;

	if(flush_enable == 0)
	{
		x = 7;
		y = 11;
		width = 26;
		high = 26;
		LCD_Show_Image(x, y, width, high, gImage_depth);

		y = 36;
		width = 64;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		Show_Str(x,y,width,high,stepLanguage10[0],16);		
		y = 52;
		width = 32;
		high = 16;
		Show_Str(x,y,width,high,stepLanguage10[1],16);			
	
		
		x = 81;
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

		x = 98;
		y = 15;
		width = 32;
		high = 16;
		BACK_COLOR = 0x0C1C;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,stepLanguage10[2],16);
		x = 93;		
		y = 53;
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		Show_Str(x,y,width,high,stepLanguage10[1],16);	
		LCD_ShowChar(126, 57, '+', 16);
		y = 91;
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,stepLanguage10[1],16);	
		LCD_ShowChar(126, 94, '-', 16);		
		flush_enable = 1;
	}
	
	x = 5;
	y = 81;
	width = 13;
	high = 20;
	tmp = step%100;	
	LCD_Show_Image(x, y, width, high, gImage_num_32[tmp/100]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_32[tmp%100/10]);
	LCD_Show_Image(x+2*width+4, y, width, high, gImage_num_32[tmp%10]);	
#endif
	u8 x,y,width,high,tmp;	
	static int step = 20;

	if(flush_enable == 0)
	{
		x = 32;
		y = 22;
		width = 26;
		high = 26;
		LCD_Show_Image(x, y, width, high, gImage_depth);

		y = 50;
		width = 64;
		high = 16;
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		Show_Str(x,y,width,high,stepLanguage10[0],16);		
		y = 66;
		width = 32;
		high = 16;
		Show_Str(x,y,width,high,stepLanguage10[1],16);			
	
		
		x = 76;
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

		x = 83;
		y = 24;
		width = 32;
		high = 16;
		BACK_COLOR = 0x0C1C;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,stepLanguage10[2],16);
		x = 83;		
		y = 53;
		BACK_COLOR = 0x1565;	
		POINT_COLOR = BLACK;	
		Show_Str(x,y,width,high,stepLanguage10[1],16);	
		LCD_ShowChar(116, 51, '+', 16);
		y = 81;
		BACK_COLOR = 0xCEA7;	
		POINT_COLOR = BLACK;
		Show_Str(x,y,width,high,stepLanguage10[1],16);	
		LCD_ShowChar(116, 81, '-', 16);		
		flush_enable = 1;
	}
	
	x = 32;
	y = 86;
	width = 6;
	high = 9;
	tmp = step%100;	
	LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/100]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%100/10]);
	LCD_Show_Image(x+2*width+4, y, width, high, gImage_num_16[tmp%10]);	


#endif
#if KEY_HANDLER_NEW
	u8 keyState = getKeyStateNew();										//获取单击或者双击

	if(keyState == KEY_CC_SHORT_PRESS || keyState == KEY_CC_LONG_PRESS){	
		 step --;	
		if(step < 0) step = 99;		 
	}else if(keyState==KEY_BB_SHORT_PRESS || keyState == KEY_BB_LONG_PRESS){
		step ++;	
		if(step > 100) step = 0;				
	}else if(keyState==KEY_AA_SHORT_PRESS) {
		setShow_ui(MAIN_UI);			
	}
#else
	u8 keyState = getKeyState();										//获取单击或者双击

	if(keyState == KEY_C_SHORT_PRESS || keyState == KEY_C_LONG_PRESS){	
		 step --;	
		if(step < 0) step = 99;		 
	}else if(keyState==KEY_B_SHORT_PRESS || keyState == KEY_B_LONG_PRESS){
		step ++;	
		if(step > 100) step = 0;				
	}else if(keyState==KEY_A_SHORT_PRESS) {
		setShow_ui(MAIN_UI);			
	}


#endif	
}
