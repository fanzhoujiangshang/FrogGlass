#include "sys.h"
#include "lcd.h"
#include "font.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
//#include "alientek_log.h"

/*********************************************************************************
 *	LCD TFT驱动代码
 *	创建日期:2018/10/27
 *	版本：V1.0
 *	版权所有，盗版必究。
 *	All rights reserved
 *	******************************************************************************
 *	初始版本
 *	******************************************************************************/

//LCD缓存大小设置，修改此值时请注意！！！！修改这两个值时可能会影响以下函数	LCD_Clear/LCD_Fill/LCD_DrawLine
#define LCD_TOTAL_BUF_SIZE	(180*120*2)
#define LCD_Buf_Size 1152
static u8 lcd_buf[LCD_Buf_Size];

u16	POINT_COLOR = BLACK;	//画笔颜色	默认为黑色
u16	BACK_COLOR 	= WHITE;	//背景颜色	默认为白色
u8 flush_enable = 0;

SPI_HandleTypeDef SPI_Lcd_Handler;  //SPI句柄

void lcd_spi_init(void)
{
    SPI_Lcd_Handler.Instance=SPI1;                         //SP1
    SPI_Lcd_Handler.Init.Mode=SPI_MODE_MASTER;             //设置SPI工作模式，设置为主模式
    SPI_Lcd_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //设置SPI单向或者双向的数据模式:SPI设置为双线模式
    SPI_Lcd_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //设置SPI的数据大小:SPI发送接收8位帧结构
    SPI_Lcd_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //串行同步时钟的空闲状态为高电平
    SPI_Lcd_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //串行同步时钟的第二个跳变沿（上升或下降）数据被采样
    SPI_Lcd_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
    SPI_Lcd_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;//定义波特率预分频的值:波特率预分频值为256
    SPI_Lcd_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
    SPI_Lcd_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //关闭TI模式
    SPI_Lcd_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//关闭硬件CRC校验
    SPI_Lcd_Handler.Init.CRCPolynomial=7;                  //CRC值计算的多项式
    SPI_Lcd_Handler.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    SPI_Lcd_Handler.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	
    HAL_SPI_Init(&SPI_Lcd_Handler);//初始化
    
    __HAL_SPI_ENABLE(&SPI_Lcd_Handler);                    //使能SPI5
			
}

/**
 * @brief	LCD底层SPI发送数据函数
 *
 * @param   data	数据的起始地址
 * @param   size	发送数据大小
 *
 * @return  void
 */
static void LCD_SPI_Send(u8 *data, u16 size)
{
//	u16 i;
//       LCD_CS(0);	
	HAL_SPI_Transmit(&SPI_Lcd_Handler, (uint8_t *)data, size, 10);
//       LCD_CS(1);	
}


/**
 * @brief	写命令到LCD
 *
 * @param   cmd		需要发送的命令
 *
 * @return  void
 */
static void LCD_Write_Cmd(u8 cmd)
{
//   LCD_CS(0);
   LCD_DC(0);
   LCD_SPI_Send(&cmd, 1);
//   LCD_CS(1);	   
  }

/**
 * @brief	写数据到LCD
 *
 * @param   data		需要发送的数据
 *
 * @return  void
 */
static void LCD_Write_Data(u8 data)
{
//    LCD_CS(0);
    LCD_DC(1);
    LCD_SPI_Send(&data, 1);
//    LCD_CS(1);		
}

/**
 * @brief	写半个字的数据到LCD
 *
 * @param   da		需要发送的数据
 *
 * @return  void
 */
void LCD_Write_HalfWord(const u16 da)
{
    u8 data[2] = {0};

    data[0] = da >> 8;
    data[1] = da;
 //   LCD_CS(0);
    LCD_DC(1);
    LCD_SPI_Send(data, 2);
//    LCD_CS(1);	
}


/**
 * 设置数据写入LCD缓存区域
 *
 * @param   x1,y1	起点坐标
 * @param   x2,y2	终点坐标
 *
 * @return  void
 */
