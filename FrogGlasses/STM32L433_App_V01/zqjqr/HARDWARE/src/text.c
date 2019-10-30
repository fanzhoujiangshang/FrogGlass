#include "sys.h"
#include "font.h"
#include "lcd.h"
#include "text.h"
#include "string.h"

/*********************************************************************************
 *	汉字显示 驱动代码
 *	创建日期:2018/10/27
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/

 const char* font1616_table = {
"置温度深潜水时间减压\
东南西北水肺自由潜仪表模式设计记录\
配对语言恢复出厂是否备未匹请通过码界面选择高氧泳中文\
最大确定长当前状态结束取消空气升级中版本"
};
#if 0 
 const char* font2424_table = {
"置温度深潜水时间减压\
东南西北水肺自由潜仪表模式设计记录\
配对语言恢复出厂是否备未匹请通过码界面选择高氧泳中文\
最大确定长"
};
#endif

/**
 * @brief	显示一个指定大小的汉字
 *
 * @param   x 		显示横坐标
 * @param   y		显示纵坐标
 * @param   font	汉字GBK码
 * @param   size	字体大小
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
	uint8_t csize=(size/8+((size%8)?1:0))*size;//得到自由分辨字符所占的字节数 

	if(size==16)
		font_table = font1616_table;
//	else if(size==24)
//		font_table = font2424_table;
	else return;/*没有的字库*/
	for(fontSeq=0; fontSeq<strlen(font_table)/2; fontSeq++)/*计算font_table对应字库的数组下标*/
	{
		if(font_table[2*fontSeq]==font[0] && font_table[2*fontSeq+1]==font[1])
			break;
	}
	if(fontSeq >= strlen(font_table)/2) return;/*font_table中没有font该字*/

	LCD_Address_Set(x, y, x + size - 1, y + size - 1);
//	printf("csize=%d ,  fontSeq =%d, size =%d\r\n",csize,fontSeq,size);
	for(t=0;t<csize;t++)
	{   												   
		if(size==16)
			temp = font_1616[fontSeq][t];/*调用font_1212字库*/
//		else if(size==24)
//			temp = font_2424[fontSeq][t];/*调用font_2424字库*/
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
 * @brief	在指定位置开始显示一个字符串
 *
 * @param   x 		显示横坐标
 * @param   y		显示纵坐标
 * @param   width	显示区域宽度
 * @param   height	显示区域高度
 * @param   str  	字符串
 * @param   size 	字体大小
 *
 * @return 	void
 */
void Show_Str(u16 x, u16 y, u16 width, u16 height, const u8*str, u8 size)
{
    u16 x0 = x;
    u16 y0 = y;
    u8 bHz = 0;   //字符或者中文

    while(*str != 0) //数据未结束
    {
        if(!bHz)
        {
            if(*str > 0x80)bHz = 1; //中文

            else              //字符
            {
                if(x > (x0 + width - size / 2)) //换行
                {
                    y += size;
                    x = x0;
                }

                if(y > (y0 + height - size))break; //越界返回

                if(*str == 13) //换行符号
                {
                    y += size;
                    x = x0;
                    str++;
                }

                else LCD_ShowChar(x, y, *str, size); //有效部分写入

                str++;
                x += size / 2; //字符,为全字的一半
            }
        }

        else //中文
        {
            bHz = 0; //有汉字库

            if(x > (x0 + width - size)) //换行
            {
                y += size;
                x = x0;
            }

            if(y > (y0 + height - size))break; //越界返回

            Show_Font(x, y, str, size); //显示这个汉字,空心显示
            str += 2;
            x += size; //下一个汉字偏移
        }
    }
}

/**
 * @brief	在指定宽度的中间显示字符串，如果字符长度超过了len,则用Show_Str显示
 *
 * @param   x 		显示横坐标
 * @param   y		显示纵坐标
 * @param   len		指定要显示的宽度
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
	u8 csize=(p_h/8+((p_h%8)?1:0))*p_w;//得到自由分辨字符所占的字节数

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





























