#include "app\mx25Lxxx.h"
#include "spi.h"
#include <stdio.h>

configParameters m_ConfigParams;

u16 MX25Lxxx_ReadID(void)
{
	u16 Temp = 0;	  
	MX25Lxxx_CS(0);				    
	SPI2_ReadWriteByte(0x90);//发送读取ID命令	    
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
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(W25X_PowerDown);     //发送掉电命令  
	MX25Lxxx_CS(1);                            //取消片选     	      
	HAL_Delay(3);                            //等待TPD  
}
void MX25Lxxx_WakeUp(void)   
{  
	MX25Lxxx_CS(0);                                //使能器件   
	SPI2_ReadWriteByte(W25X_ReleasePowerDown);  //  send W25X_PowerDown command 0xAB    
	MX25Lxxx_CS(1);                                //取消片选     	      
	HAL_Delay(3);                                //等待TRES1
}   

void MX25Lxxx_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead)   
{ 
	u16 i;   										    
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(W25X_ReadData);      //发送读取命令 

	SPI2_ReadWriteByte((u8)((ReadAddr)>>16));   //发送24bit地址    
	SPI2_ReadWriteByte((u8)((ReadAddr)>>8));   
	SPI2_ReadWriteByte((u8)ReadAddr);   
	for(i=0;i<NumByteToRead;i++)
	{ 
		pBuffer[i]=SPI2_ReadWriteByte(0XFF);    //循环读数  
	}
	MX25Lxxx_CS(1);  				    	      
}  

void MX25Lxxx_Write_Enable(void)   
{
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(W25X_WriteEnable);   //发送写使能  
	MX25Lxxx_CS(1);                            //取消片选     	      
}
void MX25Lxxx_Write_Disable(void)   
{  
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(W25X_WriteDisable);  //发送写禁止指令    
	MX25Lxxx_CS(1);                            //取消片选     	      
} 

u8 MX25Lxxx_ReadSR(u8 regno)   
{  
	u8 byte=0,command=0; 
    switch(regno)
    {
        case 1:
            command=W25X_ReadStatusReg1;    //读状态寄存器1指令
            break;
        case 2:
            command=W25X_ReadStatusReg2;    //读状态寄存器2指令
            break;
        case 3:
            command=W25X_ReadStatusReg3;    //读状态寄存器3指令
            break;
        default:
            command=W25X_ReadStatusReg1;    
            break;
    }    
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(command);            //发送读取状态寄存器命令    
	byte=SPI2_ReadWriteByte(0Xff);          //读取一个字节  
	MX25Lxxx_CS(1);                            //取消片选     
	return byte;   
} 

void MX25Lxxx_Write_SR(u8 regno,u8 sr)   
{   
    u8 command=0;
    switch(regno)
    {
        case 1:
            command=W25X_WriteStatusReg1;    //写状态寄存器1指令
            break;
        case 2:
            command=W25X_WriteStatusReg2;    //写状态寄存器2指令
            break;
        case 3:
            command=W25X_WriteStatusReg3;    //写状态寄存器3指令
            break;
        default:
            command=W25X_WriteStatusReg1;    
            break;
    }   
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(command);            //发送写取状态寄存器命令    
	SPI2_ReadWriteByte(sr);                 //写入一个字节  
	MX25Lxxx_CS(1);                            //取消片选     	      
}   

