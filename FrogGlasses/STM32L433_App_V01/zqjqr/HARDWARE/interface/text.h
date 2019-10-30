#ifndef __TEXT_H__
#define __TEXT_H__	  
//#include "fontupd.h"

/*********************************************************************************
 *	汉字显示 驱动代码
 *	创建日期:2018/10/27
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/
 					     
//void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);		//得到汉字的点阵码
void Show_Font(u16 x,u16 y,const u8 *font,u8 size);						//在指定位置显示一个汉字
void Show_Str(u16 x,u16 y,u16 width,u16 height,const u8*str,u8 size);		//在指定位置显示一个字符串 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);				//在指定宽度的中间显示字符串，
void showPicture(u8 x,u8 y,const u8 *p,u8 p_w,u8 p_h);
#endif
