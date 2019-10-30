#include "app\mx25Lxxx.h"
#include "spi.h"
#include <stdio.h>

configParameters m_ConfigParams;

u16 MX25Lxxx_ReadID(void)
{
	u16 Temp = 0;	  
	MX25Lxxx_CS(0);				    
	SPI2_ReadWriteByte(0x90);//���Ͷ�ȡID����	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	    
	SPI2_ReadWriteByte(0x00); 	 			   
	Temp|=SPI2_ReadWriteByte(0xFF) << 8;  
	Temp|=SPI2_ReadWriteByte(0xFF);	 
	MX25Lxxx_CS(1);				    
	return Temp;
}   	

void MX25Lxxx_Test(void)
{
	u16 tmpData;
	u8 tmpTest[8] = {0x31,0x32,0x33,0x34,0x36,0x37,0x38,0x39};
	u8 tmpRead[8];
	tmpData = MX25Lxxx_ReadID();
	printf("tmpID:%x\r\n",tmpData);
	MX25Lxxx_Write((u8 *)tmpTest, 1024, 8);	
	MX25Lxxx_Read((u8 *)tmpRead, 1024, 8);	
	printf("%02x-%02x-%02x-%02x-",tmpRead[0],tmpRead[1],tmpRead[2],tmpRead[3]);
	printf("%02x-%02x-%02x-%02x-\r\n",tmpRead[4],tmpRead[5],tmpRead[6],tmpRead[7]);	
}
void MX25Lxxx_PowerDown(void)   
{ 
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(W25X_PowerDown);     //���͵�������  
	MX25Lxxx_CS(1);                            //ȡ��Ƭѡ     	      
	HAL_Delay(3);                            //�ȴ�TPD  
}
void MX25Lxxx_WakeUp(void)   
{  
	MX25Lxxx_CS(0);                                //ʹ������   
	SPI2_ReadWriteByte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
	MX25Lxxx_CS(1);                                //ȡ��Ƭѡ     	      
	HAL_Delay(3);                                //�ȴ�TRES1
}   

void MX25Lxxx_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
	u16 i;   										    
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ReadData);      //���Ͷ�ȡ���� 

	SPI2_ReadWriteByte((u8)((ReadAddr)>>16));   //����24bit��ַ    
	SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
	SPI2_ReadWriteByte((u8)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=SPI2_ReadWriteByte(0XFF);    //ѭ������  
	}
	MX25Lxxx_CS(1);  				    	      
}  

void MX25Lxxx_Write_Enable(void)   
{
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(W25X_WriteEnable);   //����дʹ��  
	MX25Lxxx_CS(1);                            //ȡ��Ƭѡ     	      
}
void MX25Lxxx_Write_Disable(void)   
{  
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(W25X_WriteDisable);  //����д��ָֹ��    
	MX25Lxxx_CS(1);                            //ȡ��Ƭѡ     	      
} 

u8 MX25Lxxx_ReadSR(u8 regno)   
{  
	u8 byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //��״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //��״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //��״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }    
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(command);            //���Ͷ�ȡ״̬�Ĵ�������    
	byte=SPI2_ReadWriteByte(0Xff);          //��ȡһ���ֽ�  
	MX25Lxxx_CS(1);                            //ȡ��Ƭѡ     
	return byte;   
} 

void MX25Lxxx_Write_SR(u8 regno,u8 sr)   
{   
    u8 command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //д״̬�Ĵ���1ָ��
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //д״̬�Ĵ���2ָ��
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //д״̬�Ĵ���3ָ��
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }   
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(command);            //����дȡ״̬�Ĵ�������    
	SPI2_ReadWriteByte(sr);                 //д��һ���ֽ�  
	MX25Lxxx_CS(1);                            //ȡ��Ƭѡ     	      
}   

void MX25Lxxx_Wait_Busy(void)   
{   
	while((MX25Lxxx_ReadSR(1)&0x01)==0x01);   // �ȴ�BUSYλ���
}  
void MX25Lxxx_Erase_Chip(void)   
{                                   
	MX25Lxxx_Write_Enable();                  //SET WEL 
	MX25Lxxx_Wait_Busy();   
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(W25X_ChipErase);        //����Ƭ��������  
	MX25Lxxx_CS(1);                            //ȡ��Ƭѡ     	      
	MX25Lxxx_Wait_Busy();   				   //�ȴ�оƬ��������
}   
void MX25Lxxx_Erase_Sector(u32 Dst_Addr)   
{  
	//����falsh�������,������   
	//printf("fe:%x\r\n",Dst_Addr);	  
	Dst_Addr*=4096;
	MX25Lxxx_Write_Enable();                  //SET WEL 	 
	MX25Lxxx_Wait_Busy();   
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(W25X_SectorErase);   //������������ָ�� 

	SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //����24bit��ַ    
	SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
	SPI2_ReadWriteByte((u8)Dst_Addr);  
	MX25Lxxx_CS(1);                            //ȡ��Ƭѡ     	      
	MX25Lxxx_Wait_Busy();   				    //�ȴ��������
}  

