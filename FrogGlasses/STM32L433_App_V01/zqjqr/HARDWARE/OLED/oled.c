#include "oled.h"
#include "font.h"  	 
#include "usr_hardware.h"

/********************************************************************************	 
 * ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
 * ALIENTEK MiniFly_Remotor
 * SSD1306 OLED��������
 * ������ʽ:STM32Ӳ��SPI 
 * ����ԭ��@ALIENTEK
 * ������̳:www.openedv.com
 * ��������:2018/6/1
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
 * All rights reserved
********************************************************************************/

//OLED���Դ�
//��Ÿ�ʽ����.
//[0]0 1 2 3 ... 127	
//[1]0 1 2 3 ... 127	
//[2]0 1 2 3 ... 127	
//[3]0 1 2 3 ... 127	
//[4]0 1 2 3 ... 127	
//[5]0 1 2 3 ... 127	
//[6]0 1 2 3 ... 127	
//[7]0 1 2 3 ... 127 		   
volatile static u8 oledGram[128][8];


//void SPI_WByte(u8 TxData)
//{
//	u8 i;	
//	
//	for(i=0;i<8;i++)
//	{			  
//		OLED_SCLK_OFF;
//		if(TxData&0x80) HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_SET);//OLED_SDIN_ON;//OLED_SDIN=1;
//		else HAL_GPIO_WritePin(GPIOB,GPIO_PIN_13,GPIO_PIN_RESET);//OLED_SDIN_OFF;//OLED_SDIN=0;
//		OLED_SCLK_ON;
//		TxData<<=1;   
//	}		
//}

