#ifndef __DISPLAY_H
#define __DISPLAY_H
/********************************************************************************	 
 * 界面显示代码	
 * 正点原子@ALIENTEK
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
#include "main_ui.h"


#define  PRESS_KEY_TIME  250
#define OLED_SCREEN_FULL 0 	//是否满屏

//界面类型
enum ui_e
{
	MAIN_UI,
	MENU_UI,
	MODE_UI,
	SET_UI,	
	LOGGER_UI,
	TIME_UI,
	DEPTH_UI,
	MATCH_UI,
	RESET_UI,
	LANGUAGE_UI,
	ENDDIVE_UI,
	UPDATE_UI,
	GPS_UI,
};

typedef struct display_data_t_s {
		char temp;
		RTC_TimeTypeDef diving_time; 
		RTC_DateTypeDef zebra_date; 		
		RTC_TimeTypeDef zebra_time;
		int depth;
		int no_deco_min;
		uint16_t bat_Value;
		uint8_t bat_Status;
		uint8_t oxygen;
		uint8_t oxygenPwr;
		char compass;
} display_data_t;

void setShow_ui(enum ui_e ui);
void displayTask(void* param);
//void displayTask(void);
void displayInit(void);
void set_display_data_temp(char temp);
void set_display_data_diving_time(u32 diving_time);
void set_display_data_zebra_date(RTC_DateTypeDef zebra_data);
void set_display_data_zebra_time(RTC_TimeTypeDef zebra_time);
void set_display_data_depth(int depth);
void set_display_data_no_deco_min(int no_deco_min);
void set_display_data_no_bat_state(uint16_t bat);
void set_display_data_no_bat_status(uint8_t bat);
void set_display_data_no_oxygen_State(uint8_t oxygen,uint8_t power);
void set_display_data_compass(char compass);
void get_display_data(display_data_t * show_data);


#endif /*__DISPLAY_H*/
