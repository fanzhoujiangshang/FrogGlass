/**
  ******************************************************************************
  * @file    STM32F4xx_IAP/inc/flash_if.h 
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    10-October-2011
  * @brief   This file provides all the headers of the flash_if functions.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FLASH_IF_H
#define __FLASH_IF_H

/* Includes ------------------------------------------------------------------*/
//#include "stm32f4xx.h"
#include "stm32l4xx_hal.h"
#include "app\action.h"
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Define the address from where user application will be loaded.
   Note: the 1st sector 0x08000000-0x08003FFF is reserved for the IAP code */
#define CHIP_PAGE_SIZE   											(0x800)    
#define DOUBLEWORD_SIZE   											(8)   
#define DATA_PACKAGE_SIZE   										(16)//(128)   
#define DATA_BUFFER_SIZE  	 										(256)
//#define APPLICATION_ADDRESS   (uint32_t)0x08004000 
enum
{
	FIRMWARE_A,
	FIRMWARE_B,
	FIRMWARE_MAX
};

#define	MX25L_APPLICATION_A_ADDR				(1024 * 1024 * 14)					// 14M	4K-bytes sector
#define	MX25L_APPLICATION_A_SIZE					(256 * 1024)						// 256k		256 * 1024

#define	MX25L_APPLICATION_B_ADDR				(MX25L_APPLICATION_A_ADDR + MX25L_APPLICATION_A_SIZE)		// 4K-bytes sector
#define	MX25L_APPLICATION_B_SIZE					(MX25L_APPLICATION_A_SIZE)	
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
void Flash_If_Begin(void);
void Flash_If_End(void);
uint8_t Flash_If_Erase(uint32_t Add);
uint8_t Flash_If_Write(uint8_t *src, uint32_t dest_addr, uint32_t Len);
uint8_t Flash_If_Read(uint8_t* buff, uint32_t dest_addr, uint32_t Len);
uint8_t User_IAP_Handler(uint8_t index,uint8_t* pData,uint8_t ByteNum);
void writeFlashTest(void);
void printFlashTest(void);
uint8_t User_IAP_Handler(uint8_t index,uint8_t* pData,uint8_t ByteNum);
uint8_t User_IAP_Writer(uint32_t *WriteOffset,uint8_t* pData,uint16_t ByteNum);
void User_IAP_Loop(void);
#endif  /* __FLASH_IF_H */

/*******************(C)COPYRIGHT 2011 STMicroelectronics *****END OF FILE******/
