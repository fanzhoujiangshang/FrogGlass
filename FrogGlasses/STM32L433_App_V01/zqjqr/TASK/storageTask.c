#include <stdbool.h>
#include <string.h>
#include "math.h"
#include "storageTask.h"
#include "w25qxx.h"
#include "rtc.h"
#include "bat_adc.h"
#include "uart_si446.h"
#include "sensorsTask.h"
//#include "mpuTask.h"
#include "displaytask.h"
#include "malloc.h"	  
#include <stdio.h>

#include "FreeRTOS.h"
#include "task.h"
#include "print.h"



/********************************************************************************	 
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C)
 * All rights reserved
********************************************************************************/
void ReadRecordGetIndexInfo(void);
void DiveRecordAddNewPoint(void);
u8 CalculateTimeSpan(u8 SaveAnyway);
u32 MX25Lxxx_ConfigCheckSum(void);
#define DEPTH_CONVERT(x)		((x - 9.86f - 0.14f) * 10)
configParameters m_ConfigParams;
u8 m_ConfigParamsChanged = false;
static u8 m_LoopCnt = 0;
u8 g_RecordOffsetIndex = 0xff;
Dive_Head g_DiveDataHead;
Position_Point g_DiveDataPoint;
RecIndex_Info g_RecordIndexInfo;
void configParamTask(void* param)
{
	uint16_t tmpU16;
	uint8_t tmpU8,tmpU8_2;
	u8 g_nDiveMode_Old;
	
	MX25Lxxx_ReadConfig();		// zhengl add 20190527
	ReadRecordGetIndexInfo();
	while(1)
	{
		vTaskDelay(200);	
		m_LoopCnt++;
		if(m_LoopCnt == 0)
		{
			MX25Lxxx_WriteConfig();		// zhengl add 20190527
		}
		//--------------------------------------------------------------date time		
		/* Get the RTC current Time ,must get time first*/
	       HAL_RTC_GetTime(&RTC_Handler, &stimestructure, RTC_FORMAT_BIN);
		/* Get the RTC current Date */
		HAL_RTC_GetDate(&RTC_Handler, &sdatestructure, RTC_FORMAT_BIN);

		set_display_data_zebra_time(stimestructure);
		set_display_data_zebra_date(sdatestructure);
		//--------------------------------------------------------------dive info update
		if(g_nCurrentDiveMode != DIVE_MODE_NONE)
		{	
			if(g_nDiveMode_Old != g_nCurrentDiveMode)
			{
				DiveRecordStart();
				DiveRecordAddNewPoint();
			}
			else if(g_RecordOffsetIndex != 0xff)
			{
				if(CalculateTimeSpan(0))
				{
					DiveRecordAddNewPoint();
				}
				if(g_DiveDataHead.m_DiveSpan >= 99 * 60)	// 99 min
				{
					g_nCurrentDiveMode = DIVE_MODE_NONE;
				}
			}
		}
		else
		{
			if(g_nDiveMode_Old != g_nCurrentDiveMode)
			{
				CalculateTimeSpan(1);
				DiveRecordAddNewPoint();
				DiveRecordFinish();
				ReadRecordGetIndexInfo();
				g_DiveDataHead.m_DiveSpan = 0;
			}
		}
		g_nDiveMode_Old = g_nCurrentDiveMode;

		set_display_data_diving_time(g_DiveDataHead.m_DiveSpan);

		set_display_data_temp((u8)sensors.baro.temperature);
		//set_display_data_depth((int)((sensors.baro.depth - 9.86f - 0.14f) * 10));
		set_display_data_depth((int)(DEPTH_CONVERT(sensors.baro.depth)));
		//--------------------------------------------------------------usart1 SI446 170M wireless
		SI446_ReceiveDataHandler();
		if (uartsi446GetOxygen(&tmpU8,&tmpU8_2))
		{
			set_display_data_no_oxygen_State(tmpU8,tmpU8_2);	
		}
		//--------------------------------------------------------------battery charge
		tmpU16  = Get_Adc_AverageNew(ADC_CHANNEL_6);
		set_display_data_no_bat_state( tmpU16);
	}
//	myfree(SRAMIN,diveRecord);
}
//==============================================
//	new Record save data functions begin
//==============================================
void ReadRecordGetIndexInfo(void)
{
	uint8_t tmpBuf[10];
	uint8_t tmpHaveNewData = 0;
	for(int i = 0;i < 10;i++)
	{
		W25QXX_Read((u8 *)tmpBuf, MX25L_DIVE_LIST_INDICATOR_ADDR_START + i * 10, 10);
		for(int j = 0;j < 10;j++)
		{
			switch(tmpBuf[j])
			{
				default:
				case 0:
					g_RecordIndexInfo.m_TotalRecordNum = i * 10 + j;
					if(g_RecordIndexInfo.m_TotalRecordNum > 0)
					{
						g_RecordIndexInfo.m_NewestRecordOffset = g_RecordIndexInfo.m_TotalRecordNum - 1;
					}
					else
					{
						g_RecordIndexInfo.m_NewestRecordOffset = 0;
					}
					return;
				case 1:
					if(tmpHaveNewData == 0)
					{
						g_RecordIndexInfo.m_TotalRecordNum = i * 10 + j + 1;
						g_RecordIndexInfo.m_NewestRecordOffset = i * 10 + j;
					}
					break;
				case 2:
					tmpHaveNewData = 1;
					g_RecordIndexInfo.m_TotalRecordNum = MX25L_DIVE_LIST_RECORD_COUNT_MAX;
					g_RecordIndexInfo.m_NewestRecordOffset = i * 10 + j;
					break;
			}
		}
	}
}
void ReadRecordGetHeadInfo(u8 Offset,Dive_Head * pHead)
{
	u32 tmpReadAddr;

	tmpReadAddr = MX25L_DIVE_LIST_HEAD_ADDR_START;
	tmpReadAddr += sizeof(Dive_Head) * Offset;
	W25QXX_Read((u8 *)pHead, tmpReadAddr, 10);	
}
void ReadRecordGetPointInfo(u8 RecIndex,u8 PointIndex,Position_Point * pPoint)
{
	u32 tmpOffset;

	tmpOffset = MX25L_DIVE_LIST_DATA_N_ADDR_START(RecIndex);
	tmpOffset += sizeof(g_DiveDataPoint) * PointIndex;
	W25QXX_Read((u8 *)(pPoint), tmpOffset, sizeof(g_DiveDataPoint));
}
u8 DiveRecordGetIndex(void)
{
	uint8_t tmpBuf[10];
	uint8_t tmpHaveNewData = 0;
	for(int i = 0;i < 10;i++)
	{
		W25QXX_Read((u8 *)tmpBuf, MX25L_DIVE_LIST_INDICATOR_ADDR_START + i * 10, 10);
		for(int j = 0;j < 10;j++)
		{
			switch(tmpBuf[j])
			{
				default:
				case 0:
					tmpBuf[j] = 1;
					W25QXX_Write((const u8 *)tmpBuf, MX25L_DIVE_LIST_INDICATOR_ADDR_START + i * 10, 10);
					return i * 10 + j;
				case 1:
					if(tmpHaveNewData)
					{
						tmpBuf[j] = 1;
						W25QXX_Write((const u8 *)tmpBuf, MX25L_DIVE_LIST_INDICATOR_ADDR_START + i * 10, 10);
						return i * 10 + j;
					}
					break;
				case 2:
					tmpHaveNewData = 1;
					break;
			}
		}
	}
	//	all flag are 2
	if(tmpHaveNewData)
	{
		memset(tmpBuf,1,10);
		for(int i = 9;i > 0;i++)
		{
			W25QXX_Write((const u8 *)tmpBuf, MX25L_DIVE_LIST_INDICATOR_ADDR_START + i * 10, 10);
		}	
		tmpBuf[0] = 2;
		W25QXX_Write((const u8 *)tmpBuf, MX25L_DIVE_LIST_INDICATOR_ADDR_START, 10);
	}
	else
	{
		memset(tmpBuf,1,10);
		tmpBuf[0] = 2;
		W25QXX_Write((const u8 *)tmpBuf, MX25L_DIVE_LIST_INDICATOR_ADDR_START, 10);
	}
	return 0;	
}
void DiveRecordSaveHead(u8 index)
{
	u32 tmpOffset;

	tmpOffset = MX25L_DIVE_LIST_HEAD_ADDR_START;
	tmpOffset += sizeof(g_DiveDataHead) * index;
	W25QXX_Write((const u8 *)(&g_DiveDataHead), tmpOffset, sizeof(g_DiveDataHead));
}
void DiveRecordStart(void)
{
	/* Get the RTC current Time ,must get time first*/
       HAL_RTC_GetTime(&RTC_Handler, &stimestructure, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
	HAL_RTC_GetDate(&RTC_Handler, &sdatestructure, RTC_FORMAT_BIN);	
	g_DiveDataHead.m_DiveDate.year = sdatestructure.Year;
	g_DiveDataHead.m_DiveDate.month = sdatestructure.Month;
	g_DiveDataHead.m_DiveDate.date = sdatestructure.Date;
	g_DiveDataHead.m_DiveTime.hr = stimestructure.Hours;
	g_DiveDataHead.m_DiveTime.min = stimestructure.Minutes;
	g_DiveDataHead.m_DiveTime.sec = stimestructure.Seconds;
	g_DiveDataHead.m_DiveSpan = 0;
	g_DiveDataHead.m_PointNum = 0;
	g_DiveDataHead.m_DepthMax = 0;
	g_DiveDataHead.m_DiveMode = g_nCurrentDiveMode;
	g_DiveDataHead.m_Temperature = (u16)sensors.baro.temperature;

	g_RecordOffsetIndex = DiveRecordGetIndex();
	DiveRecordSaveHead(g_RecordOffsetIndex);
	g_DiveDataPoint.m_SecondsSpan = 0;
	DiveRecordAddNewPoint();
}
void DiveRecordFinish(void)
{
	DiveRecordSaveHead(g_RecordOffsetIndex);
	g_RecordOffsetIndex = 0xff;
	g_DiveDataHead.m_DiveSpan = 0;
}
void DiveRecordSaveNewPoint(u8 RecIndex,u16 SampleIndex)
{
	u32 tmpOffset;

	tmpOffset = MX25L_DIVE_LIST_DATA_N_ADDR_START(RecIndex);
	tmpOffset += sizeof(g_DiveDataPoint) * SampleIndex;
	W25QXX_Write((const u8 *)(&g_DiveDataPoint), tmpOffset, sizeof(g_DiveDataPoint));
}
void DiveRecordAddNewPoint(void)
{
	if(g_DiveDataHead.m_PointNum > MX25L_DIVE_DATA_MAX_COUNT_PER_RECORD)
	{
		return;
	}
	//g_DiveDataPoint;
	g_DiveDataPoint.m_Temperature = (u16)sensors.baro.temperature;
	g_DiveDataPoint.m_Depth = (u16)(DEPTH_CONVERT(sensors.baro.depth));
	if(g_DiveDataHead.m_DepthMax < g_DiveDataPoint.m_Depth)
	{
		g_DiveDataHead.m_DepthMax = g_DiveDataPoint.m_Depth;
	}
	g_DiveDataPoint.m_Latitude = 0;
	g_DiveDataPoint.m_Longitude = 0;
	GPS_getNewPostion(&(g_DiveDataPoint.m_Latitude),&(g_DiveDataPoint.m_Longitude));
	DiveRecordSaveNewPoint(g_RecordOffsetIndex,g_DiveDataHead.m_PointNum);
	g_DiveDataHead.m_PointNum++;
}


u8 CalculateTimeSpan(u8 SaveAnyway)
{
	u16 tmpCurSeconds,tmpStartSeconds,tmpSpanSeconds;

	tmpCurSeconds = stimestructure.Seconds + stimestructure.Minutes * 60;
	tmpStartSeconds = g_DiveDataHead.m_DiveTime.sec;
	tmpStartSeconds += g_DiveDataHead.m_DiveTime.min * 60;
	while(tmpCurSeconds < tmpStartSeconds + g_DiveDataPoint.m_SecondsSpan)
	{
		tmpCurSeconds += 60 * 60;
	}
	//------------------------------------update dive total seconds
	tmpSpanSeconds = tmpCurSeconds - tmpStartSeconds;
	
	if((tmpSpanSeconds - g_DiveDataPoint.m_SecondsSpan > 1)||
		(SaveAnyway))
	{
		g_DiveDataPoint.m_SecondsSpan = tmpSpanSeconds;
		g_DiveDataHead.m_DiveSpan = tmpSpanSeconds;
		return 1;
	}
	return 0;
}
void DiveRecordStopFromOutside(void)
{
	if(g_nCurrentDiveMode != DIVE_MODE_NONE)
	{
		g_nCurrentDiveMode = DIVE_MODE_NONE;
		while(g_DiveDataHead.m_DiveSpan > 0)
		{
			vTaskDelay(50);
		}
	}
}
//==============================================
//	new Record save data functions end
//==============================================
void MX25Lxxx_Test(void)
{
	u8 tmpTest[8] = {0x31,0x32,0x33,0x34,0x36,0x37,0x38,0x39};
	u8 tmpRead[8];

	W25QXX_Write((u8 *)tmpTest, MX25L_APPLICATION_A_ADDR, 8);	
	W25QXX_Read((u8 *)tmpRead, MX25L_APPLICATION_A_ADDR, 8);	
	printf("%02x-%02x-%02x-%02x-",tmpRead[0],tmpRead[1],tmpRead[2],tmpRead[3]);
	printf("%02x-%02x-%02x-%02x-\r\n",tmpRead[4],tmpRead[5],tmpRead[6],tmpRead[7]);	
}

void MX25Lxxx_FastWriteData(uint32_t writeOffset,uint8_t* dataPtr,uint8_t dataLen)
{
	u32 tmpWrAddr;
	
	tmpWrAddr = MX25L_APPLICATION_B_ADDR + MX25L_APP_DATA_START_OFFSET + writeOffset;
	W25QXX_Write(dataPtr,tmpWrAddr,dataLen);
}
void MX25Lxxx_FastWriteSize(uint32_t writeDataSize)
{
	uint32_t tmpSize;

	tmpSize = writeDataSize;
	W25QXX_Write((u8*)(&tmpSize),MX25L_APPLICATION_B_ADDR, sizeof(uint32_t));	
	m_ConfigParams.m_FWUpdatedFlag = 1;
	m_ConfigParams.m_FW_Entry = FIRMWARE_B;
	m_ConfigParamsChanged = true;
	MX25Lxxx_WriteConfig();
}
void MX25Lxxx_WriteApp(BT_DataSave* PparaData,u8 IsLastPkg)
{
	u32 tmpWrAddr;
	
	tmpWrAddr = MX25L_APPLICATION_A_ADDR + MX25L_APP_DATA_START_OFFSET + PparaData->m_DataStoreOffset;
	W25QXX_Write(PparaData->m_DataPtr,tmpWrAddr, PparaData->m_DataLen);	
	if((PparaData->m_DataStoreOffset == 4)&&(PparaData->m_DataLen))	// before store data,write 0 to Data Size area
	{
		W25QXX_Write((u8*)(&(PparaData->m_DataStoreOffset)),MX25L_APPLICATION_A_ADDR, sizeof(uint32_t));
	}
	PparaData->m_DataStoreOffset += PparaData->m_DataLen;
	PparaData->m_FlowNum++;
	if(IsLastPkg)														// at the end of store data,write data size to Data Size area
	{
		W25QXX_Write((u8*)(&(PparaData->m_DataStoreOffset)),MX25L_APPLICATION_A_ADDR, sizeof(uint32_t));	
		m_ConfigParams.m_FWUpdatedFlag = 1;
		m_ConfigParams.m_FW_Entry = FIRMWARE_A;
		m_ConfigParamsChanged = true;
		MX25Lxxx_WriteConfig();
	}
}
void MX25Lxxx_ReadConfig(void)
{
	u8 * tmpRead;
	u32 readOutChkSum;

	tmpRead = (u8*)(&m_ConfigParams);
	W25QXX_Read((u8 *)tmpRead, MX25L_CONFIG_SAVED_ADDR, sizeof(m_ConfigParams));
	readOutChkSum = MX25Lxxx_ConfigCheckSum();
	if(m_ConfigParams.m_checkSum != readOutChkSum)
	{
		printf("chk sum X!\r\n");
		MX25Lxxx_WriteConfigDefault();
	}
	else if(m_ConfigParams.m_FWUpdatedFlag)
	{
		m_ConfigParams.m_FWUpdatedFlag = 0;
		m_ConfigParamsChanged = true;
		MX25Lxxx_WriteConfig();		
		printf("FW update successful!\r\n");
	}
}
void MX25Lxxx_WriteConfig(void)
{
	u8 * tmpRead;
	if(m_ConfigParamsChanged)
	{
		m_ConfigParams.m_checkSum = MX25Lxxx_ConfigCheckSum();
		tmpRead = (u8*)(&m_ConfigParams);
		W25QXX_Write((u8 *)tmpRead, MX25L_CONFIG_SAVED_ADDR, sizeof(m_ConfigParams));	
		m_ConfigParamsChanged = false;
		printf("\r\n-------------save----------------\r\n");
	}
}
void MX25Lxxx_ReadMagnet(int16_t* minSave,int16_t* maxSave)
{
	maxSave[0] = m_ConfigParams.m_Mag_Max[0];
	maxSave[1] = m_ConfigParams.m_Mag_Max[1];
	maxSave[2] = m_ConfigParams.m_Mag_Max[2];
	minSave[0] = m_ConfigParams.m_Mag_Min[0];
	minSave[1] = m_ConfigParams.m_Mag_Min[1];
	minSave[2] = m_ConfigParams.m_Mag_Min[2];
}
void MX25Lxxx_WriteMagnet(int16_t* minSave,int16_t* maxSave)
{
	m_ConfigParamsChanged = true;
	m_ConfigParams.m_Mag_Max[0] = maxSave[0];
	m_ConfigParams.m_Mag_Max[1] = maxSave[1];
	m_ConfigParams.m_Mag_Max[2] = maxSave[2];
	m_ConfigParams.m_Mag_Min[0] = minSave[0];
	m_ConfigParams.m_Mag_Min[1] = minSave[1];
	m_ConfigParams.m_Mag_Min[2] = minSave[2];	
}
void MX25Lxxx_WriteMagnet_2(void)
{
	m_ConfigParamsChanged = true;	
}
void MX25Lxxx_ReadGyro(int16_t* gyroSave)
{
	gyroSave[0] = m_ConfigParams.m_Gyro_Bias[0];
	gyroSave[1] = m_ConfigParams.m_Gyro_Bias[1];
	gyroSave[2] = m_ConfigParams.m_Gyro_Bias[2];
}
void MX25Lxxx_WriteGyro(int16_t* gyroSave)
{
	m_ConfigParamsChanged = true;
	m_ConfigParams.m_Gyro_Bias[0] = gyroSave[0];
	m_ConfigParams.m_Gyro_Bias[1] = gyroSave[1];
	m_ConfigParams.m_Gyro_Bias[2] = gyroSave[2];	
}
void MX25Lxxx_ReadLanguage(uint8_t* paraLang)
{
	switch(m_ConfigParams.m_Language)
	{
		case LANGUAGE_CHI:
			*paraLang = LANGUAGE_CHI;
			break;
		case LANGUAGE_ENG:
		default:
			*paraLang = LANGUAGE_ENG;
			if(m_ConfigParams.m_Language != LANGUAGE_ENG)
			{
				m_ConfigParams.m_Language = LANGUAGE_ENG;
				m_ConfigParamsChanged = true;
			}
			break;
	}
}
void MX25Lxxx_WriteLanguage(uint8_t paraLang)
{
	m_ConfigParamsChanged = true;
	switch(paraLang)
	{
		case LANGUAGE_CHI:
		case LANGUAGE_ENG:
			if(m_ConfigParams.m_Language != paraLang)
			{
				m_ConfigParams.m_Language = paraLang;
				m_ConfigParamsChanged = true;
			}
			break;
		default:
			break;
	}
}
void MX25Lxxx_ReadOxygenAddr(uint16_t* pAddrSave)
{
	*pAddrSave = m_ConfigParams.m_OxygenAddr;
}
void MX25Lxxx_WriteOxygenAddr(uint16_t AddrSave)
{
	m_ConfigParamsChanged = true;
	m_ConfigParams.m_OxygenAddr = AddrSave;
}
void MX25Lxxx_WriteConfigDefault(void)
{
	m_ConfigParamsChanged 		 = true;
	m_ConfigParams.m_Mag_Max[0] = -32767;
	m_ConfigParams.m_Mag_Max[1] = -32767;
	m_ConfigParams.m_Mag_Max[2] = -32767;

	m_ConfigParams.m_Mag_Min[0] =  32767;
	m_ConfigParams.m_Mag_Min[1] =  32767;
	m_ConfigParams.m_Mag_Min[2] =  32767;
	m_ConfigParams.m_FW_Entry	=  FIRMWARE_A;
	m_ConfigParams.m_Language	=  LANGUAGE_ENG;
	MX25Lxxx_WriteConfig();
}
u32 MX25Lxxx_ConfigCheckSum(void)
{
	u8 * tmpptr;
	u32 retValue = 0;
	
	tmpptr = (u8*)(&m_ConfigParams);
	for(int i = 0;i < sizeof(m_ConfigParams) - sizeof(u32);i++)
	{
		retValue += tmpptr[i];
	}
	printf("ret:%02x\r\n",retValue);
	return retValue;
}

