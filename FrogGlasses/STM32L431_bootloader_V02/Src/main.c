/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "app\lcd_driver.h"
#include "app\action.h"
#include "app\flash_if.h"
#include "app\mx25Lxxx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
uint8_t M_DataPkg0[16] = {0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
						0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f
					};
uint8_t M_DataPkg1[16] = {0x10,0x11,0x12,0x13,0x14,0x15,0x16,0x17,
						0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f
					};
uint8_t M_DataPkg2[16] = {0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,
						0x28,0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f
					};
uint8_t M_DataPkg3[16] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,
						0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f
					};
uint8_t M_DataPkg4[16] = {0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,
						0x48,0x49,0x4a,0x4b,0x4c,0x4d,0x4e,0x4f
					};


#define	LANGUAGE_CH	(1)	// 1: 中文
#define	LANGUAGE_EN	(0)	// 0: english
const unsigned char* logoCh[]	={"中潜股份"};
const unsigned char* logoEn[]	={"CHINA DIVE"};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define VECT_TABLE_OFFSET							0x0000000		// 256 - 32 KB
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t _BootupParameter_ __attribute__((at(0x2000FF80)));
u32 Sec_Counter;
u16 Key_A_State;
u16 Key_B_State;
u16 Key_C_State;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Power_Standby(void)
{
/*	if(LL_PWR_IsActiveFlag_SB(void))
	{
		LL_PWR_ClearFlag_SB(void);
	}*/	
	if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) != RESET){
		__HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);
	}
	__HAL_RCC_PWR_CLK_ENABLE();	
	HAL_PWR_EnableWakeUpPin(PWR_WAKEUP_PIN2);
	/* Clear all related wakeup flags*/
	__HAL_PWR_CLEAR_FLAG(PWR_FLAG_WU);
	HAL_PWR_EnterSTANDBYMode();	
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */
	SCB->VTOR = FLASH_BASE | VECT_TABLE_OFFSET;
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_SPI2_Init();
  /* USER CODE BEGIN 2 */
  User_SPI2_Init();  
  User_USART_EnableINT();
  User_GPIO_Init_Selfdefine();  
  LL_mDelay(100);		// 500
  LCD_Init();
  LCD_Clear(0x0000);	  
  SelfDefineValueInit();
  Sec_Counter = 0;	
  Key_A_State = 0;
  Key_B_State = 0;
  Key_C_State = 0;
  //MX25Lxxx_Test();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
	//BT_USART3_SendData("TTM:TST-",8);
  	//BT_USART3_SendData("TTM:ADV-ON",10);
	//BT_USART3_SendData("TTM:REN-ZHL",11);
	//BT_USART3_SendData("TTM:ADC-LIANG",13);
