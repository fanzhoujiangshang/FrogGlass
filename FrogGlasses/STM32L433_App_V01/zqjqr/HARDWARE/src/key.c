
#include "key.h"
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
		
//����IO��ʼ������
void keyInit(void) 
{ 
    GPIO_InitTypeDef GPIO_Initure;
   
    __HAL_RCC_GPIOA_CLK_ENABLE();           //?a??GPIOA����?��
    __HAL_RCC_GPIOC_CLK_ENABLE();           //?a??GPIOC����?��
    __HAL_RCC_GPIOB_CLK_ENABLE();           //?a??GPIOB����?��

    GPIO_Initure.Pin=GPIO_PIN_0;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //��?��?
    GPIO_Initure.Pull=GPIO_PULLUP;        //??��-
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //???��
    HAL_GPIO_Init(GPIOA,&GPIO_Initure);

    GPIO_Initure.Pin=GPIO_PIN_9;            //PA0
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //��?��?
    GPIO_Initure.Pull=GPIO_PULLUP;        //??��-
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //???��
    HAL_GPIO_Init(GPIOB,&GPIO_Initure);
	
    GPIO_Initure.Pin=GPIO_PIN_13;           //PC13
    GPIO_Initure.Mode=GPIO_MODE_INPUT;      //��?��?
    GPIO_Initure.Pull=GPIO_PULLUP;          //��?��-
    GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //???��
    HAL_GPIO_Init(GPIOC,&GPIO_Initure);

}














