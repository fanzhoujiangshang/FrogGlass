#ifndef __KEY_H
#define __KEY_H	 
#include "sys.h"
/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * ALIENTEK MiniFly_Remotor
 * 按键驱动代码	
 * 正点原子@ALIENTEK
 * 技术论坛:www.openedv.com
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * Copyright(C) 广州市星翼电子科技有限公司 2014-2024
 * All rights reserved
********************************************************************************/   	 

#if 0
#define READ_KEY_C()  	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)	//读取左按键
#define READ_KEY_B() 	HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)	//读取右按键
#define READ_KEY_A()  	HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)	//读取摇杆1按键
#else
#define READ_KEY_A()  	HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_9)		//读取左按键
#define READ_KEY_B() 	HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)		//读取右按键
#define READ_KEY_C()  	HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13)	//读取摇杆1按键
#endif

//IO初始化
void keyInit(void);


#endif