//��SSD1306д��һ���ֽڡ���SPIģʽ��
//dat:Ҫд�������/����
//cmd:����/�����־ 0,��ʾ����;1,��ʾ����;	
void oled_wr_byte(u8 dat,u8 cmd)
{		  
	if(cmd == OLED_CMD) {
		OLED_RS_OFF;	
	}
	else {
		OLED_RS_ON;
	}

	usr_spi_write_byte(dat);
//	SPI_WByte(dat);		 
	  
	OLED_RS_ON; 
}
//�����Դ浽LCD		 
void oled_refreshGram(void)
{
	u8 i,n;		    
	for(i=0;i<8;i++)  
	{  
		oled_wr_byte (0xb0+i,OLED_CMD);    //����ҳ��ַ��0~7��
		oled_wr_byte (0x00,OLED_CMD);      //������ʾλ�á��е͵�ַ
		oled_wr_byte (0x10,OLED_CMD);      //������ʾλ�á��иߵ�ַ 

		for(n=0;n<128;n++) {
			oled_wr_byte(oledGram[n][i],OLED_DATA); 
		}	
	}   
}  	  
//����OLED��ʾ    
void oled_displayON(void)
{
	oled_wr_byte(0X8D,OLED_CMD);  //SET DCDC����
	oled_wr_byte(0X14,OLED_CMD);  //DCDC ON
	oled_wr_byte(0XAF,OLED_CMD);  //DISPLAY ON
}
//�ر�OLED��ʾ     
void oled_displayOFF(void)
{
	oled_wr_byte(0X8D,OLED_CMD);  //SET DCDC����
	oled_wr_byte(0X10,OLED_CMD);  //DCDC OFF
	oled_wr_byte(0XAE,OLED_CMD);  //DISPLAY OFF
}		   			 
//��������,������,������Ļ�Ǻ�ɫ��!��û����һ��!!!	  
void oled_clear(void)  
{  
	u8 i,n;  
	for(i=0;i<8;i++)
		for(n=0;n<128;n++)
			oledGram[n][i]=0X00;  
	oled_refreshGram();//������ʾ
}
//���� 
//x:0~127
//y:0~63
//t:1 ��� 0,���				   
void oled_drawPoint(u8 x,u8 y,u8 t)
{
	u8 pos,bx,temp=0;
	if(x>127||y>63)return;//������Χ��.
	pos=7-y/8;
	bx=y%8;
	temp=1<<(7-bx);
	if(t)oledGram[x][pos]|=temp;
	else oledGram[x][pos]&=~temp;	    
}
//���� 
//x:0~127
//y:0~63
u8 oled_readPoint(u8 x,u8 y)
{
	u8 pos,bx,temp=0x00;
	//x = 127-x;
	y = 63-y;
	pos=y/8;
	bx=y%8;
	temp=1<<bx;
  if(temp&oledGram[x][pos]) return 1;
	return 0;
}
//x1,y1,x2,y2 �������ĶԽ�����
//ȷ��x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63	 	 
//dot:0,���;1,���	  
void oled_fill(u8 x1,u8 y1,u8 x2,u8 y2,u8 dot)  
{  
	u8 x,y;  
	for(x=x1;x<=x2;x++)
		for(y=y1;y<=y2;y++)
			oled_drawPoint(x,y,dot);											    
//	oled_refreshGram();//������ʾ
}
//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
//f_w:�ֿ�
//f_h:�ָ�
//mode:0,������ʾ;1,������ʾ				 
void oled_showChar(u8 x,u8 y,u8 chr,u8 f_w,u8 f_h,u8 mode)
{      			    
	u8 temp,t,t1;
	u8 y0=y;	
	u8 csize=(f_h/8+((f_h%8)?1:0))*f_w;//�õ����ɷֱ��ַ���ռ���ֽ���
	chr=chr-' ';//�õ�ƫ�ƺ��ֵ		 
	for(t=0;t<csize;t++)
	{   
		if(f_w==6&&f_h==8)temp=asc2_0608[chr][t];		//����0608ascii����
		else if(f_w==6&&f_h==12)temp=asc2_0612[chr][t];	//����0612ascii����
		else if(f_w==12&&f_h==24)temp=asc2_1224[chr][t];//����1224ascii����	
		else return;	//û�е��ֿ�
		for(t1=0;t1<8;t1++)
		{
			if(temp&0x80)oled_drawPoint(x,y,mode);
			else oled_drawPoint(x,y,!mode);
			temp<<=1;
			y++;
			if((y-y0)==f_h)
			{
				y=y0;
				x++;
				break;
			}
		}  	 
	}     
}
//m^n����
u32 mypow(u8 m,u8 n)
{
	u32 result=1;	 
	while(n--)result*=m;    
	return result;
}				  
//��ʾ2������
//x,y :�������	 
//len :���ֵ�λ��
//f_w:�ֿ�
//f_h:�ָ�
//mode:ģʽ	0,���ģʽ;1,����ģʽ
//num:��ֵ(0~4294967295);	 		  
void oled_showNum(u8 x,u8 y,u32 num,u8 len,u8 f_w,u8 f_h)
{         	
	u8 t,temp;
	u8 enshow=0;						   
	for(t=0;t<len;t++)
	{
		temp=(num/mypow(10,len-t-1))%10;
		if(enshow==0&&t<(len-1))
		{
			if(temp==0)
			{
				oled_showChar(x+(f_w)*t,y,' ',f_w,f_h,1);
				continue;
			}
			else 
				enshow=1; 
		}
	 	oled_showChar(x+(f_w)*t,y,temp+'0',f_w,f_h,1); 
	}
} 
//��ʾ�ַ���
//x,y:�������  
//f_w:�ֿ�
//f_h:�ָ�
//*p:�ַ�����ʼ��ַ 
void oled_showString(u8 x,u8 y,const u8 *p,u8 f_w,u8 f_h)
{	
    while((*p<='~')&&(*p>=' '))//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(f_w))){x=0;y+=f_h;}
        if(y>(64-f_h)){y=x=0;oled_clear();}
        oled_showChar(x,y,*p,f_w,f_h,1);	 
        x+=f_w;
        p++;
    }  
	
}
//��ʾͼƬ
//x,y:�������  
//p_w:ͼƬ����λ���أ�
//p_h:ͼƬ�ߣ���λ���أ�
//*p:ͼƬ��ʼ��ַ 
void oled_showPicture(u8 x,u8 y,const u8 *p,u8 p_w,u8 p_h)
{	
	u8 temp,i,col,row;
	u8 y0=y;
	u8 width=p_w;
	if(x+p_w>128)width=128-p_w;//ʵ����ʾ���
	u8 high=p_h;
	if(y+p_h>64)high=64-p_h;//ʵ����ʾ�߶�
	u8 exp_col_bytes=(p_h/8+((p_h%8)?1:0));//��ʾһ�е��ֽ���
	u8 act_col_bytes=(high/8+((high%8)?1:0));//ʵ����ʾһ�е��ֽ���
	
	for(row=0;row<width;row++)//��++
	{
		for(col=0;col<act_col_bytes;col++)//��ʾһ��
		{   
			temp = p[col+row*exp_col_bytes];
			for(i=0;i<8;i++)
			{
				if(temp&0x80)oled_drawPoint(x,y,1);
				else oled_drawPoint(x,y,0);
				temp<<=1;
				y++;
				if((y-y0)==high)
				{
					y=y0;
					x++;
					break;
				}		
			} 
		}
	}		
}
//��ʼ��OLED				    
void oledInit(void)
{ 	
//    GPIO_InitTypeDef  GPIO_Initure;
//	
//    __HAL_RCC_GPIOA_CLK_ENABLE();   //ʹ��GPIOAʱ��
//    __HAL_RCC_GPIOB_CLK_ENABLE();   //ʹ��GPIOBʱ��
//    __HAL_RCC_GPIOC_CLK_ENABLE();   //ʹ��GPIOCʱ��
//    __HAL_RCC_GPIOD_CLK_ENABLE();   //ʹ��GPIODʱ��
//    __HAL_RCC_GPIOH_CLK_ENABLE();   //ʹ��GPIOHʱ��
//    
//	//ʹ��4��SPI ����ģʽ

//	//GPIO��ʼ������      
//    //PB4,7
//    GPIO_Initure.Pin=GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_12|GPIO_PIN_13;	
//    GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//�������
//    GPIO_Initure.Pull=GPIO_PULLUP;        //����
//    GPIO_Initure.Speed=GPIO_SPEED_FAST;   //����

//     HAL_GPIO_Init(GPIOB,&GPIO_Initure);//��ʼ��

	OLED_SDIN_ON;
	OLED_SCLK_ON;
 
	OLED_RS_ON;	 
	
	OLED_RST_OFF
        HAL_Delay(100);
	OLED_RST_ON
	
	oled_wr_byte(0xAE,OLED_CMD); //�ر���ʾ
	oled_wr_byte(0xD5,OLED_CMD); //����ʱ�ӷ�Ƶ����,��Ƶ��
	oled_wr_byte(0x80,OLED_CMD); //[3:0],��Ƶ����;[7:4],��Ƶ��
	oled_wr_byte(0xA8,OLED_CMD); //��������·��
	oled_wr_byte(0X3F,OLED_CMD); //Ĭ��0X3F(1/64) 
	oled_wr_byte(0xD3,OLED_CMD); //������ʾƫ��
	oled_wr_byte(0X00,OLED_CMD); //Ĭ��Ϊ0

	oled_wr_byte(0x40,OLED_CMD); //������ʾ��ʼ�� [5:0],����.
													    
	oled_wr_byte(0x8D,OLED_CMD); //��ɱ�����
	oled_wr_byte(0x14,OLED_CMD); //bit2������/�ر�
	oled_wr_byte(0x20,OLED_CMD); //�����ڴ��ַģʽ
	oled_wr_byte(0x02,OLED_CMD); //[1:0],00���е�ַģʽ;01���е�ַģʽ;10,ҳ��ַģʽ;Ĭ��10;
	oled_wr_byte(0xA1,OLED_CMD); //���ض�������,bit0:0,0->0;1,0->127;
	oled_wr_byte(0xC0,OLED_CMD); //����COMɨ�跽��;bit3:0,��ͨģʽ;1,�ض���ģʽ COM[N-1]->COM0;N:����·��
	oled_wr_byte(0xDA,OLED_CMD); //����COMӲ����������
	oled_wr_byte(0x12,OLED_CMD); //[5:4]����
		 
	oled_wr_byte(0x81,OLED_CMD); //�Աȶ�����
	oled_wr_byte(0xEF,OLED_CMD); //1~255;Ĭ��0X7F (��������,Խ��Խ��)
	oled_wr_byte(0xD9,OLED_CMD); //����Ԥ�������
	oled_wr_byte(0xf1,OLED_CMD); //[3:0],PHASE 1;[7:4],PHASE 2;
	oled_wr_byte(0xDB,OLED_CMD); //����VCOMH ��ѹ����
	oled_wr_byte(0x30,OLED_CMD); //[6:4] 000,0.65*vcc;001,0.77*vcc;011,0.83*vcc;

	oled_wr_byte(0xA4,OLED_CMD); //ȫ����ʾ����;bit0:1,����;0,�ر�;(����/����)
	oled_wr_byte(0xA6,OLED_CMD); //������ʾ��ʽ;bit0:1,������ʾ;0,������ʾ	    						   
	oled_wr_byte(0xAF,OLED_CMD); //������ʾ	 
	oled_clear();
}  


