#include <stdio.h>
#include "main_ui.h"
#include "displayTask.h"
#include "font.h"
#include "keyTask.h"
#include "pmTask.h"
#include "oxygenTask.h"
#include "global.h"
#if OLED_SCREEN
#include "oled.h"
#else
#include "lcd.h"
#include "zqjqr_log.h"
#endif
#include "text.h"
#include "FreeRTOS.h"
#include "task.h"
#include "rtc.h"
#include "sensorsTask.h"
#include "radiolink.h"
/********************************************************************************	 
 * 主界面代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

float CompensaveResult(void);

#define  LOW_POWER  	3.25f
#define  FULL_POWER		4.15f
const unsigned short direct_Degree[][2] ={
	{0,23},				// 0, 23
	{0+23,	90-22},		// 23  23+45
	{90-22,	90+23},		// 90 - 10,90 + 10
	{90+23,	180-22},		// 
	{180-22,	180+23},		// 180 - 10,180 + 10
	{180+23,	270-22},		// 180 - 10,180 + 10
	{270-22,	270+23},		// 
	{270+23,360-22},		// 270 - 10,270 + 10
	{360-22,360},		// 	
};
const unsigned char* direct_Point_Ch[] ={
	{"北"},
	{"东北"},
	{"东"},
	{"东南"},
	{"南"},
	{"西南"},
	{"西"},
	{"西北"},
	{"北"},
};
const unsigned char* direct_Point_En[]	={
	{"N"},
	{"NE"},
	{"E"},
	{"SE"},
	{"S"},
	{"SW"},
	{"W"},
	{"NW"},
	{"N"},
};
const unsigned char* stepLanguage1[8]={
{"北"},
{"东北"},
{"东"},
{"东南"},
{"南"},
{"西南"},
{"西"},
{"西北"},
};

static u16 bat_old_width = 0xff;
static u16 oxy_old_width = 0xff;
static u16 direction_update_step = 0;

void main_ui(void)
{
#if OLED_SCREEN
	int x,y,i,j,k;
		
	rtc_time_t diving_time; 
	rtc_time_t zebra_time;

	display_data_t data;

	get_display_data(&data);
	
//	oled_fill(0,0,127,63,0);
	x = 16;
	y = 10;
	i=6;
	oled_showPicture(0,0,bmp_logo,127,63);	
	
	oled_showNum(x,y,data.temp/10,1,6,12);				//温度
	oled_showNum(x+i,y,data.temp%10,1,6,12);	
	oled_showPicture(x+2+2*i,y+2,bmp_temp1,8,8);

	y = 29;

	//j = rtcTime.min -init_rtcTime.min;
	j = data.diving_time.min;
	oled_showNum(x,y,(u8)(j/10),1,6,12);				//潜水时间
	oled_showNum(x+i,y,(u8)(j%10),1,6,12);	
	oled_showChar(x+2*i,y+2,':',6,8,1);	
	oled_showNum(x+3*i,y,(u8)data.diving_time.sec/10,1,6,12);			
	oled_showNum(x+4*i,y,(u8)(data.diving_time.sec%10),1,6,12);	


	y = 46;

	oled_showNum(x,y,(u8)(data.zebra_time.hr/10),1,6,12);				//时间
	oled_showNum(x+i,y,(u8)(data.zebra_time.hr%10),1,6,12);	
	oled_showChar(x+2*i,y+2,':',6,8,1);	
	oled_showNum(x+3*i,y,(u8)data.zebra_time.min/10,1,6,12);			
	oled_showNum(x+4*i,y,(u8)(data.zebra_time.min%10),1,6,12);

	x=56;
	y = 18;	
	i=6;
	oled_showNum(x,y,data.depth/100,1,6,12);													//深度
	oled_showNum(x+i,y,data.depth%100/10,1,6,12);	
	oled_showChar(x+2*i,y+2,'.',6,8,1);	
	oled_showNum(x+3*i,y,data.depth%10,1,6,12);
	oled_showChar(x+4*i,y+2,'m',6,8,1);

	y = 46;	
	i=6;
	oled_showNum(x,y,data.no_deco_min/100,1,6,12);													//无减压时间
	oled_showNum(x+i,y,data.no_deco_min%100/10,1,6,12);		
	oled_showNum(x+2*i,y,data.no_deco_min%10,1,6,12);

	switch (data.bat_state)
	{
		case BATTERY_FULL:
			oled_showPicture(106,3,bmp_battery1[3],21,8);
			break;	
		case BATTERY_OK:
			oled_showPicture(106,3,bmp_battery1[2],21,8);
			break;
		case BATTERY_WARNING:
			oled_showPicture(106,3,bmp_battery1[1],21,8);	
			break;
		case BATTERY_CRITICAL:
			oled_showPicture(106,3,bmp_battery1[0],21,8);	
			break;
		default:
			oled_showPicture(106,3,bmp_battery1[0],21,8);	
			break;
	}	

	switch (data.oxygen_State)
	{
		case OXYGEN_FULL:
			oled_showPicture(106,16,bmp_oxygen[4],15,29);
			break;			
		case OXYGEN_OK:
			oled_showPicture(106,16,bmp_oxygen[3],15,29);
			break;
		case OXYGEN_WARNING:
			oled_showPicture(106,16,bmp_oxygen[2],15,29);		
			break;
		case OXYGEN_CRITICAL:
			oled_showPicture(106,16,bmp_oxygen[1],15,29);
			break;
		default:
			oled_showPicture(106,16,bmp_oxygen[0],15,29);
			break;
	}
	
	show_str(105,50,stepLanguage1[data.compass],12,12,1);	
#else
#if OLED_SCREEN_FULL
	int x,y,width,high,tmp;	

	display_data_t data;

	get_display_data(&data);
	if(flush_enable == 0)
	{
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;	
		x = 3;
		y = 18;
		width = 26;
		high = 26;
		LCD_Show_Image(x, y, width, high, gImage_depth);
		LCD_ShowString(31, 5, 64, 16, 16, "DEPTH(m)");
		LCD_ShowString(31, 48, 64, 16, 16, "NO DECO");
		x = 2;
		y = 66;
		width = 20;
		high = 20;
		LCD_Show_Image(x, y, width, high, gImage_warn);
		x = 2;
		y = 94;
		LCD_Show_Image(x, y, width, high, gImage_water_temperature);	
		LCD_ShowString(24, 90, 32, 12, 12, "TEMP");
		x = 59;
		y = 94;
		width = 20;
		high = 20;	
		LCD_Show_Image(x, y, width, high, gImage_diving_time);
		LCD_ShowString(81, 90, 30, 12, 12, "DUR");	
		x = 120;	
		y = 94;
		width = 20;
		high = 20;
		LCD_Show_Image(x, y, width, high, gImage_current_time);	
		LCD_ShowString(142, 90, 32, 12, 12, "TIME");
		x = 98;
		y = 25;
		width = 35;
		high = 14;
		LCD_Show_Image(x, y, width, high, gImage_oxygen_capacity);
		LCD_ShowString(99, 12, 32, 12, 12, "NIX");
		x = 138;
		LCD_Show_Image(x, y, width, high, gImage_electricity);	
		LCD_ShowString(140, 12, 32, 12, 12, "BAT");		
		x = 98;
		y = 40;
		width = 35;
		high = 3;
		LCD_Show_Image(x, y, width, high, gImage_oxygen_power);	
		x = 102;
		y = 55;
		width = 26;
		high = 26;
		LCD_Show_Image(x, y, width, high, gImage_diving);	
		x = 143;
		y = 70;
		width = 10;
		high = 10;
		LCD_Show_Image(x, y, width, high, gImage_direction);
		
		flush_enable = 1;	
	}
	
	x = 31;
	y = 21;
	width = 13;
	high = 20;
	LCD_Show_Image(x, y, width, high, gImage_num_32[data.depth/100]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_32[data.depth%100/10]);
	LCD_Show_Image(x+2*width+4, y, 4, high, gImage_dian11);
	LCD_Show_Image(x+36, y, width, high, gImage_num_32[data.depth%10]);	

	static int old_width = 0;
	x = 102;
	y = 28;
	width = data.oxygen/4;
	high = 7;

	if(width > 20){
		width = 20;
	}

	if(old_width != width){
		LCD_Fill(x, y, x+20, y+high, BLACK);	
		LCD_Fill(x, y, x+width, y+high, LIGHTBLUE);
		old_width = width;	
	}

	
	static int old1_width = 0;
	x = 142;
	y = 28;
	width = data.bat/0xf9;
	high = 7;

	if(width > 20){
		width = 20;
	}

	if(old1_width != width){
		LCD_Fill(x, y, x+25, y+high, BLACK);	
		LCD_Fill(x, y, x+width, y+high,GREEN);		
		old1_width = width;	
	}	
	
	x = 31;
	y = 65;
	width = 13;
	high = 20;
	LCD_Show_Image(x, y, width, high, gImage_num_32[data.no_deco_min/100]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_32[data.no_deco_min%100/10]);
	LCD_Show_Image(x+2*width+4, y, width, high, gImage_num_32[data.no_deco_min%10]);	
	x = 24;
	y = 102;
	width = 6;
	high = 9;
	LCD_Show_Image(x, y, width, high, gImage_num_16[data.temp/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[data.temp%10]);
	x = 81;
	y = 102;
	width = 6;
	high = 9;
	tmp = data.diving_time.Minutes;
	LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%10]);
	LCD_Show_Image(x+2*width+4, y, 2, high, gImage_16dian22);
	tmp = data.diving_time.Seconds;
	LCD_Show_Image(x+20, y, width, high, gImage_num_16[tmp/10]);	
	LCD_Show_Image(x+width+22, y, width, high, gImage_num_16[tmp%10]);
	x = 142;
	y = 102;
	width = 6;
	high = 9;
	tmp = data.zebra_time.Hours;
	LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%10]);
	LCD_Show_Image(x+2*width+4, y, 2, high, gImage_16dian22);
	tmp = data.zebra_time.Minutes;
	LCD_Show_Image(x+20, y, width, high, gImage_num_16[tmp/10]);	
	LCD_Show_Image(x+width+22, y, width, high, gImage_num_16[tmp%10]);

	x = 139;
	y = 50;
	width = 32;
	high = 16;
	BACK_COLOR = BLACK;	
	POINT_COLOR = GREEN;

	float yaw = state.attitude.yaw+180;
	static int old_tmp = 0;
	if((yaw >355)||(yaw<5)){
		tmp = 6;
	}else if((yaw >5)&&(yaw<85)){
		tmp = 7;
	}else if((yaw >85)&&(yaw<95)){
		tmp = 0;
	}else if((yaw >95)&&(yaw<175)){
		tmp = 1;
	}else if((yaw > 175)&&(yaw<185)){
		tmp = 2;
	}else if((yaw > 185)&&(yaw<265)){
		tmp = 3;
	}else if((yaw > 265)&&(yaw<275)){
		tmp = 4;
	}else if((yaw > 275 )&&(yaw<355)){
		tmp = 5;
	}

	if(old_tmp != tmp){
		LCD_Fill(x, y, x+width, y+high, BLACK);
		old_tmp = tmp;	
	}
	Show_Str(x,y,width,high,stepLanguage1[tmp],16);	
	x = 155;
	y = 70;
	width = 6;
	high = 9;
	LCD_Show_Image(x, y, width, high, gImage_num_16[(int)yaw%90/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[(int)yaw%90%10]);	



#if 0
	LCD_Fill(0, 0, 180, 120, WHITE);	
	LCD_Fill(27, 18, 27+126, 18+84, GREEN);
	LCD_Fill(36, 24, 36+108, 24+72, RED);
       LCD_Fill(41, 27, 41+99, 27+66, YELLOW);
	LCD_Fill(45, 30, 45+90, 30+60, BLUE);	
	vTaskDelay(4000);	
#endif

#else	// #if OLED_SCREEN_FULL
	u16 x,y,width,high;
	int tmp;	

	display_data_t Disp_Data;

	get_display_data(&Disp_Data);
	
	int nLangue = m_ConfigParams.m_Language;  //根据用户配置设置界面
	
	if(flush_enable == 0)
	{
		BACK_COLOR = BLACK;	
		POINT_COLOR = GRAY;
		x = 27;	
		y = 18;
		width = 20;
		high = 20;
		
		LCD_Show_Image(x, y, width, high, gImage_current_time);
		if(nLangue ==LANGUAGE_CH)
		{
			//LCD_ShowString(48, 16, 32, 12, 12, "TIME");
		}
		else
		{
			LCD_ShowString(48, 16, 32, 12, 12, "TIME");		
		}
		
		
		if(nLangue ==LANGUAGE_CH)
		{
			y = 39;
			//LCD_ShowString(48, 38, 30, 12, 12, "DUR");	
		}
		else
		{
			y = 39;
			LCD_ShowString(48, 38, 30, 12, 12, "DUR");	
		}
		LCD_Show_Image(x, y, width, high, gImage_diving_time);

		if(nLangue ==LANGUAGE_CH)
		{
			y = 62;
		}
		else
		{
			y = 66;
		}
		LCD_Show_Image(x, y, width, high, gImage_water_temperature);	

		//℃or F
		if(nLangue ==LANGUAGE_CH)
		{
			//LCD_ShowString(48, 60, 32, 12, 12, "TEMP");
			LCD_Show_Image(74, 70, 12,12, gImage_Temp_Symbol_ch);
		}
		else
		{
			LCD_ShowString(48, 60, 32, 12, 12, "TEMP");
			LCD_Show_Image(74, 70, 12,12, gImage_Temp_Symbol_en);	 
		}

		x = 32 - 3;		// zhengl
		y = 88;
		width = 10;
		high = 10;
		LCD_Show_Image(x, y, width, high, gImage_direction);	
		
		x = 86;
		width = 35;
		high = 14;
		y = 28;
		if(nLangue ==LANGUAGE_CH)
		{
			
			//LCD_ShowString(86, 16, 32, 12, 12, "NIX");
		}
		else
		{
			LCD_ShowString(86, 16, 32, 12, 12, "NIX");
		}
		
		if(DIVE_MODE_NONE == g_nCurrentDiveMode)
		{
			LCD_Show_Image(x, y, width, high, gImage_oxygen_capacity);	//混合	
		}
		else if(DIVE_MODE_AIR== g_nCurrentDiveMode)
		{
			LCD_Show_Image(x, y, width, high, gImage_oxygen_capacity_air);	//空气	
		}
		else if(DIVE_MODE_MIX== g_nCurrentDiveMode)
		{
			LCD_Show_Image(x, y, width, high, gImage_oxygen_capacity);	//空气	
		}
		else if(DIVE_MODE_NIX == g_nCurrentDiveMode)
		{
			LCD_Show_Image(x, y, width, high, gImage_oxygen_capacity_high);	//高氧	
		}
		x = 121;
		y = 28;
		if(nLangue ==LANGUAGE_CH)
		{

			//LCD_ShowString(121, 16, 32, 12, 12, "BAT");	
		}
		else
		{
			LCD_ShowString(121, 16, 32, 12, 12, "BAT");	
		}
		LCD_Show_Image(x, y, width, high, gImage_electricity);
		
		x = 87;
		y = 47;
		width = 26;
		high = 26;
		LCD_Show_Image(x, y, width, high, gImage_depth);
		if(nLangue ==LANGUAGE_CH)
		{
			//LCD_ShowString(113, 45, 64, 16, 16, "DEPTH");
		}
		else
		{
			LCD_ShowString(113, 45, 64, 16, 16, "DEPTH");
		}
		
		LCD_ShowString(146, 60, 20, 12, 12, "m");
		width = 20;
		high = 20;
		y = 78;
		if(nLangue ==LANGUAGE_CH)
		{
			x = 100;
		}
		else
		{
			x = 93;
		}
		LCD_Show_Image(x, y, width, high, gImage_warn);


		//bluetooth ico
		x = 120;
		y = 78;
		width = 14;
		high = 20;
		if(BT_QueryConnectedOrNot())
		{
			LCD_Show_Image(x, y, width, high, gImage_bl_connect);	
		}
		else
		{
			LCD_Show_Image(x, y, width, high, gImage_bl_disconnect);
		}

		
		x = 120+14;
		y = 74;
		width = 26;
		high = 26;
		LCD_Show_Image(x, y, width, high, gImage_diving);	

		bat_old_width = 0xff;
		oxy_old_width = 0xff;
		direction_update_step = 0;
		flush_enable = 1;	
	}
	
	//----------------------------------------------------------------------depth
	if(nLangue ==LANGUAGE_CH)
	{	
		x = 117;
		y = 61;
	}
	else
	{
		x = 113;
		y = 60;
	}
	
	width = 6;
	high = 9;
	tmp = Disp_Data.depth;
	if(tmp < 0)
	{
		tmp = 0;
	}
	LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/100]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%100/10]);
	LCD_Show_Image(x+2*width+4, y, 2, high, gImage_16dian11);
	LCD_Show_Image(x+2*width+8, y, width, high, gImage_num_16[tmp%10]);	
	//----------------------------------------------------------------------oxygen
	x = 90;
	y = 31;
	width = Disp_Data.oxygen / 5;
	high = 7;

	if(width > 20)
	{
		width = 20;
	}

	if(oxy_old_width != width)
	{
		if(width == 0)
		{
			LCD_Fill(x, 		y, 		x+20, 		y+high, BLACK);
			//LCD_Fill(x, y, x+width, y+high, LIGHTBLUE);
		}
		else if(width == 20)
		{
			LCD_Fill(x, 		y, 		x+20, 		y+high, LIGHTBLUE);	
			//LCD_Fill(x, y, x+width, y+high, LIGHTBLUE);
		}
		else
		{
			LCD_Fill(x, 		y, 		x+width, 		y+high, LIGHTBLUE);
			LCD_Fill(x+width, 	y, 		x+20, 		y+high, BLACK);				
		}
		oxy_old_width = width;	
	}
	//----------------------------------------------------------------------oxygen power
	x = 87;
	y = 45;
	width = 25;
	high = 3;

	width = Disp_Data.oxygenPwr /4;
	if(width > 25)
	{
		width = 25;
	}
	//LCD_Show_Image(x, y, width, high, gImage_oxygen_power);	
	switch(width)
	{
		case 0:
			POINT_COLOR = 0x02e0;
			LCD_DrawLine(x,y,x + 25,y);
			break;
		case 25:
			POINT_COLOR = GREEN;
			LCD_DrawLine(x,y,x + 25,y);
			break;
		default:
			POINT_COLOR = GREEN;
			LCD_DrawLine(x,y,x + width,y);
			POINT_COLOR = 0x02e0;
			LCD_DrawLine(x+width,y,x + 25,y);			
			break;
	}
	
	POINT_COLOR = GRAY;
	//----------------------------------------------------------------------battery
	Main_UI_ShowBattery(125,31,Disp_Data.bat_Value,Disp_Data.bat_Status);
	//----------------------------------------------------------------------date time
	x = 49;
	y = 71;
	width = 6;
	high = 9;

	char temp = Disp_Data.temp;
	
	if(nLangue ==LANGUAGE_EN)
	{
		temp=temp*1.8+32;
	}
	else
	{
	
	}
	
	#if 0
	LCD_Show_Image(x, y, width, high, gImage_num_16[Disp_Data.temp/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[Disp_Data.temp%10]);
	#else
	LCD_Show_Image(x, y, width, high, gImage_num_16[temp/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[temp%10]);
	#endif
//	LCD_Show_num_Image(x+2*width+4, y, width, high, gImage_num_16[2]);


	//current time
	x = 48;
	y = 28;
	width = 6;
	high = 9;
	tmp = Disp_Data.zebra_time.Hours ;
	LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%10]);
	LCD_Show_Image(x+2*width+4, y, 2, high, gImage_16dian22);
	tmp = Disp_Data.zebra_time.Minutes;
	LCD_Show_Image(x+20, y, width, high, gImage_num_16[tmp/10]);	
	LCD_Show_Image(x+width+22, y, width, high, gImage_num_16[tmp%10]);

	//diving time
	x = 48;
	y = 50;
	width = 6;
	high = 9;

	if(g_nCurrentDiveMode == DIVE_MODE_NONE)
	{
		tmp = 0;
	}
	else
	{
		tmp = Disp_Data.diving_time.Minutes;
	}
	
	LCD_Show_Image(x, y, width, high, gImage_num_16[tmp/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[tmp%10]);
	LCD_Show_Image(x+2*width+4, y, 2, high, gImage_16dian22);
	
	if(g_nCurrentDiveMode == DIVE_MODE_NONE)
	{
		tmp = 0;
	}
	else
	{
		tmp = Disp_Data.diving_time.Seconds;
	}

	LCD_Show_Image(x+20, y, width, high, gImage_num_16[tmp/10]);	
	LCD_Show_Image(x+width+22, y, width, high, gImage_num_16[tmp%10]);
	


	//------------------------------------------------------------------Magnetic
	if(direction_update_step > 0)
	{
		direction_update_step--;
		return;
	}
	else
	{
		direction_update_step = 4;
	}
	x = 42 - 3;	// zhengl
	y = 84;
	width = 32;
	high = 16;
	BACK_COLOR = BLACK;	
	POINT_COLOR = GREEN;

	float yaw;
#if 1	
	yaw = state.attitude.yaw+180;
	if(yaw > 360)
	{
		yaw -= 360;
	}
	if(yaw < 0)
	{
		yaw += 360;
	}
#else
	yaw = CompensaveResult();
#endif
	//printf("r:%6.2f p:%6.2f y:%6.2f   -r:%6.2f-\r\n",state.attitude.roll,state.attitude.pitch,state.attitude.yaw,yaw);	
	//printf("-r:%6.2f- \r\n",state.attitude.yaw);
	
	static int old_Index = 0;
	static int tmp_Index = 0;	
	for(tmp_Index = 0;tmp_Index < sizeof(direct_Degree);tmp_Index++)
	{
		if((yaw >= direct_Degree[tmp_Index][0])&&(yaw < direct_Degree[tmp_Index][1]))
		{
			break;
		}
	}
	
	if(old_Index != tmp_Index){
		LCD_Fill(x, y, x+width, y+high, BLACK);
		old_Index = tmp_Index;	
	}
	if(nLangue ==LANGUAGE_CH)
	{	
		Show_Str(x,y,width,high,direct_Point_Ch[tmp_Index],16);
	}
	else
	{
		Show_Str(x+5,y,width,high,direct_Point_En[tmp_Index],16);	
	}
			
	x = 77 - 3;	// zhengl
	y = 88;
	width = 6;
	high = 9;
	LCD_Show_Image(x, y, width, high, gImage_num_16[(int)yaw%90/10]);
	LCD_Show_Image(x+width+2, y, width, high, gImage_num_16[(int)yaw%90%10]);	

#endif

#endif

}

float CompensateMap_roll[][3] =
{
//		from			to		compenValue
		{80.00,		120.00,		-140.00},
		{60.00,		80.00,		-105.00},
		{40.00,		60.00,		-80.00},
		{25.00,		40.00,		-60.00},
		{15.00,		25.00,		-45.00},
		{5.00,		15.00,		-22.00},
		{-5.00,		5.00,		0.00},
		{-15.00,		-5.00,		5.00},	
		{-25.00,		-15.00,		10.00},
		{-40.00,		-25.00,		15.00},
		{-60.00,		-40.00,		18.00},
		{-80.00,		-60.00,		22.00},
		{-120.00,	-80.00,		32.00}
};
float CompensateMap_pitch[][3] =
{
//		from			to		compenValue
		{80.00,		120.00,		52.00},
		{60.00,		80.00,		42.00},
		{40.00,		60.00,		38.00},
		{25.00,		40.00,		25.00},
		{15.00,		25.00,		15.00},
		{5.00,		15.00,		5.00},
		{-5.00,		5.00,		0.00},
		{-15.00,		-5.00,		-5.00},	
		{-25.00,		-15.00,		-15.00},
		{-40.00,		-25.00,		-25.00},
		{-60.00,		-40.00,		-30.00},
		{-80.00,		-60.00,		-40.00},
		{-120.00,	-80.00,		-50.00}
};

float CompensaveResult(void)
{
	float pitch = state.attitude.pitch;
	float roll = state.attitude.roll;
	float yaw = state.attitude.yaw+180;
	float tmpfloat = state.attitude.yaw;
	for(int i = 0;i < sizeof(CompensateMap_roll);i++)
	{
		if((roll >= CompensateMap_roll[i][0])&&(roll < CompensateMap_roll[i][1]))
		{
			tmpfloat += CompensateMap_roll[i][2];
			break;
		}
	}
	for(int i = 0;i < sizeof(CompensateMap_pitch);i++)
	{
		if((pitch >= CompensateMap_pitch[i][0])&&(pitch < CompensateMap_pitch[i][1]))
		{
			tmpfloat += CompensateMap_pitch[i][2];
			break;
		}
	}	
	tmpfloat += 180;
	if(tmpfloat > 360)
	{
		tmpfloat -= 360;
	}
	if(tmpfloat < 0)
	{
		tmpfloat += 360;
	}	
	return tmpfloat;
}

#define	BAT_SAMPLE_BASE_VALUE					(2840)
#define	BAT_SAMPLE_STEP_VALUE					(27)
void Main_UI_ShowBattery(uint16_t dispX,uint16_t dispY,uint16_t bat_Value,uint8_t bat_Status)
{
	static uint8_t updateStep = 0;
	static int bat_show_state = 0;
	int x,y,width,high;
	int bat_show_begin = 0;

	updateStep++;
	if(updateStep < 4)
	{
		return;
	}
	else
	{
		updateStep = 0;
	}
	
	x = dispX;
	y = dispY;
	high = 7;

	if(bat_Value > BAT_SAMPLE_BASE_VALUE)
	{
		width = bat_Value - BAT_SAMPLE_BASE_VALUE;
	}
	else
	{
		width = 0;
	}
	width /= BAT_SAMPLE_STEP_VALUE;

	if(width > 25){
		width = 25;
	}
	if((bat_Status & BAT_CHARGE_STATE_MASK) == BAT_CHARGE_STATE_CHECK)	// charging
	{
		bat_show_begin = width / 5;
		bat_show_state++;
		if(bat_show_state > MAIN_UI_BAT_VALUE5)
		{
			bat_show_state = bat_show_begin;
		}
		//printf("bat show state:%d\r\n",bat_show_state);
		switch(bat_show_state)
		{
			case MAIN_UI_BAT_VALUE0:
				LCD_Fill(x, 		y, 	x+25, 	y+high, 	BLACK);	
				//LCD_Fill(x, y, x+width, y+high,GREEN);
				break;
			case MAIN_UI_BAT_VALUE1:
				LCD_Fill(x, 		y, 	x+5, 	y+high, 	GREEN);	
				LCD_Fill(x+5, 		y, 	x+25, 	y+high,	BLACK);				
				break;
			case MAIN_UI_BAT_VALUE2:
				LCD_Fill(x, 		y, 	x+10, 	y+high, 	GREEN);	
				LCD_Fill(x+10, 	y, 	x+25, 	y+high,	BLACK);				
				break;
			case MAIN_UI_BAT_VALUE3:
				LCD_Fill(x, 		y, 	x+15, 	y+high, 	GREEN);	
				LCD_Fill(x+15, 	y, 	x+25, 	y+high,	BLACK);				
				break;
			case MAIN_UI_BAT_VALUE4:
				LCD_Fill(x, 		y, 	x+20, 	y+high, 	GREEN);	
				LCD_Fill(x+20, 	y, 	x+25, 	y+high,	BLACK);				
				break;
			default:
			case MAIN_UI_BAT_VALUE5:
				LCD_Fill(x, 		y, 	x+25, 	y+high, 	GREEN);	
				//LCD_Fill(x, y, x+width, y+high,BLACK);				
				break;			
		}
	}
	else
	{
		if(bat_old_width != width){
			if(width == 0)
			{
				LCD_Fill(x, y, x+25, y+high, BLACK);
			}
			else if(width == 25)
			{
				LCD_Fill(x, y, x+25, y+high, GREEN);	
			}
			else
			{
				LCD_Fill(x, 		y, 	x+width, 		y+high,	GREEN);
				LCD_Fill(x+width, 	y, 	x+25, 		y+high, 	BLACK);
			}
			bat_old_width = width;	
		}
	}	
}