void MX25Lxxx_Wait_Busy(void)   
{   
	while((MX25Lxxx_ReadSR(1)&0x01)==0x01);   // 等待BUSY位清空
}  
void MX25Lxxx_Erase_Chip(void)   
{                                   
	MX25Lxxx_Write_Enable();                  //SET WEL 
	MX25Lxxx_Wait_Busy();   
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(W25X_ChipErase);        //发送片擦除命令  
	MX25Lxxx_CS(1);                            //取消片选     	      
	MX25Lxxx_Wait_Busy();   				   //等待芯片擦除结束
}   
void MX25Lxxx_Erase_Sector(u32 Dst_Addr)   
{  
	//监视falsh擦除情况,测试用   
	//printf("fe:%x\r\n",Dst_Addr);	  
	Dst_Addr*=4096;
	MX25Lxxx_Write_Enable();                  //SET WEL 	 
	MX25Lxxx_Wait_Busy();   
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(W25X_SectorErase);   //发送扇区擦除指令 

	SPI2_ReadWriteByte((u8)((Dst_Addr)>>16));  //发送24bit地址    
	SPI2_ReadWriteByte((u8)((Dst_Addr)>>8));   
	SPI2_ReadWriteByte((u8)Dst_Addr);  
	MX25Lxxx_CS(1);                            //取消片选     	      
	MX25Lxxx_Wait_Busy();   				    //等待擦除完成
}  

void MX25Lxxx_Write_Page(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)
{
	u16 i;  
	MX25Lxxx_Write_Enable();                  //SET WEL 
	MX25Lxxx_CS(0);                            //使能器件   
	SPI2_ReadWriteByte(W25X_PageProgram);   //发送写页命令   

	SPI2_ReadWriteByte((u8)((WriteAddr)>>16)); //发送24bit地址    
	SPI2_ReadWriteByte((u8)((WriteAddr)>>8));   
	SPI2_ReadWriteByte((u8)WriteAddr);   
	for(i=0;i<NumByteToWrite;i++)
	{
		SPI2_ReadWriteByte(pBuffer[i]);//循环写数  
	}
	MX25Lxxx_CS(1);                            //取消片选 
	MX25Lxxx_Wait_Busy();					   //等待写入结束
} 

void MX25Lxxx_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
{ 			 		 
	u16 pageremain;	   
	pageremain=256-WriteAddr%256; //单页剩余的字节数		 	    
	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//不大于256个字节
	while(1)
	{	   
		MX25Lxxx_Write_Page(pBuffer,WriteAddr,pageremain);
		if(NumByteToWrite==pageremain)break;//写入结束了
	 	else //NumByteToWrite>pageremain
		{
			pBuffer+=pageremain;
			WriteAddr+=pageremain;	

			NumByteToWrite-=pageremain;			  //减去已经写入了的字节数
			if(NumByteToWrite>256)pageremain=256; //一次可以写入256个字节
			else pageremain=NumByteToWrite; 	  //不够256个字节了
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
 	secpos=WriteAddr/4096;//扇区地址  
	secoff=WriteAddr%4096;//在扇区内的偏移
	secremain=4096-secoff;//扇区剩余空间大小   
 	//printf("ad:%X,nb:%X\r\n",WriteAddr,NumByteToWrite);//测试用
 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//不大于4096个字节
	while(1) 
	{	
		MX25Lxxx_Read(W25QXX_BUF,secpos*4096,4096);//读出整个扇区的内容
		for(i=0;i<secremain;i++)//校验数据
		{
			if(W25QXX_BUF[secoff+i]!=0XFF)break;//需要擦除  	  
		}
		if(i<secremain)//需要擦除
		{
			MX25Lxxx_Erase_Sector(secpos);//擦除这个扇区
			for(i=0;i<secremain;i++)	   //复制
			{
				W25QXX_BUF[i+secoff]=pBuffer[i];	  
			}
			MX25Lxxx_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//写入整个扇区  
		}
		else 
		{
			MX25Lxxx_Write_NoCheck(pBuffer,WriteAddr,secremain);//写已经擦除了的,直接写入扇区剩余区间. 				   
		}
		if(NumByteToWrite==secremain)
		{
			break;//写入结束了
		}
		else//写入未结束
		{
			secpos++;//扇区地址增1
			secoff=0;//偏移位置为0 	 

		   	pBuffer+=secremain;  //指针偏移
			WriteAddr+=secremain;//写地址偏移	   
		   	NumByteToWrite-=secremain;				//字节数递减
			if(NumByteToWrite>4096)
			{
				secremain=4096;	//下一个扇区还是写不完
			}
			else 
			{
				secremain=NumByteToWrite;			//下一个扇区可以写完了
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

