#include "bat_adc.h"
#include "main.h"
#include "MS5837.h"
#include <stdio.h>

//////////////////////////////////////////////////////////////////////////////////	 
//ADC驱动代码	   
//创建日期:2016/1/13
//版本：V1.0
//版权所有，盗版必究。
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
#define	ADC_BUFFER_SIZE			(8)
ADC_HandleTypeDef ADC1_Handler;//ADC句柄
u16 AdcSampleBuffer[ADC_BUFFER_SIZE];
DMA_HandleTypeDef hdma_adc1;
//初始化ADC
//ch: ADC_channels 
//通道值 0~16取值范围为：ADC_CHANNEL_0~ADC_CHANNEL_16
void BAT_ADC_Init(void)
{ 
	ADC_ChannelConfTypeDef sConfig = {0};
	ADC1_Handler.Instance=ADC1;
	ADC1_Handler.Init.ClockPrescaler=ADC_CLOCK_SYNC_PCLK_DIV4;   //4分频，ADCCLK=PCLK2/4=90/4=22.5MHZ
	ADC1_Handler.Init.Resolution=ADC_RESOLUTION_12B;             //12位模式
	ADC1_Handler.Init.DataAlign=ADC_DATAALIGN_RIGHT;             //右对齐
	ADC1_Handler.Init.ScanConvMode=ENABLE;                      //非扫描模式
	ADC1_Handler.Init.EOCSelection=DISABLE;                      //关闭EOC中断
	ADC1_Handler.Init.ContinuousConvMode=ENABLE;                //关闭连续转换
	ADC1_Handler.Init.NbrOfConversion=1;                         //1个转换在规则序列中 也就是只转换规则序列1 
	ADC1_Handler.Init.DiscontinuousConvMode=DISABLE;             //禁止不连续采样模式
	ADC1_Handler.Init.NbrOfDiscConversion=0;                     //不连续采样通道数为0
	ADC1_Handler.Init.ExternalTrigConv=ADC_SOFTWARE_START;       //软件触发
	ADC1_Handler.Init.ExternalTrigConvEdge=ADC_EXTERNALTRIGCONVEDGE_NONE;//使用软件触发
	ADC1_Handler.Init.DMAContinuousRequests=ENABLE;             //关闭DMA请求
	//初始化 
	if (HAL_ADC_Init(&ADC1_Handler) != HAL_OK)
	{
		Error_Handler();
	}	
	for(int i = 0;i < ADC_BUFFER_SIZE;i++)
	{
		AdcSampleBuffer[i] = 0;
	}
	/** Configure Regular Channel 
	*/
	sConfig.Channel = ADC_CHANNEL_6;
	sConfig.Rank = ADC_REGULAR_RANK_1;
	sConfig.SamplingTime = ADC_SAMPLETIME_2CYCLES_5;
	sConfig.SingleDiff = ADC_SINGLE_ENDED;
	sConfig.OffsetNumber = ADC_OFFSET_NONE;
	sConfig.Offset = 0;
	if (HAL_ADC_ConfigChannel(&ADC1_Handler, &sConfig) != HAL_OK)
	{
		Error_Handler();
	}
	//--------------------------------------------dma
	__HAL_RCC_DMA1_CLK_ENABLE();	
	/* ADC1 DMA Init */
	/* ADC1 Init */
	hdma_adc1.Instance = DMA1_Channel1;
	hdma_adc1.Init.Request = DMA_REQUEST_0;
	hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
	hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;
	hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_HALFWORD;
	hdma_adc1.Init.Mode = DMA_CIRCULAR;	//循环模式  
	hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;

	if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
	{
		Error_Handler();
	}	
	__HAL_LINKDMA(&ADC1_Handler,DMA_Handle,hdma_adc1);

	HAL_ADC_Start_DMA(&ADC1_Handler,(uint32_t*)AdcSampleBuffer,ADC_BUFFER_SIZE);
}

void BAT_ADC_Deinit(void)
{
	HAL_ADC_Stop_DMA(&ADC1_Handler);
}

void BAT_I2C_Init(void)
{
	u8 data;
	BQ24295ReadByte(0x04,&data);
	printf("Q24295 reg04:0x%02x++++\r\n",data);	
	data |= 0x01;
	BQ24295WriteByte(0x04,data);	
}

u16 Get_Adc_AverageNew(u32 ch)
{
	u32 temp_val;

	temp_val=0;
	for(u8 t = 0;t<ADC_BUFFER_SIZE;t++)
	{
		temp_val += AdcSampleBuffer[t];
	}
	return temp_val / ADC_BUFFER_SIZE;
} 


