#include "sys.h"
#include "lcd.h"
#include "font.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
//#include "alientek_log.h"

/*********************************************************************************
 *	LCD TFT��������
 *	��������:2018/10/27
 *	�汾��V1.0
 *	��Ȩ���У�����ؾ���
 *	All rights reserved
 *	******************************************************************************
 *	��ʼ�汾
 *	******************************************************************************/

//LCD�����С���ã��޸Ĵ�ֵʱ��ע�⣡�������޸�������ֵʱ���ܻ�Ӱ�����º���	LCD_Clear/LCD_Fill/LCD_DrawLine
#define LCD_TOTAL_BUF_SIZE	(180*120*2)
#define LCD_Buf_Size 1152
static u8 lcd_buf[LCD_Buf_Size];

u16	POINT_COLOR = BLACK;	//������ɫ	Ĭ��Ϊ��ɫ
u16	BACK_COLOR 	= WHITE;	//������ɫ	Ĭ��Ϊ��ɫ
u8 flush_enable = 0;

SPI_HandleTypeDef SPI_Lcd_Handler;  //SPI���

void lcd_spi_init(void)
{
    SPI_Lcd_Handler.Instance=SPI1;                         //SP1
    SPI_Lcd_Handler.Init.Mode=SPI_MODE_MASTER;             //����SPI����ģʽ������Ϊ��ģʽ
    SPI_Lcd_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //����SPI�������˫�������ģʽ:SPI����Ϊ˫��ģʽ
    SPI_Lcd_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
    SPI_Lcd_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
    SPI_Lcd_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //����ͬ��ʱ�ӵĵڶ��������أ��������½������ݱ�����
    SPI_Lcd_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
    SPI_Lcd_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_2;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ256
    SPI_Lcd_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
    SPI_Lcd_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //�ر�TIģʽ
    SPI_Lcd_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//�ر�Ӳ��CRCУ��
    SPI_Lcd_Handler.Init.CRCPolynomial=7;                  //CRCֵ����Ķ���ʽ
    SPI_Lcd_Handler.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    SPI_Lcd_Handler.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	
    HAL_SPI_Init(&SPI_Lcd_Handler);//��ʼ��
    
    __HAL_SPI_ENABLE(&SPI_Lcd_Handler);                    //ʹ��SPI5
			
}

/**
 * @brief	LCD�ײ�SPI�������ݺ���
 *
 * @param   data	���ݵ���ʼ��ַ
 * @param   size	�������ݴ�С
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
 * @brief	д���LCD
 *
 * @param   cmd		��Ҫ���͵�����
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
 * @brief	д���ݵ�LCD
 *
 * @param   data		��Ҫ���͵�����
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
 * @brief	д����ֵ����ݵ�LCD
 *
 * @param   da		��Ҫ���͵�����
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
 * ��������д��LCD��������
 *
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
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
 * ��LCD��ʾ
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
 * �ر�LCD��ʾ
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
 * ��һ����ɫ���LCD��
 *
 * @param   color	������ɫ
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
 * ��һ����ɫ�����������
 *
 * @param   x_start,y_start     �������
 * @param   x_end,y_end			�յ�����
 * @param   color       		�����ɫ
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
 * ��?��?????��?BUF��?3???????����
 *
 * @param   x_start,y_start     ?e��?��?����
 * @param   x_end,y_end			??��?��?����
 * @param   color       		��?3???��?
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
 * ���㺯��
 *
 * @param   x,y		��������
 *
 * @return  void
 */
void LCD_Draw_Point(u16 x, u16 y)
{
    LCD_Address_Set(x, y, x, y);
    LCD_Write_HalfWord(POINT_COLOR);
}

/**
 * �������ɫ����
 *
 * @param   x,y		��������
 *
 * @return  void
 */
void LCD_Draw_ColorPoint(u16 x, u16 y,u16 color)
{
    LCD_Address_Set(x, y, x, y);
    LCD_Write_HalfWord(color);
}



/**
 * @brief	���ߺ���(ֱ�ߡ�б��)
 *
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
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
        /*���ٻ�ˮƽ��*/
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
 * @brief	��һ������
 *
 * @param   x1,y1	�������
 * @param   x2,y2	�յ�����
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
 * @brief	��һ��Բ
 *
 * @param   x0,y0	Բ������
 * @param   r       Բ�뾶
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
 * @brief	��ʾһ��ASCII���ַ�
 *
 * @param   x,y		��ʾ��ʼ����
 * @param   chr		��Ҫ��ʾ���ַ�
 * @param   size	�����С(֧��16/24/32������)
 *
 * @return  void
 */
