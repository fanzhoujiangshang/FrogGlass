#include "displaytask.h"
#include "main_ui.h"
#include "time_ui.h"
#include "reset_ui.h"
#include "language_ui.h"
#include "match_ui.h"
#include "depth_ui.h"
#include "menu_display.h"
#include "logger_display.h"
#include "mode_display.h"
#include "set_display.h"
#include "endDive_ui.h"
#include "update_ui.h"
#include "gps_ui.h"
#include "global.h"
#if OLED_SCREEN
#include "oled.h"
#else 
#include "lcd.h"
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "keyTask.h"
#include "Gpio_control.h"
#include "print.h"
#include "storageTask.h"
#include "radiolink.h"
/********************************************************************************	 
 * 界面显示代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
static enum ui_e show_ui = MAIN_UI;
static display_data_t display_data;
static u8 logger_index;
static u8 time_step = 20;
static u8 depth_step = 20;
/*设置显示界面*/
void setShow_ui(enum ui_e ui)
{
#if OLED_SCREEN
	oled_fill(0,0,127,63,0);
#else
	LCD_Clear(BLACK);		   	//清屏
	flush_enable = 0;
#endif
	show_ui = ui;
}

#if KEY_HANDLER_NEW
void DispSwitchByKey()
{
	u8 keyState;
	
	if(BT_Is_FW_Updating())
	{
		DiveRecordStopFromOutside();
		if(show_ui != UPDATE_UI)
		{
			setShow_ui(UPDATE_UI);
		}
		return;
	}	
	keyState = getKeyStateNew();
	if(keyState == KEY_CC_LONGLONG_PRESS)
	{
		DiveRecordStopFromOutside();
		LCD_Clear(BLACK);		   	//清屏	
		LCD_RST(0);
		vTaskDelay(20);
		WritePowerControl(0);
		Jump_Bootloader(TO_BOOTLOADER_STANDBY);
	}
	switch(show_ui)
	{
		case MAIN_UI:
			if(keyState == KEY_CC_DOUBLE_PRESS) 
			{
				setShow_ui(MENU_UI);
				printf("arrive here ++++\r\n");
			}
			break;
		case MENU_UI:
			if(keyState == KEY_CC_SHORT_PRESS) //-----------------c-----------
			{
				logger_index = 0;
				setShow_ui(LOGGER_UI);	
			}
			else if(keyState == KEY_BB_DOUBLE_PRESS) //-----------------b-----------
			{
				setShow_ui(MAIN_UI);	
			}
			else if(keyState == KEY_BB_SHORT_PRESS) //-----------------b-----------
			{
				setShow_ui(SET_UI);	
			}	
			else if(keyState == KEY_AA_DOUBLE_PRESS) //-----------------a-----------
			{
				setShow_ui(MAIN_UI);	
			}
			else if(keyState == KEY_AA_SHORT_PRESS) //-----------------a-----------
			{
				if(DIVE_MODE_NONE != g_nCurrentDiveMode)
				{
					setShow_ui(ENDDIVE_UI);	
				}
				else
				{
					setShow_ui(MODE_UI);	
				}
			}				
			break;
		case MODE_UI:			
			if(keyState == KEY_CC_DOUBLE_PRESS) //-----------------c-----------
			{
				setShow_ui(MAIN_UI);	
			}
			else if(keyState == KEY_CC_SHORT_PRESS) //-----------------c-----------
			{
				time_step = 20;
				g_nDiveMode_Tmp = DIVE_MODE_NIX;
				setShow_ui(TIME_UI);	
			}
			else if(keyState == KEY_BB_DOUBLE_PRESS) //-----------------b-----------
			{
				setShow_ui(MAIN_UI);	
			}
			else if(keyState == KEY_BB_SHORT_PRESS) //-----------------b-----------
			{
				time_step = 20;
				g_nDiveMode_Tmp = DIVE_MODE_MIX;
				setShow_ui(TIME_UI);	
				
			}	
			else if(keyState == KEY_AA_DOUBLE_PRESS) //-----------------a-----------
			{
				setShow_ui(MENU_UI);	
			}
			else if(keyState == KEY_AA_SHORT_PRESS) //-----------------a-----------
			{
				time_step = 20;
				g_nDiveMode_Tmp = DIVE_MODE_AIR;
				setShow_ui(TIME_UI);	
				
			}
			break;	
		case SET_UI:			
			if(keyState == KEY_CC_DOUBLE_PRESS) //-----------------c-----------
			{
				setShow_ui(MENU_UI);	
			}
			else if(keyState == KEY_CC_SHORT_PRESS) //-----------------c-----------
			{
				setShow_ui(RESET_UI);	
			}
			else if(keyState == KEY_BB_DOUBLE_PRESS) //-----------------b-----------
			{
				setShow_ui(MAIN_UI);	
			}
			else if(keyState == KEY_BB_SHORT_PRESS) //-----------------b-----------
			{
				setShow_ui(LANGUAGE_UI);	
			}	
			else if(keyState == KEY_AA_DOUBLE_PRESS) //-----------------a-----------
			{
				setShow_ui(MENU_UI);	
			}
			else if(keyState == KEY_AA_SHORT_PRESS) //-----------------a-----------
			{
				setShow_ui(MATCH_UI);	
			}			
			break;
		case LOGGER_UI:
			if(keyState == KEY_CC_SHORT_PRESS)
			{
				if(g_RecordIndexInfo.m_TotalRecordNum > 0)
				{
					logger_index++;
					if (logger_index >= g_RecordIndexInfo.m_TotalRecordNum)
					{
						logger_index = 0;	
					}
				}
				setShow_ui(LOGGER_UI);
			}
			else if(keyState==KEY_BB_SHORT_PRESS)
			{
				if(g_RecordIndexInfo.m_TotalRecordNum > 0)
				{
					if(logger_index == 0)
					{
						logger_index = g_RecordIndexInfo.m_TotalRecordNum - 1;
					}
					else
					{
						logger_index--;
					}
				}
				setShow_ui(LOGGER_UI);
			}
			else if(keyState==KEY_AA_DOUBLE_PRESS)
			{
				setShow_ui(MENU_UI);			
			}				
			break;
		case TIME_UI:
			if(keyState == KEY_CC_SHORT_PRESS || keyState == KEY_CC_LONG_PRESS)
			{	
				if(time_step <= 0)
				{
					time_step = 99;
				}
				else
				{
					time_step --;
				}
				setShow_ui(TIME_UI);
			}else if(keyState==KEY_BB_SHORT_PRESS || keyState == KEY_BB_LONG_PRESS)
			{
				if(time_step >= 99)
				{
					time_step = 0;
				}
				else
				{
					time_step ++;
				}
				setShow_ui(TIME_UI);
			}else if(keyState==KEY_AA_SHORT_PRESS)
			{
				depth_step = 20;
				setShow_ui(DEPTH_UI);	
			}				
			break;
		case DEPTH_UI:
			if(keyState == KEY_CC_SHORT_PRESS || keyState == KEY_CC_LONG_PRESS)
			{	
				if(depth_step <= 0)
				{
					depth_step = 99;
				}
				else
				{
					depth_step --;
				}	 
			}else if(keyState==KEY_BB_SHORT_PRESS || keyState == KEY_BB_LONG_PRESS)
			{
				if(depth_step >= 99)
				{
					depth_step = 0;
				}
				else
				{
					depth_step ++;
				}			
			}else if(keyState==KEY_AA_SHORT_PRESS) 
			{
				setShow_ui(MAIN_UI);	
				g_nCurrentDiveMode = g_nDiveMode_Tmp;
			}			
			break;				
		case MATCH_UI:	
			if(keyState == KEY_CC_DOUBLE_PRESS) //-----------------c-----------
			{
				setShow_ui(MENU_UI);	
			}
			/*else if(keyState == KEY_CC_SHORT_PRESS) //-----------------c-----------
			{
				setShow_ui(TIME_UI);	
			}*/
			else if(keyState == KEY_BB_DOUBLE_PRESS) //-----------------b-----------
			{
				setShow_ui(MAIN_UI);	
			}
			/*else if(keyState == KEY_BB_SHORT_PRESS) //-----------------b-----------
			{
				setShow_ui(TIME_UI);	
			}*/	
			else if(keyState == KEY_AA_DOUBLE_PRESS) //-----------------a-----------
			{
				setShow_ui(SET_UI);	
			}
			/*else if(keyState == KEY_AA_SHORT_PRESS) //-----------------a-----------
			{
				setShow_ui(TIME_UI);	
			}*/			
			break;					
		case RESET_UI:		
			if(keyState == KEY_CC_DOUBLE_PRESS) //-----------------c-----------
			{
				setShow_ui(MENU_UI);	
			}
			else if(keyState == KEY_CC_SHORT_PRESS) //-----------------c-----------
			{
				setShow_ui(SET_UI);	
			}
			else if(keyState == KEY_BB_DOUBLE_PRESS) //-----------------b-----------
			{
				setShow_ui(MAIN_UI);	
			}
			else if(keyState == KEY_BB_SHORT_PRESS) //-----------------b-----------
			{
				MX25Lxxx_WriteConfigDefault();
				setShow_ui(MAIN_UI);	
			}	
			else if(keyState == KEY_AA_DOUBLE_PRESS) //-----------------a-----------
			{
				setShow_ui(SET_UI);	
			}
		/*	else if(keyState == KEY_AA_SHORT_PRESS) //-----------------a-----------
			{
				setShow_ui(TIME_UI);	
			}			*/
			break;	
		case LANGUAGE_UI:	
			if(keyState == KEY_CC_DOUBLE_PRESS) //-----------------c-----------
			{
				setShow_ui(MENU_UI);	
			}
			else if(keyState == KEY_CC_SHORT_PRESS) //-----------------c-----------
			{
				MX25Lxxx_WriteLanguage(LANGUAGE_ENG);
				setShow_ui(LANGUAGE_UI);	
			}
			else if(keyState == KEY_BB_DOUBLE_PRESS) //-----------------b-----------
			{
				setShow_ui(MAIN_UI);	
			}
			else if(keyState == KEY_BB_SHORT_PRESS) //-----------------b-----------
			{
				MX25Lxxx_WriteLanguage(LANGUAGE_CHI);
				setShow_ui(LANGUAGE_UI);
			}
			else if(keyState == KEY_AA_DOUBLE_PRESS) //-----------------a-----------
			{
				setShow_ui(SET_UI);	
			}
			else if(keyState == KEY_AA_SHORT_PRESS) //-----------------a-----------
			{
				//MX25Lxxx_WriteConfigDefault();
				//setShow_ui(MATCH_UI);	
				//NVIC_SystemReset();
			}				
			break;
		case ENDDIVE_UI:
			if(keyState == KEY_AA_SHORT_PRESS)
			{
				g_nCurrentDiveMode = DIVE_MODE_NONE;
				setShow_ui(MODE_UI);		
			}
			else if(keyState==KEY_BB_SHORT_PRESS)
			{
				setShow_ui(MAIN_UI);					
			}

			break;
		case UPDATE_UI:
			break;
		default:
			break;
	}
}
#endif //#if KEY_HANDLER_NEW
void DispSwitchByKeyLongpress(u8 KeyCode)
{
	//u8 tmpValue;
	switch(KeyCode)
	{
		case KEY_CC_LONGLONG_PRESS:
			LCD_Clear(BLACK);		   	//清屏	
			LCD_RST(0);
			vTaskDelay(20);
			WritePowerControl(0);
			Jump_Bootloader(TO_BOOTLOADER_STANDBY);
			break;
		case KEY_BB_LONGLONG_PRESS:
			break;
		case KEY_AA_LONGLONG_PRESS:
			break;
		default:
			break;
	}
}
/*显示任务*/
void displayTask(void* param)
//void displayTask(void)
{
	while(1)
	{
		vTaskDelay(100);	
		/* 每过1s 更新一次时间*/
		switch(show_ui)
		{
			case MAIN_UI:
				main_ui();
				break;
			case MENU_UI:
				menu_ui();
				break;
			case MODE_UI:
				mode_ui();
				break;	
			case SET_UI:
				set_ui();				
				break;
			case LOGGER_UI:
				logger_ui_new(logger_index);
				break;
			case TIME_UI:
				time_ui_new(time_step);
				break;
			case DEPTH_UI:
				depth_ui_new(depth_step);
				break;				
			case MATCH_UI:
				match_ui();							
				break;					
			case RESET_UI:
				reset_ui();							
				break;	
			case LANGUAGE_UI:
				language_ui();						
				break;	
			case ENDDIVE_UI:
				endDive_ui(0);
				break;
			case UPDATE_UI:
				update_ui(BT_Fetch_Percent());
				break;
			case GPS_UI:
				gps_ui();							
				break;	
			default:break;
		}
#if KEY_HANDLER_NEW
		DispSwitchByKey();
#endif
#if OLED_SCREEN		
	oled_refreshGram();
#endif
	}
}

