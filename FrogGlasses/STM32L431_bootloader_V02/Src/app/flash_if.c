/**
  ******************************************************************************
  * @file    STM32F4xx_IAP/src/flash_if.c 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    10-October-2011
  * @brief   This file provides all the memory related operation functions.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */ 

/** @addtogroup STM32F4xx_IAP
  * @{
  */

/* Includes ------------------------------------------------------------------*/
#include "app\flash_if.h"
#include "stm32l4xx_hal_flash.h"
#include "app\mx25Lxxx.h"
#include "app\lcd_driver.h"
#include "usart.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint32_t m_FlashOffset = 0;
uint8_t writeFlashData[DOUBLEWORD_SIZE];
uint32_t writeFlashAddress = 0x8020000;
uint8_t writeFlashIndex = 0;
uint8_t dataBuffer[DATA_BUFFER_SIZE];
/* Private function prototypes -----------------------------------------------*/
static uint32_t GetPage(uint32_t Addr);
static uint32_t GetBank(uint32_t Addr);



void User_IAP_Loop(void)
{
	u32 tmpReadAddr;
	u32 tmpWriteAddr;
	u32 tmpDataLen;
	u32 tmpReadOffset;
	u16 tmpPercent;
	u16 tmpPercent_Old = 0xFF;	
	uint8_t tmpExFlashSection;
	if(Check_FW_Update_Flag(&tmpExFlashSection) == 0)
	{
		return;
	}
	switch(tmpExFlashSection)
	{
		default:
		case FIRMWARE_A:
			tmpReadAddr = MX25L_APPLICATION_A_ADDR;
			break;
		case FIRMWARE_B:
			tmpReadAddr = MX25L_APPLICATION_B_ADDR;
			break;
	}
#if 1
	MX25Lxxx_Read((u8*)(&tmpDataLen),tmpReadAddr,sizeof(u32));
	if(tmpDataLen > 0x37FF0)
	{
		tmpDataLen = 0x37FF0;
	}
#else
	tmpDataLen = 0x37FF0;
#endif
	tmpReadOffset = 0;
	tmpWriteAddr = 0;
	while(tmpReadOffset < tmpDataLen)
	{
		if(tmpDataLen - tmpReadOffset > DATA_BUFFER_SIZE)
		{
			MX25Lxxx_Read(dataBuffer,tmpReadAddr + tmpReadOffset + 4,DATA_BUFFER_SIZE);
			User_IAP_Writer(&tmpWriteAddr,dataBuffer,DATA_BUFFER_SIZE);
			tmpReadOffset += DATA_BUFFER_SIZE;
		}
		else
		{
			MX25Lxxx_Read(dataBuffer,tmpReadAddr + tmpReadOffset + 4,tmpDataLen - tmpReadOffset);
			User_IAP_Writer(&tmpWriteAddr,dataBuffer,tmpDataLen - tmpReadOffset);
			tmpReadOffset = tmpDataLen;
		}
		if(tmpReadOffset == tmpDataLen)
		{
			User_IAP_Writer(&tmpWriteAddr,dataBuffer,0);
			break;
		}
		tmpPercent = (tmpReadOffset * 100) / tmpDataLen;
		if(tmpPercent_Old != tmpPercent)
		{
			printf("persent:%d\r\n",tmpPercent);
			tmpPercent_Old = tmpPercent;
			LCD_Fill(40 + tmpPercent,	100,				40 + 100,		104,	0xAAAA);	
			LCD_Fill(40,				100,				40 + tmpPercent,	104,	0xFFF0);	
		}
	}
}

