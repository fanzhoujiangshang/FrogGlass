#include <string.h>
#include <stdio.h>
#include "keyTask.h"
#include "displayTask.h"
#include "main_ui.h"
#include "font.h"
#include "text.h"
//#include "rtc.h"
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
const unsigned char* loggerLanguage11[3]={
{"水肺"},
{"自由潜"},
{"仪表"},
};

u8 ConvertOffset(u8 Index)
{
	uint16_t tmpValue;
	
	if(g_RecordIndexInfo.m_TotalRecordNum > 0)
	{
		if(Index <= g_RecordIndexInfo.m_NewestRecordOffset)
		{
			return g_RecordIndexInfo.m_NewestRecordOffset - Index;
		}
		else
		{
			tmpValue = g_RecordIndexInfo.m_TotalRecordNum;
			if(tmpValue > Index)
			{
				tmpValue -= Index;
				tmpValue += g_RecordIndexInfo.m_NewestRecordOffset;
				return tmpValue;
			}
		}
	}
	
	return 0;
}

void logger_ui_new(u8 RecordIndex)
{
	int x,y,width,high,tmp;
	diveRecord_t dive_tmp;	
	Dive_Head tmpDiveHead;
	
	if(flush_enable == 0)
	{	
		int nLangue = m_ConfigParams.m_Language;  //根据用户配置设置界面
		
		if(g_RecordIndexInfo.m_TotalRecordNum > 0) 
		{
			x = 27;
			y = 38;
			width = 20;
			high = 20;
			BACK_COLOR = BLACK;	
			POINT_COLOR = GRAY;	
			LCD_Show_Image(x, y, width, high, gImage_water_temperature);	
			
			if(nLangue ==LANGUAGE_CH)
			{
				LCD_ShowString(48, 36, 32, 12, 12, "TEMP");
			}
			else
			{
				LCD_ShowString(48, 36, 32, 12, 12, "TEMP");
			}

			x = 84;
			y = 38;
			width = 20;
			high = 20;
			LCD_Show_Image(x, y, width, high, gImage_depth_max);	
			if(nLangue ==LANGUAGE_CH)
			{
				LCD_ShowString(106, 36, 40, 12, 12, "DEPTH");	
			}
			else
			{
				LCD_ShowString(106, 36, 40, 12, 12, "DEPTH");
			}

			ReadRecordGetHeadInfo(ConvertOffset(RecordIndex),&tmpDiveHead);
			//readDiveRecord(&dive_tmp, index);
			printf("data is %d/%d/%d \r\n", dive_tmp.diveDate.year,dive_tmp.diveDate.month,dive_tmp.diveDate.date);		
			printf("time is %d:%d:%d \r\n", dive_tmp.currentTime.hr,dive_tmp.currentTime.min,dive_tmp.currentTime.sec);		
			printf("num is %d \r\n", dive_tmp.recordNumber);

			BACK_COLOR = DARKBLUE;
			LCD_Fill(27, 18, 27+30, 18+16, BACK_COLOR);	
			x = 28;
			y = 20;
			width = 6;
			high = 9;
			LCD_Show_Image(x, y, width, high, gImage_num_16[RecordIndex/100]);
			LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[RecordIndex/10]);
			LCD_Show_Image(x+2*width+4, y, width, high, gImage_num_16[RecordIndex%10]);

			x = 58;
			y = 20;
			width = 6;
			high = 9;
			BACK_COLOR = BLACK;	
			POINT_COLOR = WHITE;
			LCD_Show_Image(x, y, width, high, gImage_num_16[tmpDiveHead.m_DiveDate.year/10]);
			LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmpDiveHead.m_DiveDate.year%10]);
			LCD_ShowChar(x+2*width+2, 19, '/', 12);
			LCD_Show_Image(x+20, y, width, high, gImage_num_16[tmpDiveHead.m_DiveDate.month/10]);	
			LCD_Show_Image(x+width+22, y, width, high, gImage_num_16[tmpDiveHead.m_DiveDate.month%10]);	
			LCD_ShowChar(x+2*width+22, 19, '/', 12);
			LCD_Show_Image(x+40, y, width, high, gImage_num_16[tmpDiveHead.m_DiveDate.date/10]);	
			LCD_Show_Image(x+width+42, y, width, high, gImage_num_16[tmpDiveHead.m_DiveDate.date%10]);	

			x = 119;
			y = 20;
			width = 6;
			high = 9;
			LCD_Show_Image(x, y, width, high, gImage_num_16[tmpDiveHead.m_DiveTime.hr/10]);
			LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmpDiveHead.m_DiveTime.hr%10]);
			LCD_Show_Image(x+2*width+4, y, 2, high, gImage_16dian22);
			LCD_Show_Image(x+20, y, width, high, gImage_num_16[tmpDiveHead.m_DiveTime.min/10]);	
			LCD_Show_Image(x+width+22, y, width, high, gImage_num_16[tmpDiveHead.m_DiveTime.min%10]);	


			x = 143;
			y = 104;
			width = 6;
			high = 9;
			LCD_Show_Image(x, y, width, high, gImage_num_16[(tmpDiveHead.m_DiveSpan /60 ) / 10]);
			LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[(tmpDiveHead.m_DiveSpan / 60) % 10]);
			LCD_Show_Image(x+2*width+4, y, 2, high, gImage_16dian22);
			LCD_Show_Image(x+20, y, width, high, gImage_num_16[(tmpDiveHead.m_DiveSpan % 60 ) / 10]);	
			LCD_Show_Image(x+width+22, y, width, high, gImage_num_16[(tmpDiveHead.m_DiveSpan % 60 ) % 10]);



			x = 48;
			y = 50;
			width = 6;
			high = 9;
			//BACK_COLOR = BLACK;	
			//POINT_COLOR = ORANGE;
			tmp = tmpDiveHead.m_Temperature%100;
			LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/10]);
			LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%10]);
			//LCD_ShowChar(x+2*width+4, 46, 'o', 12);	

			//℃or F
			if(nLangue ==LANGUAGE_CH)
			{
				LCD_Show_Image(68, 48, 12,12, gImage_Temp_Symbol_ch);
			}
			else
			{
				LCD_Show_Image(68, 48, 12,12, gImage_Temp_Symbol_en);	 
			}

			BACK_COLOR = BLACK;	
			POINT_COLOR = WHITE;

			x = 105;
			y = 50;
			width = 6;
			high = 9;
			tmp = tmpDiveHead.m_DepthMax;
			LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/100]);
			LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%100/10]);
			LCD_Show_Image(x+2*width+4, y, 2, high, gImage_16dian11);
			LCD_Show_Image(x+19, y, width, high, gImage_num_16[tmp%10]);	
			LCD_ShowChar(x+27, 49, 'm', 12);	