void set_display_data_temp(char temp)
{
	display_data.temp = temp;
}

void set_display_data_diving_time(u32 totalSeconds)
{
	display_data.diving_time.Minutes = totalSeconds / 60; 
	display_data.diving_time.Seconds = totalSeconds % 60;
}

void set_display_data_zebra_date(RTC_DateTypeDef zebra_date)
{
	display_data.zebra_date.Year = zebra_date.Year; 
	display_data.zebra_date.Month= zebra_date.Month; 
	display_data.zebra_date.Date = zebra_date.Date;
}

void set_display_data_zebra_time(RTC_TimeTypeDef zebra_time)
{
	display_data.zebra_time.Hours = 	zebra_time.Hours; 
	display_data.zebra_time.Minutes = 	zebra_time.Minutes; 
	display_data.zebra_time.Seconds = 	zebra_time.Seconds;
}

void set_display_data_depth(int depth)
{
	display_data.depth = depth;
}

void set_display_data_no_deco_min(int no_deco_min)
{
	display_data.no_deco_min = no_deco_min;
}

void set_display_data_no_bat_state(uint16_t bat)
{
	display_data.bat_Value = bat;
}
void set_display_data_no_bat_status(uint8_t bat_Status)
{
	display_data.bat_Status = bat_Status;
}

