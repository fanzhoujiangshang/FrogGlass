#ifndef __GPIO_CONTROL_H
#define __GPIO_CONTROL_H	 
#include "sys.h"
/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ��������:2018/6/1
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * All rights reserved
********************************************************************************/   	 
#define POWER_CTRL_PIN  	GPIO_PIN_1
#define MOTOR_PIN  		GPIO_PIN_2
#define BUZZER_PIN  		GPIO_PIN_8
#define BT_BRTS_PIN  	GPIO_PIN_12

#define POWER_CTRL(n)	(n?HAL_GPIO_WritePin(GPIOB,POWER_CTRL_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,POWER_CTRL_PIN,GPIO_PIN_RESET))
#define MOTOR(n)	(n?HAL_GPIO_WritePin(GPIOB,MOTOR_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,MOTOR_PIN,GPIO_PIN_RESET))
#define BUZZER(n)	(n?HAL_GPIO_WritePin(GPIOB,BUZZER_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,BUZZER_PIN,GPIO_PIN_RESET))
#define BT_BRTS(n)	(n?HAL_GPIO_WritePin(GPIOA,BT_BRTS_PIN,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,BT_BRTS_PIN,GPIO_PIN_RESET))

//IO��ʼ��
void GPIOInit(void);

void WritePowerControl(u8 state);
u8 ReadPowerControl(void);

#endif



