#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ALIENTEK MiniFly_Remotor
 * ������������	
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/1
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/   	 

#if 0
#define READ_KEY_C()  	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)	//��ȡ�󰴼�
#define READ_KEY_B() 	HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)	//��ȡ�Ұ���
#define READ_KEY_A()  	HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)	//��ȡҡ��1����
#else
#define READ_KEY_A()  	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)		//��ȡ�󰴼�
#define READ_KEY_B() 	HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)		//��ȡ�Ұ���
#define READ_KEY_C()  	HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)	//��ȡҡ��1����
#endif

//IO��ʼ��
void keyInit(void);


#endif