/*	
	User_IAP_Handler(0,M_DataPkg0,16);
	User_IAP_Handler(1,M_DataPkg1,16);
	User_IAP_Handler(2,M_DataPkg2,16);
	User_IAP_Handler(3,M_DataPkg3,16);
	User_IAP_Handler(4,M_DataPkg4,14);    
*/	
	
	int i;
	for(i = 0; i < 3; i++)
	{
            /* disable interrupts. */
            NVIC->ICER[i] = 0xFFFFFFFF;
            /* clean up interrupts flags. */
            NVIC->ICPR[i] = 0xFFFFFFFF;
	}
 	__enable_irq(); 	
	User_IAP_Loop();
	switch(_BootupParameter_)
	{
		default:
		case TO_BOOTLOADER_DEFAULT:
			_BootupParameter_ = TO_BOOTLOADER_DEFAULT;
			break;
		case TO_BOOTLOADER_STANDBY:
			_BootupParameter_ = TO_BOOTLOADER_DEFAULT;
			while(IS_KEY_A_RELEASE() == 0)
			{
				LL_mDelay(200);
			}
			User_GPIO_ReadyForStandby();
			Power_Standby();
			break;
	}
	LCD_Show_Image(70, 38, 40,25, gImage_logo);
	BACK_COLOR = BLACK;	
	POINT_COLOR = WHITE;	

	//printf("LANGUAGE TAG : %d \r\n",m_ConfigParams.m_Language);

	if(LANGUAGE_CH ==m_ConfigParams.m_Language)
	{
		Show_Str(57, 70, 80, 16, logoCh[0],16); //中文
	}
	else if(LANGUAGE_EN==m_ConfigParams.m_Language)
	{
		Show_Str(48, 70, 80, 16, logoEn[0],16); //英文
	}
	else
	{
		Show_Str(48, 70, 80, 16, logoEn[0],16); //英文
	}	
	printf("\r\n");    
	printf("Bootloader v01\r\n");
	printf("BParm:%02x\r\n",_BootupParameter_);
	printf("*********************************\r\n");  
	printf("Press A: Enter Standby\r\n");   
	printf("Press B: From Section A read FW and update\r\n");   
	printf("Press C: From Section B read FW and update\r\n");     
	printf("*********************************\r\n");  
  while (1)
  {
  	if(Get_TM_Flag() & TM_EVENT_100MS)
  	{
  		Clr_TM_Flag(TM_EVENT_100MS);
		//printf("timer event!\r\n");
		UpdateKeyStatus();
	}
  	if(Get_TM_Flag() & TM_EVENT_200MS)
  	{
  		Clr_TM_Flag(TM_EVENT_200MS);
		//printf("timer event!\r\n");
		//BT_Handler();
	}
  	if(Get_TM_Flag() & TM_EVENT_1SECOND)
  	{
  		Clr_TM_Flag(TM_EVENT_1SECOND);
		//printf("timer event!\r\n");
		if(GetKeyStatus(KEY_ENUM_A))				//		standby
		{
			printf("Key A Long Pressed\r\n");
			_BootupParameter_ = TO_BOOTLOADER_STANDBY;
		}
		if(GetKeyStatus(KEY_ENUM_B))				//		update FW from A source
		{
			printf("Key B Long Pressed\r\n");
			_BootupParameter_ = TO_BOOTLOADER_UPDATE_A;
		}
		if(GetKeyStatus(KEY_ENUM_C))				//		update FW from B source
		{
			printf("Key C Long Pressed\r\n");
			_BootupParameter_ = TO_BOOTLOADER_UPDATE_B;
		}		
	
		if(Sec_Counter < BOOTLOADER_DELAY_SECS)
		{		
			Sec_Counter+= 1;
			printf("%d,",Sec_Counter);	
		}
		else
		{
			printf("para__%d___\r\n",_BootupParameter_);
			switch(_BootupParameter_)
			{
				default:
				case TO_BOOTLOADER_DEFAULT:
					LL_mDelay(500);
					Jump_Main_App2();
					break;
				case TO_BOOTLOADER_STANDBY:
					while(IS_KEY_A_RELEASE() == 0)
					{
						LL_mDelay(200);
					}			
					User_GPIO_ReadyForStandby();
					Power_Standby();
					break;
			}
		}
	}	
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_4);

  if(LL_FLASH_GetLatency() != LL_FLASH_LATENCY_4)
  {
  Error_Handler();  
  }
  LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
  LL_RCC_HSI_Enable();

   /* Wait till HSI is ready */
  while(LL_RCC_HSI_IsReady() != 1)
  {
    
  }
  LL_RCC_HSI_SetCalibTrimming(16);
  LL_RCC_MSI_Enable();

   /* Wait till MSI is ready */
  while(LL_RCC_MSI_IsReady() != 1)
  {
    
  }
  LL_RCC_MSI_EnableRangeSelection();
  LL_RCC_MSI_SetRange(LL_RCC_MSIRANGE_6);
  LL_RCC_MSI_SetCalibTrimming(0);
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_MSI, LL_RCC_PLLM_DIV_1, 40, LL_RCC_PLLR_DIV_2);
  LL_RCC_PLL_EnableDomain_SYS();
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {
    
  }
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {
  
  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_Init1msTick(80000000);
  LL_SYSTICK_SetClkSource(LL_SYSTICK_CLKSOURCE_HCLK);
  LL_SetSystemCoreClock(80000000);
  LL_RCC_SetUSARTClockSource(LL_RCC_USART1_CLKSOURCE_HSI);
  LL_RCC_SetUSARTClockSource(LL_RCC_USART2_CLKSOURCE_HSI);
  LL_RCC_SetUSARTClockSource(LL_RCC_USART3_CLKSOURCE_PCLK1);
}

/* USER CODE BEGIN 4 */
void UpdateKeyStatus(void)
{
	Key_C_State <<= 1;
	if(IS_KEY_C_RELEASE() == 0)
	{
		Key_C_State |= 1;
	}
	Key_B_State <<= 1;
	if(IS_KEY_B_RELEASE() == 0)
	{
		Key_B_State |= 1;
	}
	Key_A_State <<= 1;
	if(IS_KEY_A_RELEASE() == 0)
	{
		Key_A_State |= 1;
	}	
}
u8 GetKeyStatus(u8 index)
{
	u8 retStatus = 0;
	u16 tmpValue;
	switch(index)
	{
		case KEY_ENUM_A:		// key A
			tmpValue = Key_A_State;
			tmpValue++;
			if(tmpValue == 0x0)
			{
				retStatus = 1;
			}
			break;
		case KEY_ENUM_B:		// key B
			tmpValue = Key_B_State;
			tmpValue++;
			if(tmpValue == 0x0)
			{
				retStatus = 1;
			}	
			break;
		case KEY_ENUM_C:		// key C
			tmpValue = Key_C_State;
			tmpValue++;
			if(tmpValue == 0x0)
			{
				retStatus = 1;
			}	
			break;
		default:
			break;
	}
	return retStatus;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
