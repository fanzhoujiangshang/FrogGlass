#ifndef __TEXT_H__
#define __TEXT_H__	  
//#include "fontupd.h"

/*********************************************************************************
 *	������ʾ ��������
 *	��������:2018/10/27
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	All rights reserved
 *	******************************************************************************
 *	��ʼ�汾
 *	******************************************************************************/
 					     
//void Get_HzMat(unsigned char *code,unsigned char *mat,u8 size);		//�õ����ֵĵ�����
void Show_Font(u16 x,u16 y,const u8 *font,u8 size);						//��ָ��λ����ʾһ������
void Show_Str(u16 x,u16 y,u16 width,u16 height,const u8*str,u8 size);		//��ָ��λ����ʾһ���ַ��� 
void Show_Str_Mid(u16 x,u16 y,u8*str,u8 size,u8 len);				//��ָ����ȵ��м���ʾ�ַ�����
void showPicture(u8 x,u8 y,const u8 *p,u8 p_w,u8 p_h);
#endif
