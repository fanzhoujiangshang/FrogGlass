/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2019 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"
#include "usart.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include "print.h"
#include "gpio_control.h"
#include "lcd.h"
#include "mpu9250.h"
#include "MS5837.h"
#include "w25qxx.h"
#include "storageTask.h"
#include "displaytask.h"
#include "keyTask.h"
#include "FreeRTOS.h"
#include "task.h"
#include "radiolink.h"
#include "sensorsTask.h"
#include "stabilizerTask.h"
#include "rtcTask.h"
#include "uart_si446.h"
#include "bat_adc.h"
#include "global.h"


#define BOOTLOADER_ADDRESS						(0x8000000)		// 64 KB
#define APPLICATION_ADDRESS						(0x8010000)		// 256 - 64 KB
#define VECT_TABLE_OFFSET							(0x0010000)		// 256 - 64 KB
#define NVIC_VectTab_FLASH           					((u32)0x08000000)
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void startTask(void *arg);


TaskHandle_t startTaskHandle;
uint8_t _BootupParameter_ __attribute__((at(0x2000FF80)));

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* MCU Configuration--------------------------------------------------------*/
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock */
  SystemClock_Config();

  /*Initialize all configured peripherals */
  GPIOInit();
#if 0
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, APPLICATION_ADDRESS - BOOTLOADER_ADDRESS);
#else	
	//SCB->VTOR = APPLICATION_ADDRESS; /* Vector Table Relocation in Internal FLASH */
	SCB->VTOR = FLASH_BASE | VECT_TABLE_OFFSET;
#endif  
  __enable_irq();
  Print_USART2_Init();
  printf("BSP init start... \r\n");
  BAT_ADC_Init();
  LCD_Init();
  LCD_Clear(BLACK);		   	//清屏
  BACK_COLOR = BLACK;	
  POINT_COLOR = GRAY;
  keyInit();			/*按键初始化*/ 
  RTC_Init();
  InitGlobalData();
//  ms5837_init();  
//  MPU9250_Init();  
  W25QXX_Init();			//	    //3?¨o???￥W25Q256	
  BT_Init();  
  uartSi446Init();  
  displayInit();
 //MX25Lxxx_Test();
  printf("BSP init sucess!\r\n");
  printf("Mode:%02x\r\n",_BootupParameter_);
//  printf("VTOR: 0x%08x ==\r\n",SCB->VTOR);
  xTaskCreate(startTask, "START_TASK", 300, NULL, 2, &startTaskHandle);	/*创建起始任务*/
  vTaskStartScheduler();	/*开启任务调度*/
  while (1)
  {
	vTaskDelay(500);
  };

}


/*创建任务*/
void startTask(void *arg)
{
	taskENTER_CRITICAL();	/*进入临界区*/

	xTaskCreate(BT_RxTask, "RADIOLINK_RX", 150, NULL, 10, NULL);		/*创建串口连接任务*/
	xTaskCreate(GPS_RecTask, "GPS_RECV", 150, NULL, 3, NULL);			/*GPS receive task*/
	xTaskCreate(configParamTask, "CONFIG_PARAM", 150, NULL, 4, NULL);
	xTaskCreate(keyTask, "BUTTON_SCAN", 100, NULL, 5, NULL);	
	xTaskCreate(displayTask, "DISPLAY", 200, NULL, 5, NULL);	
	xTaskCreate(sensorsTask, "SENSORS", 450, NULL, 4, NULL);			/*创建传感器处理任务*/
	printf("Free heap: %d bytes\n", xPortGetFreeHeapSize());			/*打印剩余堆栈大小*/
	
	vTaskDelete(startTaskHandle);										/*删除开始任务*/
		
	taskEXIT_CRITICAL();	/*退出临界区*/
} 


/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /**Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_HSI;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_HSI;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /**Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

pFunction Jump_To_Application;	
void Jump_Bootloader(u8 paraMode)
{
#if 0
	_BootupParameter_ = 0x01;
	NVIC_SystemReset();
#else
	u32 JumpAddress;
	typedef void (*_func)(void);
	BAT_ADC_Deinit();
	taskENTER_CRITICAL();	/*进入临界区*/
	__disable_irq();
        /* reset systick */
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        /* disable all peripherals clock. */
        RCC->AHB1ENR = (1<<20); /* 20: F4 CCMDAT ARAMEN. */
        RCC->AHB2ENR = 0;
        RCC->AHB3ENR = 0;
        RCC->APB1ENR2 = 0;
        RCC->APB2ENR = 0;
        /* Switch to default cpu clock. */
        RCC->CFGR = 0;
	/* Disable MPU */
	MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;	
    /* disable and clean up all interrupts. */
        int i;
        for(i = 0; i < 3; i++)
        {
            /* disable interrupts. */
            NVIC->ICER[i] = 0xFFFFFFFF;
            /* clean up interrupts flags. */
            NVIC->ICPR[i] = 0xFFFFFFFF;
        }	
	/* Set new vector table pointer */
	SCB->VTOR = BOOTLOADER_ADDRESS;		
    /* reset register values */
    __set_BASEPRI(0);
    __set_FAULTMASK(0);	
	 /* Test if user code is programmed starting from address "BOOTLOADER_ADDRESS" */ 
	if(((*(vu32*)BOOTLOADER_ADDRESS)&0x2FFE0000)==0x20000000)//检查栈顶地址是否合法.
	{
		/* Jump to user application */ 
		JumpAddress = *(__IO uint32_t *)(BOOTLOADER_ADDRESS+4); // Jump to user application
		Jump_To_Application = (pFunction)JumpAddress;	
		/* Initialize user application's Stack Pointer */ 
		__set_MSP(*(__IO uint32_t*) BOOTLOADER_ADDRESS);
		__set_PSP(*(__IO uint32_t*) BOOTLOADER_ADDRESS);
		__set_CONTROL(0);
		/* ensure what we have done could take effect */
		__ISB();		
		__disable_irq();		
		/* set parameters for bootloader to read */
		_BootupParameter_ = paraMode;
		printf("_BootupParameter_:%d\r\n",_BootupParameter_);
		/* never return */
		((_func)(*(uint32_t*)(BOOTLOADER_ADDRESS + 4)))();
		//Jump_To_Application();
	}
#endif
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
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

}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