void set_display_data_no_oxygen_State(uint8_t oxygen,uint8_t power)
{
	display_data.oxygen = oxygen;
	display_data.oxygenPwr = power;
}

void set_display_data_compass(char compass)
{
	display_data.compass = compass;
}

void get_display_data(display_data_t * show_data)
{
	show_data->temp =	display_data.temp;
	show_data->diving_time.Minutes =	display_data.diving_time.Minutes;	
	show_data->diving_time.Seconds =	display_data.diving_time.Seconds;	
	show_data->zebra_date.Year = display_data.zebra_date.Year;	
	show_data->zebra_date.Month = display_data.zebra_date.Month;
	show_data->zebra_date.Date = display_data.zebra_date.Date;		
	show_data->zebra_time.Hours = display_data.zebra_time.Hours;	
	show_data->zebra_time.Minutes = display_data.zebra_time.Minutes;
	show_data->zebra_time.Seconds = display_data.zebra_time.Seconds;	
	show_data->depth =	display_data.depth;
	show_data->no_deco_min =	display_data.no_deco_min;
	show_data->bat_Value =	display_data.bat_Value;
	show_data->bat_Status =	display_data.bat_Status;
	show_data->oxygen =	display_data.oxygen;
	show_data->oxygenPwr =	display_data.oxygenPwr;
	show_data->compass =	display_data.compass;
}



/*界面显示初始化*/
void displayInit(void)
{
	show_ui = MAIN_UI;
	logger_index = 0;
	time_step = 20;
	depth_step = 20;
}








