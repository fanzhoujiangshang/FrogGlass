/**
  ******************************************************************************
  * File Name          : gpio.c
  * Description        : This file provides code for the configuration
  *                      of all used GPIO pins.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "gpio.h"
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */

/*----------------------------------------------------------------------------*/
/* Configure GPIO                                                             */
/*----------------------------------------------------------------------------*/
/* USER CODE BEGIN 1 */

/* USER CODE END 1 */

/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void MX_GPIO_Init(void)
{

  /* GPIO Ports Clock Enable */
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOH);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
  LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);

}

/* USER CODE BEGIN 2 */
void User_GPIO_Init_Selfdefine(void)
{
	LL_GPIO_InitTypeDef  GPIO_Initure;

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	//GPIO初始化设置      
	/*
		pin 19		GPIOB01		POWER CTRL
		pin 20		GPIOB02		Motor
		pin 45		GPIOB08		buzzer
	*/
	GPIO_Initure.Pin			=LL_GPIO_PIN_1|LL_GPIO_PIN_2|LL_GPIO_PIN_8;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_OUTPUT;//推挽输出
	GPIO_Initure.Pull			=LL_GPIO_PULL_NO;        //上拉
	GPIO_Initure.Speed		=LL_GPIO_SPEED_FREQ_HIGH;   //高速
	GPIO_Initure.OutputType 	= LL_GPIO_OUTPUT_PUSHPULL;
	 LL_GPIO_Init(GPIOB,&GPIO_Initure);//初始化
	/*
		pin 33		GPIOA12		BT BRTS
	*/	 
	GPIO_Initure.Pin			=LL_GPIO_PIN_12;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_OUTPUT;//推挽输出
	GPIO_Initure.Pull			=LL_GPIO_PULL_NO;        //上拉
	GPIO_Initure.Speed		=LL_GPIO_SPEED_FREQ_HIGH;   //高速
	GPIO_Initure.OutputType 	= LL_GPIO_OUTPUT_PUSHPULL;	 
	 LL_GPIO_Init(GPIOA,&GPIO_Initure);//初始化
	 POWER_CTRL(1);
	 BT_BRTS(1);
	/*
		pin 10		GPIOA00		Key2
		pin 46		GPIOB09		Key3
		pin 2		GPIOC13		Key1
	*/
	GPIO_Initure.Pin			=LL_GPIO_PIN_0;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_INPUT;//推挽输出
	GPIO_Initure.Pull			=LL_GPIO_PULL_UP;        //上拉
	GPIO_Initure.Speed		=LL_GPIO_SPEED_FREQ_HIGH;   //高速
	 LL_GPIO_Init(GPIOA,&GPIO_Initure);//初始化
	GPIO_Initure.Pin			=LL_GPIO_PIN_9;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_INPUT;//推挽输出
	GPIO_Initure.Pull			=LL_GPIO_PULL_UP;        //上拉
	GPIO_Initure.Speed		=LL_GPIO_SPEED_FREQ_HIGH;   //高速
	 LL_GPIO_Init(GPIOB,&GPIO_Initure);//初始化
	GPIO_Initure.Pin			=LL_GPIO_PIN_13;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_INPUT;//推挽输出
	GPIO_Initure.Pull			=LL_GPIO_PULL_UP;        //上拉
	GPIO_Initure.Speed		=LL_GPIO_SPEED_FREQ_HIGH;   //高速
	 LL_GPIO_Init(GPIOC,&GPIO_Initure);//初始化	 

	/*
		pin 14		GPIOA04		SPI1_NSS
		pin 29		GPIOA08		RESET
		pin 18		GPIOB00		D/CX
	*/   
	LL_GPIO_SetOutputPin(GPIOA,LL_GPIO_PIN_4|LL_GPIO_PIN_8);
	GPIO_Initure.Pin			=LL_GPIO_PIN_4|LL_GPIO_PIN_8;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_OUTPUT;//推挽输出
	GPIO_Initure.Pull			=LL_GPIO_PULL_NO;        //上拉
	GPIO_Initure.Speed		=LL_GPIO_SPEED_FREQ_HIGH;   //高速
	GPIO_Initure.OutputType 	= LL_GPIO_OUTPUT_PUSHPULL;
	 LL_GPIO_Init(GPIOA,&GPIO_Initure);//初始化
	 
	GPIO_Initure.Pin			=LL_GPIO_PIN_0;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_OUTPUT;//推挽输出
	GPIO_Initure.Pull			=LL_GPIO_PULL_UP;        //上拉
	GPIO_Initure.Speed		=LL_GPIO_SPEED_FREQ_HIGH;   //高速
	GPIO_Initure.OutputType 	= LL_GPIO_OUTPUT_PUSHPULL;
	 LL_GPIO_Init(GPIOB,&GPIO_Initure);//初始化	 
}
void User_GPIO_ReadyForStandby(void)
{
	LL_GPIO_InitTypeDef  GPIO_Initure;

	LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
	//GPIO
	GPIO_Initure.Pin			=LL_GPIO_PIN_ALL;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_INPUT;
	GPIO_Initure.Pull			=LL_GPIO_PULL_NO;  
	 LL_GPIO_Init(GPIOA,&GPIO_Initure);
	GPIO_Initure.Pin			=LL_GPIO_PIN_ALL &(~LL_GPIO_PIN_1);	
	GPIO_Initure.Mode			=LL_GPIO_MODE_INPUT;
	GPIO_Initure.Pull			=LL_GPIO_PULL_NO;  
	 LL_GPIO_Init(GPIOB,&GPIO_Initure);
	GPIO_Initure.Pin			=LL_GPIO_PIN_1 | LL_GPIO_PIN_10 | LL_GPIO_PIN_11;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_INPUT;
	GPIO_Initure.Pull			=LL_GPIO_PULL_DOWN;  
	 LL_GPIO_Init(GPIOB,&GPIO_Initure);
	GPIO_Initure.Pin			=LL_GPIO_PIN_ALL;	
	GPIO_Initure.Mode			=LL_GPIO_MODE_INPUT;
	GPIO_Initure.Pull			=LL_GPIO_PULL_NO;  
	 LL_GPIO_Init(GPIOC,&GPIO_Initure); 
//	 POWER_CTRL(0);	 
}
/* USER CODE END 2 */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