void MX25Lxxx_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u16 i;  
	MX25Lxxx_Write_Enable();                  //SET WEL 
	MX25Lxxx_CS(0);                            //ʹ������   
	SPI2_ReadWriteByte(W25X_PageProgram);   //����дҳ����   

	SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //����24bit��ַ    
	SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
	SPI2_ReadWriteByte((u8)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)
	{
		SPI2_ReadWriteByte(pBuffer[i]);//ѭ��д��  
	}
	MX25Lxxx_CS(1);                            //ȡ��Ƭѡ 
	MX25Lxxx_Wait_Busy();					   //�ȴ�д�����
} 

void MX25Lxxx_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //��ҳʣ����ֽ���		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//������256���ֽ�
	while(1)
	{	   
		MX25Lxxx_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//д�������
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //��ȥ�Ѿ�д���˵��ֽ���
			if(NumByteToWrite>256)pageremain=256; //һ�ο���д��256���ֽ�
			else pageremain=NumByteToWrite; 	  //����256���ֽ���
		}
	};	    
} 

u8 W25QXX_BUFFER[4096];		 
void MX25Lxxx_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 
	u32 secpos;
	u16 secoff;
	u16 secremain;	   
 	u16 i;    
	u8 * W25QXX_BUF;	  
   	W25QXX_BUF=W25QXX_BUFFER;	     
 	secpos=WriteAddr/4096;//������ַ  
	secoff=WriteAddr%4096;//�������ڵ�ƫ��
	secremain=4096-secoff;//����ʣ��ռ��С   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//������
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//������4096���ֽ�
	while(1) 
	{	
		MX25Lxxx_Read(W25QXX_BUF,secpos*4096,4096);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			MX25Lxxx_Erase_Sector(secpos);//�����������
			for(i=0;i<secremain;i++)	   //����
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			MX25Lxxx_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//д����������  
		}
		else 
		{
			MX25Lxxx_Write_NoCheck(pBuffer,WriteAddr,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		}
		if(NumByteToWrite==secremain)
		{
			break;//д�������
		}
		else//д��δ����
		{
			secpos++;//������ַ��1
			secoff=0;//ƫ��λ��Ϊ0 	 

		   	pBuffer+=secremain;  //ָ��ƫ��
			WriteAddr+=secremain;//д��ַƫ��	   
		   	NumByteToWrite-=secremain;				//�ֽ����ݼ�
			if(NumByteToWrite>4096)
			{
				secremain=4096;	//��һ����������д����
			}
			else 
			{
				secremain=NumByteToWrite;			//��һ����������д����
			}
		}	 
	};	 
}


void MX25Lxxx_ReadConfig(void)
{
	u8 * tmpRead;
	tmpRead = (u8*)(&m_ConfigParams);
	MX25Lxxx_Read((u8 *)tmpRead, MX25L_CONFIG_SAVED_ADDR, sizeof(m_ConfigParams));	
	printf("UpFW:%d\r\n",m_ConfigParams.m_FWUpdatedFlag);
	printf("Xb:%d  Yb:%d  Zb:%d  \r\n",m_ConfigParams.m_Mag_Max[0],m_ConfigParams.m_Mag_Max[1],m_ConfigParams.m_Mag_Max[2]);
	printf("Xs:%d  Ys:%d  Zs:%d  \r\n",m_ConfigParams.m_Mag_Min[0],m_ConfigParams.m_Mag_Min[1],m_ConfigParams.m_Mag_Min[2]);
}

uint8_t Check_FW_Update_Flag(uint8_t * Entry)
{
	MX25Lxxx_ReadConfig();
	if(m_ConfigParams.m_FWUpdatedFlag == 1)
	{
		*Entry = m_ConfigParams.m_FW_Entry;
		return 1;
	}
	else
	{
		return 0;
	}
	//return m_ConfigParams.m_FWUpdatedFlag;
	//return 0;
}

