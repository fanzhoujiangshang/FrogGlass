#ifndef __OLED_H
#define __OLED_H		
#include "sys.h"  
#include "usr_hardware.h"

//////////////////////////////////////////////////////////////////////////////////	 
//¡À?3¨¬D¨°??1??¡ì?¡ã¨º1¨®?¡ê??¡ä?-¡Á¡Â??D¨ª?¨¦¡ê?2?¦Ì?¨®?¨®¨²???¨¹¨¨?o?¨®?¨ª?
//ALIENTEK STM32F429?a¡¤¡é¡ã?
//OLED?y?¡¥¡ä¨²??	   
//?y¦Ì??-¡Á¨®@ALIENTEK
//??¨º???¨¬3:www.openedv.com
//¡ä¡ä?¡§¨¨??¨²:2016/1/6
//¡ã?¡À?¡êoV1.0
//¡ã?¨¨¡§?¨´¨®D¡ê?¦Ì¨¢¡ã?¡À????¡ê
//Copyright(C) 1??Y¨ºDD?¨°¨ª¦Ì?¡Á¨®????¨®D?T1??? 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
//OLED?¡ê¨º?¨¦¨¨??
//0: 4??¡ä?DD?¡ê¨º?  ¡ê¡§?¡ê?¨¦¦Ì?BS1¡ê?BS2?¨´?¨®GND¡ê?
//
		    						  
//-----------------OLED???¨²?¡§¨°?----------------  		

//#define OLED_RST_OFF    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_RESET);	
//#define OLED_RST_ON    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_5,GPIO_PIN_SET);	

//#define OLED_RS_OFF    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_RESET);	
//#define OLED_RS_ON    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_4,GPIO_PIN_SET);	

//#define OLED_SCLK_OFF    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_RESET);	
//#define OLED_SCLK_ON   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_12,GPIO_PIN_SET);	

//#define OLED_SDIN_OFF   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);	
//#define OLED_SDIN_ON   HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);	


#define OLED_RST_OFF		usr_oled_gpio_write(2, 0);
#define OLED_RST_ON			usr_oled_gpio_write(2, 1);

#define OLED_RS_OFF 		usr_oled_gpio_write(1, 0);
#define OLED_RS_ON    	usr_oled_gpio_write(1, 1);

#define OLED_SCLK_OFF 	usr_oled_gpio_write(4, 0);
#define OLED_SCLK_ON   	usr_oled_gpio_write(4, 1);

#define OLED_SDIN_OFF   usr_oled_gpio_write(3, 0);	
#define OLED_SDIN_ON   	usr_oled_gpio_write(3, 1);

#define OLED_CMD  	0		
#define OLED_DATA 	1		

//OLED¿ØÖÆÓÃº¯Êý
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
	 







 

