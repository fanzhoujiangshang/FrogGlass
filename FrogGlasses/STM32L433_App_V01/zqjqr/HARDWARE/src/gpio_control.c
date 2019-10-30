#include "gpio_control.h"
#include <stdio.h>
/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ��������:2018/6/1
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * All rights reserved
********************************************************************************/
static u8 SysPowerCtrl;
//IO��ʼ������
void GPIOInit(void) 
{ 
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOA_CLK_ENABLE();           //?a??GPIOA����?��
	__HAL_RCC_GPIOB_CLK_ENABLE();           //?a??GPIOB����?��

	GPIO_Initure.Pin=BT_BRTS_PIN;            //PA0
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//��a?��a��?��o?3?
	GPIO_Initure.Pull=GPIO_PULLUP;        //��|?����-
	GPIO_Initure.Speed=GPIO_SPEED_FAST;   //???����

	HAL_GPIO_Init(GPIOA,&GPIO_Initure);

	GPIO_Initure.Pin=POWER_CTRL_PIN|MOTOR_PIN|BUZZER_PIN;           
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_PULLUP;        
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 

	HAL_GPIO_Init(GPIOB,&GPIO_Initure);

/*
	__HAL_RCC_GPIOB_CLK_ENABLE(); //ʹ��ʱ��
	GPIO_Initure.Pin=BUZZER_PIN;           
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;
	GPIO_Initure.Pull=GPIO_PULLUP;        
	GPIO_Initure.Speed=GPIO_SPEED_HIGH; 

	HAL_GPIO_Init(GPIOB,&GPIO_Initure);
*/

	

	//SysPowerCtrl = 1;
	//POWER_CTRL(1);
	WritePowerControl(1);
	MOTOR(1);
	BUZZER(0);
	BT_BRTS(0);
	


}

void WritePowerControl(u8 state)
{
	if(state)
	{
		SysPowerCtrl = 1;
		POWER_CTRL(1);		
		printf("power on!\r\n");
	}
	else
	{
		SysPowerCtrl = 0;
		POWER_CTRL(0);
		printf("power off!\r\n");
	}
}
u8 ReadPowerControl(void)
{
	return SysPowerCtrl;
}