void LCD_ShowChar(u16 x, u16 y, char chr, u8 size)
{
    u8 temp, t1, t;
    u8 csize;		//�õ�����һ���ַ���Ӧ������ռ���ֽ���
    u16 colortemp;
    u8 sta;

    chr = chr - ' '; //�õ�ƫ�ƺ��ֵ��ASCII�ֿ��Ǵӿո�ʼȡģ������-' '���Ƕ�Ӧ�ַ����ֿ⣩

    if((x > (LCD_Width - size / 2)) || (y > (LCD_Height - size)))	return;

    LCD_Address_Set(x, y, x + size / 2 - 1, y + size - 1);//(x,y,x+8-1,y+16-1)

    if((size == 16) || (size == 32) )	//16��32������
    {
        csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);

        for(t = 0; t < csize; t++)
        {
            if(size == 16)temp = asc2_1608[chr][t];	//����1608����

           else if(size == 32)temp = asc2_3216[chr][t];	//����3216����

            else return;			//û�е��ֿ�

            for(t1 = 0; t1 < 8; t1++)
            {
                if(temp & 0x80) colortemp = POINT_COLOR;

                else colortemp = BACK_COLOR;

                LCD_Write_HalfWord(colortemp);
                temp <<= 1;
            }
        }
    }

	else if  (size == 12)	//12������
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
	
    else if(size == 24)		//24������
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
 * @brief	m^n����
 *
 * @param   m,n		�������
 *
 * @return  m^n�η�
 */
static u32 LCD_Pow(u8 m, u8 n)
{
    u32 result = 1;

    while(n--)result *= m;

    return result;
}

/**
 * @brief	��ʾ����,��λΪ0����ʾ
 *
 * @param   x,y		�������
 * @param   num		��Ҫ��ʾ������,���ַ�Χ(0~4294967295)
 * @param   len		��Ҫ��ʾ��λ��
 * @param   size	�����С
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
 * @brief	��ʾ����,��λΪ0,���Կ�����ʾΪ0���ǲ���ʾ
 *
 * @param   x,y		�������
 * @param   num		��Ҫ��ʾ������,���ַ�Χ(0~999999999)
 * @param   len		��Ҫ��ʾ��λ��
 * @param   size	�����С
 * @param   mode	1:��λ��ʾ0		0:��λ����ʾ
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
 * @brief	��ʾ�ַ���
 *
 * @param   x,y		�������
 * @param   width	�ַ���ʾ������
 * @param   height	�ַ���ʾ����߶�
 * @param   size	�����С
 * @param   p		�ַ�����ʼ��ַ
 *
 * @return  void
 */
void LCD_ShowString(u16 x, u16 y, u16 width, u16 height, u8 size, char *p)
{
    u8 x0 = x;
    width += x;
    height += y;

    while((*p <= '~') && (*p >= ' ')) //�ж��ǲ��ǷǷ��ַ�!
    {
        if(x >= width)
        {
            x = x0;
            y += size;
        }

        if(y >= height)break; //�˳�

        LCD_ShowChar(x, y, *p, size);
        x += size / 2;
        p++;
    }
}


/**
 * @brief	��ʾͼƬ
 *
 * @remark	Image2Lcdȡģ��ʽ��	C��������/ˮƽɨ��/16λ���ɫ(RGB565)/��λ��ǰ		�����Ĳ�Ҫѡ
 *
 * @param   x,y		�������
 * @param   width	ͼƬ���
 * @param   height	ͼƬ�߶�
 * @param   p		ͼƬ����������ʼ��ַ
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
 * @brief	LCD��ʼ��
 *
 * @param   x,y		��ʾ����
 *
 * @return  void
 */
void Display_ALIENTEK_LOGO(u16 x, u16 y)
{
    LCD_Show_Image(x, y, 180, 120, ALIENTEK_LOGO);
}
#endif


/**
 * @brief	LCD��ʼ��
 *
 * @param   void
 *
 * @return  void
 */
void LCD_Init(void)
{
#if 1
    GPIO_InitTypeDef  GPIO_Initure;
	
    __HAL_RCC_GPIOA_CLK_ENABLE();   //ʹ��GPIOAʱ��
    __HAL_RCC_GPIOB_CLK_ENABLE();   //ʹ��GPIOBʱ��

    
	//ʹ��4��SPI ����ģʽ

	//GPIO��ʼ������      
    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_8;	
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//�������
    GPIO_Initure.Pull=GPIO_PULLUP;        //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;   //����

     HAL_GPIO_Init(GPIOA,&GPIO_Initure);//��ʼ��
     
    GPIO_Initure.Pin=GPIO_PIN_0;	
    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//�������
    GPIO_Initure.Pull=GPIO_PULLUP;        //����
    GPIO_Initure.Speed=GPIO_SPEED_FAST;   //����

     HAL_GPIO_Init(GPIOB,&GPIO_Initure);//��ʼ��
	 
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
	LCD_Write_Data(0x1f);  // ELVSS -2.4V (����power ic ȷ��)
	
		
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


