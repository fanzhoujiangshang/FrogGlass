#ifndef __OLED_H
#define __OLED_H		
#include "sys.h"  	 
//////////////////////////////////////////////////////////////////////////////////	 
//��?3��D��??1??��?�㨺1��?��??��?-����??D��?����?2?��?��?����???����?o?��?��?
//ALIENTEK STM32F429?a�����?
//OLED?y?���䨲??	   
//?y��??-����@ALIENTEK
//??��???��3:www.openedv.com
//���?����??��:2016/1/6
//��?��?��oV1.0
//��?����?����D��?�̨���?��????��
//Copyright(C) 1??Y��DD?������?����????��D?T1??? 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//OLED?�꨺?����??
//0: 4??��?DD?�꨺?  �ꡧ?��?����?BS1��?BS2?��?��GND��?
//
		    						  
//-----------------OLED???��?����?----------------  		

#define OLED_RST_OFF    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_RESET);	
#define OLED_RST_ON    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_8,GPIO_PIN_SET);	

#define OLED_RS_OFF    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_RESET);	
#define OLED_RS_ON    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_6,GPIO_PIN_SET);	

#define OLED_SCLK_OFF    HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_RESET);	
#define OLED_SCLK_ON   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_5,GPIO_PIN_SET);	

#define OLED_SDIN_OFF   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_RESET);	
#define OLED_SDIN_ON   HAL_GPIO_WritePin(GPIOA,GPIO_PIN_7,GPIO_PIN_SET);	

#define OLED_CMD  	0		
#define OLED_DATA 	1		

//OLED�����ú���
void oled_wr_byte(u8 dat,u8 cmd);	    
void oled_displayON(void);
void oled_displayOFF(void);
void oled_refreshGram(void);  		    
void oledInit(void);
void oled_clear(void);
void oled_drawPoint(u8 x,u8 y,u8 t);
u8 oled_readPoint(u8 x,u8 y);
void oled_fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot);
void oled_showChar(u8 x,u8 y,u8 chr,u8 f_w,u8 f_h,u8 mode);
void oled_showNum(u8 x,u8 y,u32 num,u8 len,u8 f_w,u8 f_h);
void oled_showString(u8 x,u8 y,const u8 *p,u8 f_w,u8 f_h);	
void oled_showPicture(u8 x,u8 y,const u8 *p,u8 p_w,u8 p_h);
#endif  
	 







 