uint8_t User_IAP_Writer(uint32_t *WriteOffset,uint8_t* pData,uint16_t ByteNum)
{
	uint32_t tmpOffset;
	if(*WriteOffset == 0)
	{
		Flash_If_Begin();
		writeFlashIndex = 0;
	}

	tmpOffset = 0;
	while(ByteNum >  tmpOffset)
	{
		for(;writeFlashIndex < DOUBLEWORD_SIZE;writeFlashIndex++)
		{
			writeFlashData[writeFlashIndex] = pData[tmpOffset];
			tmpOffset++;
			if(tmpOffset > ByteNum)
			{
				break;
			}
		}
		if(writeFlashIndex == DOUBLEWORD_SIZE)
		{
			if((*WriteOffset % CHIP_PAGE_SIZE) == 0)
			{
				Flash_If_Erase(APPLICATION_ADDRESS + *WriteOffset);
			}
			Flash_If_Write(writeFlashData,APPLICATION_ADDRESS + *WriteOffset,8);
			*WriteOffset += 8;
			writeFlashIndex = 0;
		}
	}
	if(ByteNum < DATA_PACKAGE_SIZE)
	{
		if(writeFlashIndex > 0)
		{
			for(;writeFlashIndex < DOUBLEWORD_SIZE;writeFlashIndex++)
			{
				writeFlashData[writeFlashIndex] = 0x00;
			}
			if((*WriteOffset % CHIP_PAGE_SIZE) == 0)
			{
				Flash_If_Erase(APPLICATION_ADDRESS + *WriteOffset);
			}		
			Flash_If_Write(writeFlashData,APPLICATION_ADDRESS + *WriteOffset,8);
		}
		writeFlashIndex = 0;
		Flash_If_End();
	}
	return 0;
}

uint8_t User_IAP_Handler(uint8_t index,uint8_t* pData,uint8_t ByteNum)
{
	uint32_t tmpOffset;
#if DEBUG_PRINTF_EN	
	printf("DOUBLEWORD_SIZE:%d \r\n",DOUBLEWORD_SIZE);
#endif
	if((index == 0)&&(m_FlashOffset == 0))
	{
		Flash_If_Begin();
		writeFlashIndex = 0;
	}

	tmpOffset = 0;
	while(ByteNum >  tmpOffset)
	{
		for(;writeFlashIndex < DOUBLEWORD_SIZE;writeFlashIndex++)
		{
			writeFlashData[writeFlashIndex] = pData[tmpOffset];
#if DEBUG_PRINTF_EN				
			printf("writeFlashIndex:%d \r\n",writeFlashIndex);
#endif
			tmpOffset++;
			if(tmpOffset > ByteNum)
			{
				break;
			}
		}
		if(writeFlashIndex == DOUBLEWORD_SIZE)
		{
			if((m_FlashOffset % CHIP_PAGE_SIZE) == 0)
			{
				Flash_If_Erase(APPLICATION_ADDRESS + m_FlashOffset);
#if DEBUG_PRINTF_EN	
				printf("erase:%08x-- \r\n",APPLICATION_ADDRESS + m_FlashOffset);
#endif
			}	
#if DEBUG_PRINTF_EN
			printf("write:%08x-- \r\n",APPLICATION_ADDRESS + m_FlashOffset);
#endif
			Flash_If_Write(writeFlashData,APPLICATION_ADDRESS + m_FlashOffset,8);
			m_FlashOffset += 8;
			writeFlashIndex = 0;
		}
	}
	if(ByteNum < DATA_PACKAGE_SIZE)
	{
		if(writeFlashIndex > 0)
		{
			for(;writeFlashIndex < DOUBLEWORD_SIZE;writeFlashIndex++)
			{
				writeFlashData[writeFlashIndex] = 0x00;
#if DEBUG_PRINTF_EN				
				printf("writeFlashIndex2:%d \r\n",writeFlashIndex);
#endif
			}
			if((m_FlashOffset % CHIP_PAGE_SIZE) == 0)
			{
				Flash_If_Erase(APPLICATION_ADDRESS + m_FlashOffset);
#if DEBUG_PRINTF_EN
				printf("erase2:%08x-- \r\n",APPLICATION_ADDRESS + m_FlashOffset);
#endif
			}		
			Flash_If_Write(writeFlashData,APPLICATION_ADDRESS + m_FlashOffset,8);
#if DEBUG_PRINTF_EN
			printf("write2:%08x-- \r\n",APPLICATION_ADDRESS + m_FlashOffset);
#endif
		}
		writeFlashIndex = 0;
		Flash_If_End();
	}
	return 0;
}