#if 0	// open later when full test
			LCD_Fill(27, 60, 153, 103, BLACK);				

			u8 i=0;
			for(i=0;i<126;i++){
				if(dive_tmp.depth[i]-900>103)
					LCD_Draw_Point(i+27, 103);	
				else
					LCD_Draw_Point(i+27, dive_tmp.depth[i]-900);	
			}
#else
			LCD_Fill(27, 60, 153, 103, BLACK);
			u8 Long_step = tmpDiveHead.m_PointNum / (153 - 27 + 1);
			u8 Depth_step = tmpDiveHead.m_DepthMax / (103 - 60 + 1);
			u16 tmpPointIndex;
			Position_Point tmpPoint;
			if(Long_step < 1)
			{
				Long_step = 1;
			}
			if(Depth_step < 1)
			{
				Depth_step = 1;
			}
			for(int i = 0;i < (153 - 27 + 1);i++)
			{
			
				tmpPointIndex = i * Long_step;
				if(tmpPointIndex >= tmpDiveHead.m_PointNum)
				{
					break;
				}
				ReadRecordGetPointInfo(ConvertOffset(RecordIndex),tmpPointIndex,&tmpPoint);
				
				LCD_Draw_Point(i+27, tmpPoint.m_Depth * Depth_step);	
			}
#endif			
		}
		else
		{
			LCD_ShowString(31, 48, 120, 16, 16, "NO DIVE RECORD");
		} 
		
		flush_enable = 1;
	}

}


