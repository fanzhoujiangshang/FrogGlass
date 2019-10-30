#ifndef _LCD_DRIVER_H_
#define	_LCD_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
//#include "stm32l4xx_hal.h"
#include "app\sys.h"

//LCD�Ŀ�͸߶���

extern const unsigned char gImage_logo[2000] ;

//const unsigned char asc2_1608[95][16];


#define	LCD_WIDTH					(180)
#define	LCD_HEIGHT					(120)
#define	LCD_BUFFER_LEN		(1152)	//(LCD_WIDTH * 2)
#define	LCD_DISP_MOVE				(1)
#define	LCD_DISP_MOVE_LEFT		(10)		// 27
#define	LCD_DISP_MOVE_TOP		(10)		// 18


#define WHITE         	 0xFFFF
#define BLACK         	 0x0000	

extern u16	POINT_COLOR;		//Ĭ�ϻ�����ɫ
extern u16	BACK_COLOR;		//Ĭ�ϱ�����ɫ





void LCD_Init(void);
void LCD_Clear(u16 color);
void LCD_Fill(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16 color);
void LCD_Show_Image(u16 x, u16 y, u16 width, u16 height, const u8 *p);					//��ʾͼƬ
void LCD_ShowString(u16 x,u16 y,u16 width,u16 height,u8 size,char *p);					//��ʾ�ַ���
void LCD_ShowChar(u16 x, u16 y, char chr, u8 size);										//��ʾһ���ַ�
void LCD_Write_HalfWord(const u16 da);													//д����ֽ����ݵ�LCD
void Show_Str(u16 x,u16 y,u16 width,u16 height,const u8*str,u8 size);		//��ָ��λ����ʾһ���ַ��� 
void Show_Font(u16 x,u16 y,const u8 *font,u8 size);						//��ָ��λ����ʾһ������

#ifdef __cplusplus
}
#endif

#endif

