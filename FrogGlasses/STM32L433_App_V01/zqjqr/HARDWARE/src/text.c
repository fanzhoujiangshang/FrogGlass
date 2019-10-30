#include "sys.h"
#include "font.h"
#include "lcd.h"
#include "text.h"
#include "string.h"

/*********************************************************************************
 *	������ʾ ��������
 *	��������:2018/10/27
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	All rights reserved
 *	******************************************************************************
 *	��ʼ�汾
 *	******************************************************************************/

 const char* font1616_table = {
"���¶���Ǳˮʱ���ѹ\
��������ˮ������Ǳ�Ǳ�ģʽ��Ƽ�¼\
������Իָ������Ƿ�δƥ��ͨ�������ѡ�����Ӿ����\
���ȷ������ǰ״̬����ȡ�����������а汾"
};
#if 0 
 const char* font2424_table = {
"���¶���Ǳˮʱ���ѹ\
��������ˮ������Ǳ�Ǳ�ģʽ��Ƽ�¼\
������Իָ������Ƿ�δƥ��ͨ�������ѡ�����Ӿ����\
���ȷ����"
};
#endif

/**
 * @brief	��ʾһ��ָ����С�ĺ���
 *
 * @param   x 		��ʾ������
 * @param   y		��ʾ������
 * @param   font	����GBK��
 * @param   size	�����С
 *
 * @return 	void
 */
void Show_Font(u16 x, u16 y, const u8 *font, u8 size)
{
       u16 colortemp;
	const char* font_table;
	uint16_t fontSeq;
	uint8_t temp,t,t1;
//	uint16_t y0=y; 
	uint8_t csize=(size/8+((size%8)?1:0))*size;//�õ����ɷֱ��ַ���ռ���ֽ��� 

	if(size==16)
		font_table = font1616_table;
//	else if(size==24)
//		font_table = font2424_table;
	else return;/*û�е��ֿ�*/
	for(fontSeq=0; fontSeq<strlen(font_table)/2; fontSeq++)/*����font_table��Ӧ�ֿ�������±�*/
	{
		if(font_table[2*fontSeq]==font[0] && font_table[2*fontSeq+1]==font[1])
			break;
	}
	if(fontSeq >= strlen(font_table)/2) return;/*font_table��û��font����*/

	LCD_Address_Set(x, y, x + size - 1, y + size - 1);
//	printf("csize=%d ,  fontSeq =%d, size =%d\r\n",csize,fontSeq,size);
	for(t=0;t<csize;t++)
	{   												   
		if(size==16)
			temp = font_1616[fontSeq][t];/*����font_1212�ֿ�*/
//		else if(size==24)
//			temp = font_2424[fontSeq][t];/*����font_2424�ֿ�*/
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80) colortemp = POINT_COLOR;
			else colortemp = BACK_COLOR;
			
                     LCD_Write_HalfWord(colortemp);		
			temp<<=1;		
		}  	 
	}  

}
/**
 * @brief	��ָ��λ�ÿ�ʼ��ʾһ���ַ���
 *
 * @param   x 		��ʾ������
 * @param   y		��ʾ������
 * @param   width	��ʾ������
 * @param   height	��ʾ����߶�
 * @param   str  	�ַ���
 * @param   size 	�����С
 *
 * @return 	void
 */
void Show_Str(u16 x, u16 y, u16 width, u16 height, const u8*str, u8 size)
{
    u16 x0 = x;
    u16 y0 = y;
    u8 bHz = 0;   //�ַ���������

    while(*str != 0) //����δ����
    {
        if(!bHz)
        {
            if(*str > 0x80)bHz = 1; //����

            else              //�ַ�
            {
                if(x > (x0 + width - size / 2)) //����
                {
                    y += size;
                    x = x0;
                }

                if(y > (y0 + height - size))break; //Խ�緵��

                if(*str == 13) //���з���
                {
                    y += size;
                    x = x0;
                    str++;
                }

                else LCD_ShowChar(x, y, *str, size); //��Ч����д��

                str++;
                x += size / 2; //�ַ�,Ϊȫ�ֵ�һ��
            }
        }

        else //����
        {
            bHz = 0; //�к��ֿ�

            if(x > (x0 + width - size)) //����
            {
                y += size;
                x = x0;
            }

            if(y > (y0 + height - size))break; //Խ�緵��

            Show_Font(x, y, str, size); //��ʾ�������,������ʾ
            str += 2;
            x += size; //��һ������ƫ��
        }
    }
}

/**
 * @brief	��ָ����ȵ��м���ʾ�ַ���������ַ����ȳ�����len,����Show_Str��ʾ
 *
 * @param   x 		��ʾ������
 * @param   y		��ʾ������
 * @param   len		ָ��Ҫ��ʾ�Ŀ��
 *
 * @return 	void
 */
void Show_Str_Mid(u16 x, u16 y, u8*str, u8 size, u8 len)
{
    u16 strlenth = 0;
    strlenth = strlen((const char*)str);
    strlenth *= size / 2;

    if(strlenth > len)Show_Str(x, y, LCD_Width, LCD_Height, str, size);

    else
    {
        strlenth = (len - strlenth) / 2;
        Show_Str(strlenth + x, y, LCD_Width, LCD_Height, str, size);
    }
}


void showPicture(u8 x,u8 y,const u8 *p,u8 p_w,u8 p_h)
{	
       u16 colortemp;
	u8 temp,t,t1;
	u8 csize=(p_h/8+((p_h%8)?1:0))*p_w;//�õ����ɷֱ��ַ���ռ���ֽ���

	LCD_Address_Set(x, y, x + p_w - 1, y + p_h - 1);
	
	for(t=0;t<csize;t++)
	{   
		temp=*p++;
	
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80) colortemp = POINT_COLOR;
			else colortemp = BACK_COLOR;
			
                     LCD_Write_HalfWord(colortemp);		
			temp<<=1;	
		}  	 
	}   
}





























