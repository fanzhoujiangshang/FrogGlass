#ifndef __BAT_ADC_H
#define __BAT_ADC_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//ADC��������	   
//��������:2016/1/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

void BAT_ADC_Init(void); 				//ADCͨ����ʼ��
u16 Get_Adc_AverageNew(u32 ch);

void BAT_I2C_Init(void);
void BAT_ADC_Deinit(void);
#endif 