/**
  * @brief  Initializes Memory.
  * @param  None
  * @retval 0 if operation is successeful, MAL_FAIL else.
  */
void Flash_If_Begin(void)
{
	/* Unlock the internal flash */
	HAL_FLASH_Unlock();
}
void Flash_If_End(void)
{
	/* Lock the internal flash */
	HAL_FLASH_Lock();
}
uint8_t Flash_If_Erase(uint32_t Add)
{  
	uint32_t PageError = 0;  
	/* Variable contains Flash operation status */  
	HAL_StatusTypeDef status;  
	FLASH_EraseInitTypeDef eraseinitstruct;  
	/* Clear OPTVERR bit set on virgin samples */  
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);   
	/* Get the number of sector to erase from 1st sector*/  
	eraseinitstruct.Banks = GetBank(Add);  
	eraseinitstruct.TypeErase = FLASH_TYPEERASE_PAGES;  
	eraseinitstruct.Page = GetPage(Add);  
	eraseinitstruct.NbPages = 1; 
	status = HAL_FLASHEx_Erase(&eraseinitstruct, &PageError);  
	if (status != HAL_OK)  
	{    
		return 1;  
	}  
	return 0;
}
uint8_t Flash_If_Write(uint8_t *src, uint32_t dest_addr, uint32_t Len)
{  
	uint32_t i = 0;  
	/* Clear OPTVERR bit set on virgin samples */  
	__HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_OPTVERR);  
	for(i = 0; i < Len; i += 8)  
	{    
		/* Device voltage range supposed to be [2.7V to 3.6V], the operation will       be done by byte */    
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, (uint32_t)(dest_addr+i), *(uint64_t*)(src+i)) == HAL_OK)    
		{     
			/* Check the written value */      
			if(*(uint64_t *)(src + i) != *(uint64_t*)(dest_addr+i))      
			{        
				/* Flash content doesn't match SRAM content */        
				return 2;      
			}    
		}    
		else    
		{      
			/* Error occurred while writing data in Flash memory */      
			return 1;    
		}  
	}  
	return 0;
}

uint8_t Flash_If_Read(uint8_t* buff, uint32_t dest_addr, uint32_t Len)
{    
	uint32_t i;    
	for(i = 0; i < Len; i++)
	{            
		buff[i] = *(__IO uint8_t*)(dest_addr + i);    
	}  
	/* Return a valid address to avoid HardFault */  
	return 0;
}
/*
	for STM32L431CC chip page size is 2K (0x800)
*/
static uint32_t GetPage(uint32_t Addr)
{  
	uint32_t page = 0;  
	if (Addr < (FLASH_BASE + FLASH_BANK_SIZE))  
	{    
		/* Bank 1 */    
		page = (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;  
	}  
	else  
	{    
		/* Bank 2 */    
		page = (Addr - (FLASH_BASE + FLASH_BANK_SIZE)) / FLASH_PAGE_SIZE;  
	}  
	return page;
}

static uint32_t GetBank(uint32_t Addr)
{  
	return FLASH_BANK_1;
}

void writeFlashTest(void)
{
	writeFlashData[0] = 0x12;
	writeFlashData[1] = 0x34;
	writeFlashData[2] = 0x56;
	writeFlashData[3] = 0x78;
	writeFlashData[4] = 0x90;
	writeFlashData[5] = 0xab;
	writeFlashData[6] = 0xcd;
	writeFlashData[7] = 0xef;
	Flash_If_Erase(writeFlashAddress);
	Flash_If_Write(writeFlashData,writeFlashAddress,8);
}
void printFlashTest(void)
{
	uint8_t tmpdata;
	for(int i = 0;i < 12;i++)
	{
		tmpdata = *(__IO uint8_t*)(writeFlashAddress + i);
#if DEBUG_PRINTF_EN			
		printf("%02x ",tmpdata);
#endif
	}
#if DEBUG_PRINTF_EN	
	printf(" FLASH_SIZE:%08x  ",FLASH_SIZE);
	printf("\r\n");
#endif
}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
