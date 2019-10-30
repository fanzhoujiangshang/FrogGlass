
#include "key.h"
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
		
//按键IO初始化函数
void keyInit(void) 
{ 
    GPIO_InitTypeDef GPIO_Initure;
   
    __HAL_RCC_GPIOA_CLK_ENABLE();           //?a??GPIOAê±?ó
    __HAL_RCC_GPIOC_CLK_ENABLE();           //?a??GPIOCê±?ó
    __HAL_RCC_GPIOB_CLK_ENABLE();           //?a??GPIOBê±?ó

    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //ê?è?
    GPIO_Initure.Pull=GPIO_PULLUP;        //??à-
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //???ù
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    GPIO_Initure.Pin=GPIO_PIN_9;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //ê?è?
    GPIO_Initure.Pull=GPIO_PULLUP;        //??à-
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //???ù
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    GPIO_Initure.Pin=GPIO_PIN_13;           //PC13
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //ê?è?
    GPIO_Initure.Pull=GPIO_PULLUP;          //é?à-
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //???ù
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);

}