void LCD_Address_Set(u16 x1, u16 y1, u16 x2, u16 y2)
{
#if LCD_DISP_MOVE
	u16 x1_new,x2_new,y1_new,y2_new;

	x1_new = x1;
	x2_new = x2;
	y1_new = y1;
	y2_new = y2;
	if(x1_new > LCD_DISP_MOVE_LEFT)
	{
		x1_new -=  LCD_DISP_MOVE_LEFT;
		x2_new -= LCD_DISP_MOVE_LEFT;
	}
	if(y1_new > LCD_DISP_MOVE_TOP)
	{
		y1_new -= LCD_DISP_MOVE_TOP;
		y2_new -= LCD_DISP_MOVE_TOP;
	}	
	
	LCD_Write_Cmd(0x2a);
	LCD_Write_Data(x1_new>> 8);
	LCD_Write_Data(x1_new);
	LCD_Write_Data(x2_new>> 8);
	LCD_Write_Data(x2_new);

	LCD_Write_Cmd(0x2b);
	LCD_Write_Data(y1_new>> 8);
	LCD_Write_Data(y1_new);
	LCD_Write_Data(y2_new>> 8);
	LCD_Write_Data(y2_new);

	LCD_Write_Cmd(0x2C);
#else
	LCD_Write_Cmd(0x2a);
	LCD_Write_Data(x1 >> 8);
	LCD_Write_Data(x1);
	LCD_Write_Data(x2 >> 8);
	LCD_Write_Data(x2);

	LCD_Write_Cmd(0x2b);
	LCD_Write_Data(y1 >> 8);
	LCD_Write_Data(y1);
	LCD_Write_Data(y2 >> 8);
	LCD_Write_Data(y2);

	LCD_Write_Cmd(0x2C);
#endif	
}

/**
 * 打开LCD显示
 *
 * @param   void
 *
 * @return  void
 */
void LCD_DisplayOn(void)
{
  //  LCD_PWR(1);
}
/**
 * 关闭LCD显示
 *
 * @param   void
 *
 * @return  void
 */
void LCD_DisplayOff(void)
{
//    LCD_PWR(0);
}

/**
 * 以一种颜色清空LCD屏
 *
 * @param   color	清屏颜色
 *
 * @return  void
 */
