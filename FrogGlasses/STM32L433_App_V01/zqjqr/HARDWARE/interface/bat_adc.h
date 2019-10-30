#ifndef __BAT_ADC_H
#define __BAT_ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ADC驱动代码	   
//创建日期:2016/1/13
//版本：V1.0
//版权所有，盗版必究。
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

void BAT_ADC_Init(void); 				//ADC通道初始化
u16 Get_Adc_AverageNew(u32 ch);

void BAT_I2C_Init(void);
void BAT_ADC_Deinit(void);
#endif 