void LCD_Clear(u16 color)
{
    u16 i = 0;
    u32 size = 0, size_remain = 0;

    size = (180 + 1) * (120 + 1) * 2;

    if(size > LCD_Buf_Size)
    {
        size_remain = size - LCD_Buf_Size;
        size = LCD_Buf_Size;
    }

    LCD_Address_Set(0, 0, LCD_Width, LCD_Height);

    while(1)
    {
        for(i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        LCD_DC(1);
        LCD_SPI_Send(lcd_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > LCD_Buf_Size)
        {
            size_remain = size_remain - LCD_Buf_Size;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }

}

/**
 * 用一个颜色填充整个区域
 *
 * @param   x_start,y_start     起点坐标
 * @param   x_end,y_end			终点坐标
 * @param   color       		填充颜色
 *
 * @return  void
 */
void LCD_Fill(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16 color)
{
    u16 i = 0;
    u32 size = 0, size_remain = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if(size > LCD_Buf_Size)
    {
        size_remain = size - LCD_Buf_Size;
        size = LCD_Buf_Size;
    }

    LCD_Address_Set(x_start, y_start, x_end, y_end);

    while(1)
    {
        for(i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = color >> 8;
            lcd_buf[2 * i + 1] = color;
        }

        LCD_DC(1);
        LCD_SPI_Send(lcd_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > LCD_Buf_Size)
        {
            size_remain = size_remain - LCD_Buf_Size;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}

/**
 * ó?ò?????é?BUFì?3???????óò
 *
 * @param   x_start,y_start     ?eμ?×?±ê
 * @param   x_end,y_end			??μ?×?±ê
 * @param   color       		ì?3???é?
 *
 * @return  void
 */
void LCD_Fill_Buf(u16 x_start, u16 y_start, u16 x_end, u16 y_end, u16* clr_buf)
{
    u16 i = 0;
    u32 size = 0, size_remain = 0;

    size = (x_end - x_start + 1) * (y_end - y_start + 1) * 2;

    if(size > LCD_Buf_Size)
    {
        size_remain = size - LCD_Buf_Size;
        size = LCD_Buf_Size;
    }

    LCD_Address_Set(x_start, y_start, x_end, y_end);

    while(1)
    {
        for(i = 0; i < size / 2; i++)
        {
            lcd_buf[2 * i] = clr_buf[i] >> 8;
            lcd_buf[2 * i + 1] = clr_buf[i];
        }

        LCD_DC(1);
        LCD_SPI_Send(lcd_buf, size);

        if(size_remain == 0)
            break;

        if(size_remain > LCD_Buf_Size)
        {
            size_remain = size_remain - LCD_Buf_Size;
        }

        else
        {
            size = size_remain;
            size_remain = 0;
        }
    }
}



/**
 * 画点函数
 *
 * @param   x,y		画点坐标
 *
 * @return  void
 */
void LCD_Draw_Point(u16 x, u16 y)
{
    LCD_Address_Set(x, y, x, y);
    LCD_Write_HalfWord(POINT_COLOR);
}

/**
 * 画点带颜色函数
 *
 * @param   x,y		画点坐标
 *
 * @return  void
 */
void LCD_Draw_ColorPoint(u16 x, u16 y,u16 color)
{
    LCD_Address_Set(x, y, x, y);
    LCD_Write_HalfWord(color);
}



/**
 * @brief	画线函数(直线、斜线)
 *
 * @param   x1,y1	起点坐标
 * @param   x2,y2	终点坐标
 *
 * @return  void
 */
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2)
{
    u16 t;
    int xerr = 0, yerr = 0, delta_x, delta_y, distance;
    int incx, incy, row, col;
    u32 i = 0;

    if(y1 == y2)
    {
        /*快速画水平线*/
        LCD_Address_Set(x1, y1, x2, y2);

        for(i = 0; i < x2 - x1; i++)
        {
            lcd_buf[2 * i] = POINT_COLOR >> 8;
            lcd_buf[2 * i + 1] = POINT_COLOR;
        }

        LCD_DC(1);
        LCD_SPI_Send(lcd_buf, (x2 - x1) * 2);
        return;
    }

    delta_x = x2 - x1;
    delta_y = y2 - y1;
    row = x1;
    col = y1;

    if(delta_x > 0)incx = 1;

    else if(delta_x == 0)incx = 0;

    else
    {
        incx = -1;
        delta_x = -delta_x;
    }

    if(delta_y > 0)incy = 1;

    else if(delta_y == 0)incy = 0;

    else
    {
        incy = -1;
        delta_y = -delta_y;
    }

    if(delta_x > delta_y)distance = delta_x;

    else distance = delta_y;

    for(t = 0; t <= distance + 1; t++)
    {
        LCD_Draw_Point(row, col);
        xerr += delta_x ;
        yerr += delta_y ;

        if(xerr > distance)
        {
            xerr -= distance;
            row += incx;
        }

        if(yerr > distance)
        {
            yerr -= distance;
            col += incy;
        }
    }
}

/**
 * @brief	画一个矩形
 *
 * @param   x1,y1	起点坐标
 * @param   x2,y2	终点坐标
 *
 * @return  void
 */
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2)
{
    LCD_DrawLine(x1, y1, x2, y1);
    LCD_DrawLine(x1, y1, x1, y2);
    LCD_DrawLine(x1, y2, x2, y2);
    LCD_DrawLine(x2, y1, x2, y2);
}

/**
 * @brief	画一个圆
 *
 * @param   x0,y0	圆心坐标
 * @param   r       圆半径
 *
 * @return  void
 */
void LCD_Draw_Circle(u16 x0, u16 y0, u8 r)
{
    int a, b;
    int di;
    a = 0;
    b = r;
    di = 3 - (r << 1);

    while(a <= b)
    {
        LCD_Draw_Point(x0 - b, y0 - a);
        LCD_Draw_Point(x0 + b, y0 - a);
        LCD_Draw_Point(x0 - a, y0 + b);
        LCD_Draw_Point(x0 - b, y0 - a);
        LCD_Draw_Point(x0 - a, y0 - b);
        LCD_Draw_Point(x0 + b, y0 + a);
        LCD_Draw_Point(x0 + a, y0 - b);
        LCD_Draw_Point(x0 + a, y0 + b);
        LCD_Draw_Point(x0 - b, y0 + a);
        a++;

        //Bresenham
        if(di < 0)di += 4 * a + 6;

        else
        {
            di += 10 + 4 * (a - b);
            b--;
        }

        LCD_Draw_Point(x0 + a, y0 + b);
    }
}

/**
 * @brief	显示一个ASCII码字符
 *
 * @param   x,y		显示起始坐标
 * @param   chr		需要显示的字符
 * @param   size	字体大小(支持16/24/32号字体)
 *
 * @return  void
 */
void LCD_ShowChar(u16 x, u16 y, char chr, u8 size)
{
    u8 temp, t1, t;
    u8 csize;		//得到字体一个字符对应点阵集所占的字节数
    u16 colortemp;
    u8 sta;

    chr = chr - ' '; //得到偏移后的值（ASCII字库是从空格开始取模，所以-' '就是对应字符的字库）

    if((x > (LCD_Width - size / 2)) || (y > (LCD_Height - size)))	return;

    LCD_Address_Set(x, y, x + size / 2 - 1, y + size - 1);//(x,y,x+8-1,y+16-1)

    if((size == 16) || (size == 32) )	//16和32号字体
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for(t = 0; t < csize; t++)
        {
            if(size == 16)temp = asc2_1608[chr][t];	//调用1608字体

           else if(size == 32)temp = asc2_3216[chr][t];	//调用3216字体

            else return;			//没有的字库

            for(t1 = 0; t1 < 8; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;

                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }

	else if  (size == 12)	//12号字体
	{
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for(t = 0; t < csize; t++)
        {
            temp = asc2_1206[chr][t];

            for(t1 = 0; t1 < 6; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;

                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
	
    else if(size == 24)		//24号字体
    {
        csize = (size * 16) / 8;

        for(t = 0; t < csize; t++)
        {
            temp = asc2_2412[chr][t];

            if(t % 2 == 0)sta = 8;

            else sta = 4;

            for(t1 = 0; t1 < sta; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;

                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }
}

/**
 * @brief	m^n函数
 *
 * @param   m,n		输入参数
 *
 * @return  m^n次方
 */
static u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;

    while(n--)result *= m;

    return result;
}

/**
 * @brief	显示数字,高位为0不显示
 *
 * @param   x,y		起点坐标
 * @param   num		需要显示的数字,数字范围(0~4294967295)
 * @param   len		需要显示的位数
 * @param   size	字体大小
 *
 * @return  void
 */
void LCD_ShowNum(u16 x, u16 y, u32 num, u8 len, u8 size)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                LCD_ShowChar(x + (size / 2)*t, y, ' ', size);
                continue;
            }

            else enshow = 1;
        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size);
    }
}



/**
 * @brief	显示数字,高位为0,可以控制显示为0还是不显示
 *
 * @param   x,y		起点坐标
 * @param   num		需要显示的数字,数字范围(0~999999999)
 * @param   len		需要显示的位数
 * @param   size	字体大小
 * @param   mode	1:高位显示0		0:高位不显示
 *
 * @return  void
 */
void LCD_ShowxNum(u16 x, u16 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++)
    {
        temp = (num / LCD_Pow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                if(mode)LCD_ShowChar(x + (size / 2)*t, y, '0', size);

                else
                    LCD_ShowChar(x + (size / 2)*t, y, ' ', size);

                continue;
            }

            else enshow = 1;
        }

        LCD_ShowChar(x + (size / 2)*t, y, temp + '0', size);
    }
}


/**
 * @brief	显示字符串
 *
 * @param   x,y		起点坐标
 * @param   width	字符显示区域宽度
 * @param   height	字符显示区域高度
 * @param   size	字体大小
 * @param   p		字符串起始地址
 *
 * @return  void
 */
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, char *p)
{
    u8 x0 = x;
    width += x;
    height += y;

    while((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if(x >= width)
        {
            x = x0;
            y += size;
        }

        if(y >= height)break; //退出

        LCD_ShowChar(x, y, *p, size);
        x += size / 2;
        p++;
    }
}


/**
 * @brief	显示图片
 *
 * @remark	Image2Lcd取模方式：	C语言数据/水平扫描/16位真彩色(RGB565)/高位在前		其他的不要选
 *
 * @param   x,y		起点坐标
 * @param   width	图片宽度
 * @param   height	图片高度
 * @param   p		图片缓存数据起始地址
 *
 * @return  void
 */
 void LCD_Show_Image_With_Scale_ADV(u16 x, u16 y, u16 width, u16 height, const u8 *p, u8 scale)
{	
    if(x + width > LCD_Width || y + height > LCD_Height)
    {
//        return;
    }

    LCD_Address_Set(x, y, x + width - 1, y + height - 1);

    LCD_DC(1);

    u16 origin_width = width * scale;
//    u16 origin_height = height * scale;

    u32 length = width * height;
//    unsigned char gImage_mode[length*2];
	
    u32 i;
    u32 selected;
    u16 col = 0;
    u16 row = 0;
    for(i =0; i<length;  i++) {
		u16 tmp;		
#if 1
		row = i  /width;
		col = i % width;                     
		selected = row * origin_width* scale* scale  + col * scale* scale;
		
//		gImage_mode[2*i] = p[ selected  ];
//		gImage_mode[2*i + 1] = p[ selected  + 1];
		tmp = p[selected]<<8+p[selected  + 1];
		LCD_Write_HalfWord(tmp);
#endif
	}
    
}

void LCD_Show_Image_With_Scale(u16 x, u16 y, u16 width, u16 height, const u8 *p, u8 scale)
{	
    if(x + width > LCD_Width || y + height > LCD_Height)
    {
        //return;
    }

    LCD_Address_Set(x, y, x + width - 1, y + height - 1);

    LCD_DC(1);

    u16 origin_width = width * scale;
//    u16 origin_height = height * scale;

    u32 length = width * height;
    unsigned char gImage_mode[length*2];
	
    u32 i;
    u32 selected;
    u16 col = 0;
    u16 row = 0;
    for(i =0; i<length;  i++) {
#if 1
		row = i /width;
		col = i% width;                     
		selected = row * origin_width* scale* scale  + col * scale* scale;	
		
		gImage_mode[2*i] = p[selected];
		gImage_mode[2*i + 1] = p[selected + 1];	

#endif

#if 0
		u16 red,green,blue;
		u8 tmp1,tmp2,tmp3,tmp4;
		tmp1 = ((p[2*i]<<8)+p[2*i+1])&0x1F;
		tmp2 = ((p[2*i+2]<<8)+p[2*i+3])&0x1F;
		tmp3 = ((p[2*i+4]<<8)+p[2*i+5])&0x1F;
		tmp4 = ((p[2*i+6]<<8)+p[2*i+7])&0x1F;
		blue = (tmp1+tmp2+tmp3+tmp4)/4;		

		tmp1 = (((p[2*i]<<8)+p[2*i+1])>>5)&0x3F;
		tmp2 = (((p[2*i+2]<<8)+p[2*i+3])>>5)&0x3F;
		tmp3 = (((p[2*i+4]<<8)+p[2*i+5])>>5)&0x3F;
		tmp4 = (((p[2*i+6]<<8)+p[2*i+7])>>5)&0x3F;
		green = (tmp1+tmp2+tmp3+tmp4)/4;	

		tmp1 = (((p[2*i]<<8)+p[2*i+1])>>11)&0x1F;
		tmp2 = (((p[2*i+2]<<8)+p[2*i+3])>>11)&0x1F;
		tmp3 = (((p[2*i+4]<<8)+p[2*i+5])>>11)&0x1F;
		tmp4 = (((p[2*i+6]<<8)+p[2*i+7])>>11)&0x1F;
		red = (tmp1+tmp2+tmp3+tmp4)/4;			
			
		gImage_mode[2*i] = ((red<<3)+(green>>3));
		gImage_mode[2*i+1] = blue+ ((green&0x07)<<5);		

		//gImage_mode[2*i] = p[2*i+2];
		//gImage_mode[2*i+1] = p[2*i+3];
#endif
	}
    
//    LCD_SPI_Send((u8 *)p, width * height * 2);
	LCD_SPI_Send((u8 *)gImage_mode, width * height * 2);
}

void LCD_Show_Image(u16 x, u16 y, u16 width, u16 height, const u8 *p)
{	
    if(x + width > LCD_Width || y + height > LCD_Height)
    {
        return;
    }

    LCD_Address_Set(x, y, x + width - 1, y + height - 1);

    LCD_DC(1);

    LCD_SPI_Send((u8 *)p, width * height * 2);
}

void LCD_Show_num_Image(u16 x, u16 y, u16 width, u16 height, const u8 *p)
{	
    u16 i = 0;
    u32 size = 0;

    if(x + width > LCD_Width || y + height > LCD_Height)
    {
        return;
    }

    LCD_Address_Set(x, y, x + width - 1, y + height - 1);
	
    size = width* height* 2;	

    for(i = 0; i < size ; i++)
    {
 #if 1 
    	if((p[2*i] != 0x00)&&(p[2*i+1] != 0x00)) {
	       lcd_buf[2 * i] = POINT_COLOR >> 8;
	       lcd_buf[2 * i + 1] = POINT_COLOR;
    	} else {
	       lcd_buf[2 * i] = BACK_COLOR >> 8;
	       lcd_buf[2 * i + 1] = BACK_COLOR;		   
	}
#endif	
    }

    LCD_DC(1);
    LCD_SPI_Send(lcd_buf, size);

}
#if 0
/**
 * @brief	LCD初始化
 *
 * @param   x,y		显示坐标
 *
 * @return  void
 */
void Display_ALIENTEK_LOGO(u16 x, u16 y)
{
    LCD_Show_Image(x, y, 180, 120, ALIENTEK_LOGO);
}
#endif


/**
 * @brief	LCD初始化
 *
 * @param   void
 *
 * @return  void
 */
void LCD_Init(void)
{
#if 1
    GPIO_InitTypeDef  GPIO_Initure;
	
    __HAL_RCC_GPIOA_CLK_ENABLE();   //使能GPIOA时钟
    __HAL_RCC_GPIOB_CLK_ENABLE();   //使能GPIOB时钟

    
	//使用4线SPI 串口模式

	//GPIO初始化设置      
    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_8;	
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;   //高速

     HAL_GPIO_Init(GPIOA,&GPIO_Initure);//初始化
     
    GPIO_Initure.Pin=GPIO_PIN_0;	
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//推挽输出
    GPIO_Initure.Pull=GPIO_PULLUP;        //上拉
    GPIO_Initure.Speed=GPIO_SPEED_FAST;   //高速

     HAL_GPIO_Init(GPIOB,&GPIO_Initure);//初始化
	 
	lcd_spi_init();
#endif

       LCD_CS(0);
	LCD_RST(1);  
	HAL_Delay(20);
	
	LCD_RST(0);
	HAL_Delay(40);

	LCD_RST(1);
	HAL_Delay(10);

       LCD_Write_Cmd(0xFE);	
	LCD_Write_Data(0x04);
		
	LCD_Write_Cmd(0x00);	
	LCD_Write_Data(0xDC);
		
       LCD_Write_Cmd(0x01);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x02);	
	LCD_Write_Data(0x02);
		
    	LCD_Write_Cmd(0x03);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x04);	
	LCD_Write_Data(0x00);
				
   	LCD_Write_Cmd(0x05);	
	LCD_Write_Data(0x03);
		
	LCD_Write_Cmd(0x06);	
	LCD_Write_Data(0x16);
		
    	LCD_Write_Cmd(0x07);	
	LCD_Write_Data(0x13);
		
	LCD_Write_Cmd(0x08);	
	LCD_Write_Data(0x08);
		
    	LCD_Write_Cmd(0x09);	
	LCD_Write_Data(0xDC);
		
	LCD_Write_Cmd(0x0A);	
	LCD_Write_Data(0x00);	
         
    	LCD_Write_Cmd(0x0B);	
	LCD_Write_Data(0x02);
		
	LCD_Write_Cmd(0x0C);	
	LCD_Write_Data(0x00);
		
    	LCD_Write_Cmd(0x0D);	
	LCD_Write_Data(0x00);
	
	LCD_Write_Cmd(0x0e);	
	LCD_Write_Data(0x02);
		
	LCD_Write_Cmd(0x0F);	
	LCD_Write_Data(0x16);
		
    	LCD_Write_Cmd(0x10);	
	LCD_Write_Data(0x18);
		
	LCD_Write_Cmd(0x11);	
	LCD_Write_Data(0x08);
				
    	LCD_Write_Cmd(0x12);	
	LCD_Write_Data(0xC2);
		
	LCD_Write_Cmd(0x13);	
	LCD_Write_Data(0x00);
		
    	LCD_Write_Cmd(0x14);	
	LCD_Write_Data(0x34);
		
	LCD_Write_Cmd(0x15);	
	LCD_Write_Data(0x05);	

    	LCD_Write_Cmd(0x16);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x17);	
	LCD_Write_Data(0x03);
        
           
    	LCD_Write_Cmd(0x18);	
	LCD_Write_Data(0x15);
		
	LCD_Write_Cmd(0x19);	
	LCD_Write_Data(0x41);
		
    	LCD_Write_Cmd(0x1A);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x1B);	
	LCD_Write_Data(0xDC);
		
    	LCD_Write_Cmd(0x1C);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x1D);	
	LCD_Write_Data(0x04);
				
    	LCD_Write_Cmd(0x1E);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x1F);	
	LCD_Write_Data(0x00);
		
    	LCD_Write_Cmd(0x20);	
	LCD_Write_Data(0x03);
		
	LCD_Write_Cmd(0x21);	
	LCD_Write_Data(0x16);
		
    	LCD_Write_Cmd(0x22);	
	LCD_Write_Data(0x18);
		
	LCD_Write_Cmd(0x23);	
	LCD_Write_Data(0x08);
          
    	LCD_Write_Cmd(0x24);	
	LCD_Write_Data(0xDC);
		
	LCD_Write_Cmd(0x25);	
	LCD_Write_Data(0x00);
		
    	LCD_Write_Cmd(0x26);	
	LCD_Write_Data(0x04);
		
	LCD_Write_Cmd(0x27);	
	LCD_Write_Data(0x00);
		
    	LCD_Write_Cmd(0x28);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x29);	
	LCD_Write_Data(0x01);
				
    	LCD_Write_Cmd(0x2A);	
	LCD_Write_Data(0x16);
		
	LCD_Write_Cmd(0x2B);	
	LCD_Write_Data(0x18);	//28

    	LCD_Write_Cmd(0x2D);	
	LCD_Write_Data(0x08);
		
	LCD_Write_Cmd(0x4C);	
	LCD_Write_Data(0x99);
		
    	LCD_Write_Cmd(0x4D);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x4E);	
	LCD_Write_Data(0x00);
            
    	LCD_Write_Cmd(0x4F);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x50);	
	LCD_Write_Data(0x01);
		
    	LCD_Write_Cmd(0x51);	
	LCD_Write_Data(0x0A);
		
	LCD_Write_Cmd(0x52);	
	LCD_Write_Data(0x00);
		
    	LCD_Write_Cmd(0x5A);	
	LCD_Write_Data(0xE4);
		
	LCD_Write_Cmd(0x5E);	
	LCD_Write_Data(0x77);
				
    	LCD_Write_Cmd(0x5F);	
	LCD_Write_Data(0x77);
		
	LCD_Write_Cmd(0x60);	
	LCD_Write_Data(0x34);
		
    	LCD_Write_Cmd(0x61);	
	LCD_Write_Data(0x02);
		
	LCD_Write_Cmd(0x62);	
	LCD_Write_Data(0x81);
		
       LCD_Write_Cmd(0xFE);	
	LCD_Write_Data(0x07);  //
		
	LCD_Write_Cmd(0x07);	
	LCD_Write_Data(0x4F); //4
             
    	LCD_Write_Cmd(0xFE);	
	LCD_Write_Data(0x01);
		
	LCD_Write_Cmd(0x04);	
	LCD_Write_Data(0x80);
		
    	LCD_Write_Cmd(0x05);	
	LCD_Write_Data(0x65);
		
	LCD_Write_Cmd(0x06);	
	LCD_Write_Data(0x1E);
		
    	LCD_Write_Cmd(0x0E);	
	LCD_Write_Data(0x8B);
		
	LCD_Write_Cmd(0x0F);	
	LCD_Write_Data(0x8B);
				
    	LCD_Write_Cmd(0x10);	
	LCD_Write_Data(0x11);
		
	LCD_Write_Cmd(0x11);	
	LCD_Write_Data(0xA2);
		
    	LCD_Write_Cmd(0x13);//12	
	LCD_Write_Data(0x00);		 //80
		
	LCD_Write_Cmd(0x14);	
	LCD_Write_Data(0x81);
		
    	LCD_Write_Cmd(0x15);	
	LCD_Write_Data(0x82);
		
	LCD_Write_Cmd(0x18);	
	LCD_Write_Data(0x45);
            
    	LCD_Write_Cmd(0x19);	
	LCD_Write_Data(0x34);
		
	LCD_Write_Cmd(0x1A);	
	LCD_Write_Data(0x10);
		
    	LCD_Write_Cmd(0x1C);	
	LCD_Write_Data(0x57);
		
	LCD_Write_Cmd(0x1D);	
	LCD_Write_Data(0x02);
		
    	LCD_Write_Cmd(0x21);	
	LCD_Write_Data(0xF8);
		
	LCD_Write_Cmd(0x22);	
	LCD_Write_Data(0x90);
				
    	LCD_Write_Cmd(0x23);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x25);	
	LCD_Write_Data(0x0A);
		
    	LCD_Write_Cmd(0x26);	
	LCD_Write_Data(0x46);
		
	LCD_Write_Cmd(0x2A);	
	LCD_Write_Data(0x47);
		
    	LCD_Write_Cmd(0x2B);	
	LCD_Write_Data(0xFF);	

	LCD_Write_Cmd(0x2D);	
	LCD_Write_Data(0xFF);
          
    	LCD_Write_Cmd(0x2F);	
	LCD_Write_Data(0xAE);
		
	LCD_Write_Cmd(0x37);	
	LCD_Write_Data(0x0C);
		
    	LCD_Write_Cmd(0x3A);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x3B);	
	LCD_Write_Data(0x20);
		
    	LCD_Write_Cmd(0x3D);	
	LCD_Write_Data(0x0B);
		
	LCD_Write_Cmd(0x3F);	
	LCD_Write_Data(0x38);
				
    	LCD_Write_Cmd(0x40);	
	LCD_Write_Data(0x0B);
		
	LCD_Write_Cmd(0x41);	
	LCD_Write_Data(0x0B);
		
   	LCD_Write_Cmd(0x42);	
	LCD_Write_Data(0x11);
		
	LCD_Write_Cmd(0x43);	
	LCD_Write_Data(0x44);
		
    	LCD_Write_Cmd(0x44);	
	LCD_Write_Data(0x22);
		
	LCD_Write_Cmd(0x45);	
	LCD_Write_Data(0x55);
            
            
    	LCD_Write_Cmd(0x46);	
	LCD_Write_Data(0x33);
		
	LCD_Write_Cmd(0x47);	
	LCD_Write_Data(0x66);
		
    	LCD_Write_Cmd(0x4C);	
	LCD_Write_Data(0x11);
		
	LCD_Write_Cmd(0x4D);	
	LCD_Write_Data(0x44);
		
    	LCD_Write_Cmd(0x4E);	
	LCD_Write_Data(0x22);
		
	LCD_Write_Cmd(0x4F);	
	LCD_Write_Data(0x55);
				
    	LCD_Write_Cmd(0x50);	
	LCD_Write_Data(0x33);
		
	LCD_Write_Cmd(0x51);	
	LCD_Write_Data(0x66); 
		
    	LCD_Write_Cmd(0x56);	
	LCD_Write_Data(0x11);
		
	LCD_Write_Cmd(0x58);	
	LCD_Write_Data(0x44);	

    	LCD_Write_Cmd(0x59);	
	LCD_Write_Data(0x22);
		
	LCD_Write_Cmd(0x5A);	
	LCD_Write_Data(0x55);
           
    	LCD_Write_Cmd(0x5B);	
	LCD_Write_Data(0x33);
		
	LCD_Write_Cmd(0x5C);	
	LCD_Write_Data(0x66);
		
    	LCD_Write_Cmd(0x61);	
	LCD_Write_Data(0x11);
		
	LCD_Write_Cmd(0x62);	
	LCD_Write_Data(0x44);
		
    	LCD_Write_Cmd(0x63);	
	LCD_Write_Data(0x22);
		
	LCD_Write_Cmd(0x64);	
	LCD_Write_Data(0x55);
				
    	LCD_Write_Cmd(0x65);	
	LCD_Write_Data(0x33);
		
	LCD_Write_Cmd(0x66);	
	LCD_Write_Data(0x66);
		
    	LCD_Write_Cmd(0x6D);	
	LCD_Write_Data(0x90);
		
	LCD_Write_Cmd(0x6E);	
	LCD_Write_Data(0x40);
		
   	LCD_Write_Cmd(0x70);	
	LCD_Write_Data(0xA5);
		
	LCD_Write_Cmd(0x72);	
	LCD_Write_Data(0x04);
             
    	LCD_Write_Cmd(0x73);	
	LCD_Write_Data(0x15);
		
     	LCD_Write_Cmd(0xFE);	
     	LCD_Write_Data(0x0A);
		
    	LCD_Write_Cmd(0x29);	
	LCD_Write_Data(0x90);
		
    	LCD_Write_Cmd(0xFE);		 //page6
    	LCD_Write_Data(0x05);
		
   	LCD_Write_Cmd(0x05);	
	LCD_Write_Data(0x1f);  // ELVSS -2.4V (根据power ic 确认)
	
		
    	LCD_Write_Cmd(0xFE);	
    	LCD_Write_Data(0x00);			//page1
				

	LCD_Write_Cmd(0x51);	
	LCD_Write_Data(0xFF); // ADJUST BRIGHTNESS	  page 1 register 0x51


    	LCD_Write_Cmd(0x35);	
	LCD_Write_Data(0x00);
		
	LCD_Write_Cmd(0x3A);	
	LCD_Write_Data(0x55);	
 
             
    	LCD_Write_Cmd(0x2A);	
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x00);	
	LCD_Write_Data(0x00);	
	LCD_Write_Data(0xB3);
	
	LCD_Write_Cmd(0x2B);	
	LCD_Write_Data(0x00);
	LCD_Write_Data(0x00);	
	LCD_Write_Data(0x00);	
	LCD_Write_Data(0x77);	
			
	LCD_Write_Cmd(0x53);	
	LCD_Write_Data(0x28);//28	
             
 	LCD_Write_Cmd(0x11);	
	LCD_Write_Data(0x00);

 
 	
  	HAL_Delay(20);		

 	LCD_Write_Cmd(0x29);	
	LCD_Write_Data(0x00);

	LCD_Write_Cmd(0x2c);

	flush_enable = 0;
}


