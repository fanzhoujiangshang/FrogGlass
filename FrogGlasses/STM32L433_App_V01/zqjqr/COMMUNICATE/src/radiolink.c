#include <string.h>
#include "radiolink.h"
#include "storageTask.h"
#include "uart_link.h"
#include "atkp.h"
#include <stdio.h>
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
#include "main.h"
#include "gpio_control.h"
#include "malloc.h"
#include "print.h"
/********************************************************************************	 
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C) 
 * All rights reserved
********************************************************************************/
#define	BT_RECV_DATA_TIMEOUT_VALUE			(20)
#define	BT_TOGGLE_RECV_BUFFER				(0)
#define	BT_DEV_EXIST_COUNTDOWN				(255)
#define 	BT_TX_BUFFER_SIZE  					(200) /*接收队列个数*/
u8 m_BT_TX_Buffer[BT_TX_BUFFER_SIZE];
//====================================================
//			zhengl
//====================================================
u8 m_BT_WorkState = MTT_IDLE;
u8 m_BT_Countdown;
u8 m_BT_RecvData_CD;	// only for receive data timeout check
u8 m_BT_Name[16];
BT_DataSave m_DataSaveExFlash;
u16 m_BT_FastPackageCnt;
u8* m_BT_FastRecvFlag;

uint32_t crc16(const char *buf, int len);

void BT_RxTask(void *param)
{
	while(1)
	{
		if(m_DataSaveExFlash.m_Updating)
		{
			vTaskDelay(50);
		}
		else
		{
			vTaskDelay(100);
		}
		//-------------------------------------------------------BLE func
		BT_Handler();
	}
}


void BT_Init(void)
{
	BT_Uart3_Init();	
	BT_BRTS(1);
	 m_BT_WorkState = MTT_IDLE;
	 m_DataSaveExFlash.m_Updating = 0;
	printf("BT_Init......\n");
}

//====================================================
//			zhengl
//====================================================
void BT_FastUpdate_Init(uint16_t packageCnt)
{
	uint8_t tmpBufSize;;
	m_BT_FastPackageCnt = packageCnt;
	tmpBufSize = packageCnt / 8;
	if(packageCnt % 8)
	{
		tmpBufSize += 1;
	}
	m_BT_FastRecvFlag = mymalloc(SRAMIN,tmpBufSize);	
	memset(m_BT_FastRecvFlag,0xff,tmpBufSize);
}
void BT_FastUpdate_Recv(uint16_t packageIndex)
{
	uint8_t tmpBitIndex,tmpByteIndex;
	tmpBitIndex = packageIndex % 8;
	tmpByteIndex = packageIndex / 8;
	m_BT_FastRecvFlag[tmpByteIndex] &= ~(0x01 << tmpBitIndex);
}
uint8_t BT_FastUpdate_Check(uint16_t packageIndex)
{
	uint8_t tmpBitIndex,tmpByteIndex;
	tmpBitIndex = packageIndex % 8;
	tmpByteIndex = packageIndex / 8;
	if(m_BT_FastRecvFlag[tmpByteIndex] & (0x01 << tmpBitIndex))
	{
		// no same package index data exist
		return 1;
	}
	else
	{
		return 0;
	}
}
uint8_t BT_FastUpdate_Lost(uint16_t * BufPtr,uint8_t MaxSize)
{
	uint8_t retCnt,tmpBufferSize;
	uint16_t tmpIndex;
	retCnt = 0;
	tmpBufferSize = m_BT_FastPackageCnt / 8;
	if(m_BT_FastPackageCnt % 8)
	{
		tmpBufferSize += 1;
	}
	for(int outLoop = 0;outLoop < tmpBufferSize;outLoop++)
	{
		if(m_BT_FastRecvFlag[outLoop] == 0)
		{
			continue;
		}
		for(int inloop = 0;inloop < 8;inloop++)
		{
			if(m_BT_FastRecvFlag[outLoop] & (0x01 << inloop))
			{
				tmpIndex = outLoop * 8 + inloop;
				if(tmpIndex >= m_BT_FastPackageCnt)
				{
					return retCnt;
				}
				BufPtr[retCnt] = tmpIndex;
				retCnt++;
				if(retCnt >= MaxSize)
				{
					return retCnt;
				}
			}
		}
	}
	return retCnt;
}
u8 BT_CheckStrExist(u8 * str)
{
	pBT_DataStruct tmpPBT;	
	char* tmpPtr;
	char* headPtr;
	tmpPBT = get_BT_ReceiveData();
	if(tmpPBT->m_Index < 4)
	{
		return 0;
	}
	if(tmpPBT->m_Index != tmpPBT->m_Index_Old)
	{
		tmpPBT->m_Index_Old = tmpPBT->m_Index;
		return 0;
	}
	headPtr = (char*)tmpPBT->m_DataBuffer;
	headPtr[tmpPBT->m_Index] = 0;
	tmpPBT->m_Index 		= 0;
	tmpPBT->m_Index_Old 	= 0;
	tmpPtr = strstr(headPtr,(char*)str);
	//printf("---%s---%s===%d+++\r\n",tmpPtr,headPtr,m_BT_WorkState);
	if(tmpPtr)
	{
		switch(m_BT_WorkState)
		{
			case MTT_MAC:
				//printf(",,,%d,,,%d,,,%d,,,%d,,,\r\n",tmpPtr,headPtr + 13,*(tmpPtr - 1),*(tmpPtr - 2));
				if((headPtr + 13 < tmpPtr)&&
					(*(tmpPtr - 1) == 0x0a)&&
					(*(tmpPtr - 2) == 0x0d))
				{
					tmpPtr-= 2;
					*tmpPtr = 0;
					sprintf((char*)(m_BT_Name),"DIVEMASK-%s",tmpPtr - 6);
				}
				break;
		}
		return 1;
	}
	return 0;
}

void BT_CreateAckData(u8 Cmd,u8 flowNum_L,u8 flowNum_H,u8* pData,u8 size)
{
	u16 * pCrc;
	m_BT_TX_Buffer[BT_PKG_OFFSET_HEAD0]			= BT_PKG_CONST_HEAD0;
	m_BT_TX_Buffer[BT_PKG_OFFSET_HEAD1]			= BT_PKG_CONST_HEAD1;
	m_BT_TX_Buffer[BT_PKG_OFFSET_CMD]				= Cmd;
	m_BT_TX_Buffer[BT_PKG_OFFSET_FLOW_L]			= flowNum_L;
	m_BT_TX_Buffer[BT_PKG_OFFSET_FLOW_H]			= flowNum_H;
	m_BT_TX_Buffer[BT_PKG_OFFSET_SIZE]				= size;
	memcpy(m_BT_TX_Buffer + BT_PKG_OFFSET_DATA,pData,size);
	pCrc = (u16*)(m_BT_TX_Buffer + BT_PKG_OFFSET_DATA + size);
	*pCrc = (u16)crc16((const char *)m_BT_TX_Buffer, BT_PKG_OFFSET_DATA + size);
	BT_Uart3_Send_Data(m_BT_TX_Buffer,BT_PKG_OFFSET_DATA + size + 2);
}
void BT_FastUpdateQueryLost(u8 * dataPtr)
{
	uint8_t tmpU16cnt;
	u16 * pCrc;

	tmpU16cnt = BT_FastUpdate_Lost((uint16_t*)(m_BT_TX_Buffer + BT_PKG_OFFSET_DATA),BT_PKG_CONST_MAX_DATA_SIZE / 2);
	m_BT_TX_Buffer[BT_PKG_OFFSET_HEAD0]			= BT_PKG_CONST_HEAD0;
	m_BT_TX_Buffer[BT_PKG_OFFSET_HEAD1]			= BT_PKG_CONST_HEAD1;
	m_BT_TX_Buffer[BT_PKG_OFFSET_CMD]				= BT_CMD_FAST_DATALOST_ASK_ACK;
	m_BT_TX_Buffer[BT_PKG_OFFSET_FLOW_L]			= dataPtr[BT_PKG_OFFSET_FLOW_L];
	m_BT_TX_Buffer[BT_PKG_OFFSET_FLOW_H]			= dataPtr[BT_PKG_OFFSET_FLOW_H];
	m_BT_TX_Buffer[BT_PKG_OFFSET_SIZE]				= tmpU16cnt * 2;
	pCrc = (u16*)(m_BT_TX_Buffer + BT_PKG_OFFSET_DATA + tmpU16cnt * 2);
	*pCrc = (u16)crc16((const char *)m_BT_TX_Buffer, BT_PKG_OFFSET_DATA + tmpU16cnt * 2);
	BT_Uart3_Send_Data(m_BT_TX_Buffer,BT_PKG_OFFSET_DATA + tmpU16cnt * 2 + 2);
}
void BT_FastUpdateFWData(u8 * dataPtr)
{
	u8 tmpbuff[2];
	u32 tmpOffset;
	u16 tmpU16;
	tmpU16 = dataPtr[BT_PKG_OFFSET_FLOW_H];
	tmpU16 <<= 8;
	tmpU16 |= dataPtr[BT_PKG_OFFSET_FLOW_L];
	if(BT_FastUpdate_Check(tmpU16) == 0)
	{
		printf("exist %d\r\n",tmpU16);
		tmpbuff[0] = 6;
		BT_CreateAckData(BT_CMD_DATA_RECV_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);
		return;
	}

	printf("\r\nrec:%d  \r\n",tmpU16);
	tmpbuff[0] = 0;
	BT_CreateAckData(BT_CMD_DATA_RECV_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);
	tmpOffset = BT_PKG_CONST_MAX_DATA_SIZE * tmpU16;
	MX25Lxxx_FastWriteData(tmpOffset,dataPtr + BT_PKG_OFFSET_DATA,dataPtr[BT_PKG_OFFSET_SIZE]);
	if(dataPtr[BT_PKG_OFFSET_SIZE] < BT_PKG_CONST_MAX_DATA_SIZE)
	{
		MX25Lxxx_FastWriteSize(tmpOffset + dataPtr[BT_PKG_OFFSET_SIZE]);
		// update FW from App received all,should reboot
		__set_FAULTMASK(1);
		NVIC_SystemReset();		
	}
}
void BT_UpdateFWData(u8 * dataPtr)
{
	u8 tmpbuff[2];
	u8 isLastPkg;
	u16 tmpU16;
	tmpU16 = dataPtr[BT_PKG_OFFSET_FLOW_H];
	tmpU16 <<= 8;
	tmpU16 |= dataPtr[BT_PKG_OFFSET_FLOW_L];
	if(m_DataSaveExFlash.m_FlowNum != tmpU16)
	{
		printf("=%d %d\r\n",m_DataSaveExFlash.m_FlowNum,tmpU16);
		tmpbuff[0] = 5;
		BT_CreateAckData(BT_CMD_DATA_RECV_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);
		return;
	}
	printf("\r\nrec:%d  \r\n",tmpU16);
	tmpbuff[0] = 0;
	BT_CreateAckData(BT_CMD_DATA_RECV_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);
	m_DataSaveExFlash.m_DataLen = dataPtr[BT_PKG_OFFSET_SIZE];
	m_DataSaveExFlash.m_DataPtr = dataPtr + BT_PKG_OFFSET_DATA;
	if(m_DataSaveExFlash.m_DataLen < BT_PKG_CONST_MAX_DATA_SIZE)
	{
		isLastPkg = 1;
	}
	else
	{
		isLastPkg = 0;
	}
	//	save data begin
	MX25Lxxx_WriteApp(&m_DataSaveExFlash,isLastPkg);
	//	save data end
	if(isLastPkg)
	{
		// update FW from App received all,should reboot
		__set_FAULTMASK(1);
		NVIC_SystemReset();
	}
}
u8 BT_SetDateTime(u8* pData,u8 size)
{
	RTC_DateTypeDef tmpdatestructure;
	RTC_TimeTypeDef tmptimestructure;	
	tmpdatestructure.Year = pData[0];
	tmpdatestructure.Month = pData[1];
	tmpdatestructure.Date = pData[2];
	tmpdatestructure.WeekDay = pData[3];

	tmptimestructure.Hours = pData[4];
	tmptimestructure.Minutes = pData[5];
	tmptimestructure.Seconds = pData[6];
	tmptimestructure.SubSeconds = 0x00;
	tmptimestructure.SecondFraction = 0xff;
	tmptimestructure.DayLightSaving = 0x00;
	tmptimestructure.StoreOperation = 0x00;
	tmptimestructure.TimeFormat = 0x00;

	HAL_RTC_SetTime(&RTC_Handler, &tmptimestructure, RTC_FORMAT_BIN);
	HAL_RTC_SetDate(&RTC_Handler, &tmpdatestructure, RTC_FORMAT_BIN);
	return 1;
}
u8 BT_Is_FW_Updating(void)
{
	if(m_DataSaveExFlash.m_DataStoreOffset > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
u8 BT_Fetch_Percent(void)
{
	return (u8)((m_DataSaveExFlash.m_DataStoreOffset) / (105* 10));
}
u8 BT_DataParserByCommand(u8* dataPtr)
{
	u8 retStatus = 0;
	u8 tmpbuff[2];
	u16 tmpData;
	switch(dataPtr[BT_PKG_OFFSET_CMD])
	{
		case BT_CMD_UPDATE_FW:
			printf("start update...\r\n");
			m_DataSaveExFlash.m_FlowNum = 0;
			m_DataSaveExFlash.m_DataStoreOffset = 0;
			m_DataSaveExFlash.m_Updating = 1;
			tmpbuff[0] = 0;
			BT_CreateAckData(BT_CMD_UPDATE_FW_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);
			break;
		case BT_CMD_DATA_RECV:
			BT_UpdateFWData(dataPtr);
			break;
		case BT_CMD_RD_FWVERSION:
			tmpbuff[0] = FW_VERSION_PART_0;
			tmpbuff[1] = FW_VERSION_PART_1;
			BT_CreateAckData(BT_CMD_RD_FWVERSION_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,2);
			break;
		case BT_CMD_SET_DATETIME:
			BT_SetDateTime(dataPtr + BT_PKG_OFFSET_DATA,dataPtr[BT_PKG_OFFSET_SIZE]);
			tmpbuff[0] = 0;
			BT_CreateAckData(BT_CMD_DATA_RECV_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);			
			break;
		case BT_CMD_BIND_OXYGEN_ADDR:
			tmpData = dataPtr[BT_PKG_OFFSET_DATA + 0];
			tmpData <<= 8;
			tmpData |= dataPtr[BT_PKG_OFFSET_DATA + 1];
			MX25Lxxx_WriteOxygenAddr(tmpData);
			tmpbuff[0] = 0;
			BT_CreateAckData(BT_CMD_BIND_OXYGEN_ADDR_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);
			break;
		case BT_CMD_UNBIND_OXYGEN_ADDR:
			tmpData = 0x0000;
			MX25Lxxx_WriteOxygenAddr(tmpData);
			tmpbuff[0] = 0;
			BT_CreateAckData(BT_CMD_UNBIND_OXYGEN_ADDR_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);			
			break;
		case BT_CMD_READ_OXYGEN_ADDR:
			MX25Lxxx_ReadOxygenAddr(&tmpData);
			tmpbuff[1] = (u8)(tmpData & 0xff);
			tmpData >>= 8;
			tmpbuff[0] = (u8)(tmpData & 0xff);
			BT_CreateAckData(BT_CMD_READ_OXYGEN_ADDR_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,2);		
			break;
		case BT_CMD_FAST_UPDATE_FW:
			if(BT_Is_FW_Updating())
			{
				tmpbuff[0] = 1;
				BT_CreateAckData(BT_CMD_FAST_UPDATE_FW_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);			
				return retStatus;
			}
			tmpData = dataPtr[BT_PKG_OFFSET_DATA + 0];
			tmpData <<= 8;
			tmpData |= dataPtr[BT_PKG_OFFSET_DATA + 1];		
			BT_FastUpdate_Init(tmpData);
			tmpbuff[0] = 0;
			BT_CreateAckData(BT_CMD_FAST_UPDATE_FW_ACK,dataPtr[BT_PKG_OFFSET_FLOW_L],dataPtr[BT_PKG_OFFSET_FLOW_H],tmpbuff,1);			
			break;
		case BT_CMD_FAST_DATA_RECV:
			BT_FastUpdateFWData(dataPtr);
			break;
		case BT_CMD_FAST_DATALOST_ASK:
			BT_FastUpdateQueryLost(dataPtr);
			break;
		case BT_CMD_DIVING_LOG:
			break;
		default:
			break;
	}
	return retStatus;
}
u8 BT_QueryConnectedOrNot(void)
{
	if(m_BT_Countdown > 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
u8 BT_RecDataParser(void)
{
	u8 tmpbuff[1];
	u16 tmpCrc;
	u16 recCrc;
	pBT_DataStruct tmpPBT;
	tmpPBT = get_BT_ReceiveData();
	if(tmpPBT->m_Index < 4)
	{
		m_BT_RecvData_CD = BT_RECV_DATA_TIMEOUT_VALUE;
		return 0;
	}
	else
	{
		m_BT_RecvData_CD--;
	}
	m_BT_Countdown = BT_DEV_EXIST_COUNTDOWN;
	
	if(tmpPBT->m_Index != tmpPBT->m_Index_Old)
	{
		tmpPBT->m_Index_Old = tmpPBT->m_Index;
		return 0;
	}
#if BT_TOGGLE_RECV_BUFFER
	BT_Switch_RecBuffer(tmpPBT);
#endif
	if(tmpPBT->m_Index < BT_PKG_CONST_NODATA_SIZE)
	{
//		tmpPBT->m_Index_Old = 0;
//		tmpPBT->m_Index = 0;
//		tmpbuff[0] = 1;
//		BT_CreateAckData(BT_CMD_DATA_RECV_ACK,0x00,0x00,tmpbuff,1);
//		printf("pkg too small\r\n");
//		return 0;
		goto WAIT;
	}
	if((tmpPBT->m_DataBuffer[BT_PKG_OFFSET_HEAD0] != BT_PKG_CONST_HEAD0)||
		(tmpPBT->m_DataBuffer[BT_PKG_OFFSET_HEAD1] != BT_PKG_CONST_HEAD1))
	{
		tmpPBT->m_Index_Old = 0;
		tmpPBT->m_Index = 0;
		tmpbuff[0] = 2;
		BT_CreateAckData(BT_CMD_DATA_RECV_ACK,0x00,0x00,tmpbuff,1);
		printf("head error!\r\n");
		return 0;
	}
	if(tmpPBT->m_DataBuffer[BT_PKG_OFFSET_SIZE] + BT_PKG_CONST_NODATA_SIZE >  tmpPBT->m_Index)
	{
//		tmpPBT->m_Index_Old = 0;
//		tmpPBT->m_Index = 0;
//		tmpbuff[0] = 3;
//		BT_CreateAckData(BT_CMD_DATA_RECV_ACK,0x00,0x00,tmpbuff,1);
//		printf("broken error!\r\n");
//		return 0;
		goto WAIT;
	}

	recCrc = tmpPBT->m_DataBuffer[tmpPBT->m_DataBuffer[BT_PKG_OFFSET_SIZE] + BT_PKG_OFFSET_DATA + 1];
	recCrc <<= 8;
	recCrc |= tmpPBT->m_DataBuffer[tmpPBT->m_DataBuffer[BT_PKG_OFFSET_SIZE] + BT_PKG_OFFSET_DATA + 0];
	tmpCrc = (u16)crc16((const char *)(tmpPBT->m_DataBuffer),tmpPBT->m_DataBuffer[BT_PKG_OFFSET_SIZE] + BT_PKG_OFFSET_DATA);
	if(tmpCrc != recCrc)
	{
		tmpbuff[0] = 4;
		BT_CreateAckData(BT_CMD_DATA_RECV_ACK,0x00,0x00,tmpbuff,1);
		printf("rec:%04x self:%04x\r\n",recCrc,tmpCrc);
		return 0;
	}
	tmpPBT->m_Index_Old = 0;
	tmpPBT->m_Index = 0;
	return BT_DataParserByCommand(tmpPBT->m_DataBuffer);
WAIT:
	if(m_BT_RecvData_CD == 0)
	{
		tmpPBT->m_Index_Old = 0;
		tmpPBT->m_Index = 0;
		tmpbuff[0] = 7;
		BT_CreateAckData(BT_CMD_DATA_RECV_ACK,0x00,0x00,tmpbuff,1);
	}
	return 0;
}

void BT_Handler(void)
{
	pBT_DataStruct tmpPBT;	
	switch(m_BT_WorkState)
	{
		case MTT_IDLE:		// power on
			BT_BRTS(0);
			m_BT_WorkState = MTT_MAC;//MTT_RST;		
			m_BT_Countdown = 0;
			break;
		case MTT_RST:		// reset
		//	printf("BT_RST  \r\n");
			BT_Uart3_Send_Data("TTM:RST-",8);
			m_BT_WorkState = MTT_TST;
			m_BT_Countdown = 0;
			//ForTest();
			break;
		case MTT_TST:		// test
		//	printf("MTT_TST  \r\n");
			if(m_BT_Countdown == 0)
			{
				BT_Uart3_Send_Data("TTM:TST-",8);
				m_BT_Countdown = 0;
			}
			else if(BT_CheckStrExist("TTM:OK"))
			{
				m_BT_WorkState = MTT_MAC;
				m_BT_Countdown = 0;
				break;
			}
			m_BT_Countdown++;
			if(m_BT_Countdown > BT_WAIT_TIMES)
			{
				m_BT_Countdown = 0;
			}			
			break;
		case MTT_FAST:
		//	printf("MTT_FAST  \r\n");
			if(m_BT_Countdown == 0)
			{
				BT_Uart3_Send_Data("TTM:BPS-115200",14);
				m_BT_Countdown = 0;
			}
			else if(BT_CheckStrExist("TTM:OK"))
			{
				BT_Uart3_SwitchToFaster();	
		//		Uart2_SwitchToFaster();
				BT_BRTS(1);			
				m_BT_WorkState = MTT_FAST_1;
				m_BT_Countdown = 0;
				break;
			}
			m_BT_Countdown++;
			if(m_BT_Countdown > BT_WAIT_TIMES)
			{
				m_BT_Countdown = 0;
			}
			break;
		case MTT_FAST_1:
			BT_BRTS(0);
			m_BT_WorkState = MTT_MAC;
			m_BT_Countdown = 0;
		case MTT_MAC:		// get mac
		//	printf("MTT_MAC  \r\n");
			tmpPBT = get_BT_ReceiveData();			
			printf("%d+%d+\r\n",tmpPBT->m_Index,tmpPBT->m_Index_Old);
			if(m_BT_Countdown == 0)
			{
				BT_Uart3_Send_Data("TTM:MAC-?",9);
				m_BT_Countdown = 0;
			}
			else if(BT_CheckStrExist("TTM:OK"))
			{
				if(m_BT_Name[0] != 0)
				{
					m_BT_WorkState = MTT_REN;
					m_BT_Countdown = 0;
					break;
				}
			}
			m_BT_Countdown++;
			if(m_BT_Countdown > BT_WAIT_TIMES)
			{
				m_BT_Countdown = 0;
			}
			break;
		case MTT_REN:		// rename
		//	printf("MTT_REN  \r\n");
			if(m_BT_Countdown == 0)
			{
				sprintf((char*)(m_BT_TX_Buffer),"TTM:REN-%s",m_BT_Name);
				BT_Uart3_Send_Data(m_BT_TX_Buffer,strlen((char*)(m_BT_TX_Buffer)));
				m_BT_Countdown = 0;
			}
			else if(BT_CheckStrExist("TTM:OK"))
			{
				m_BT_WorkState = MTT_COM;//MTT_ADV;
				m_BT_Countdown = 0;
				break;
			}
			m_BT_Countdown++;
			if(m_BT_Countdown > BT_WAIT_TIMES)
			{
				m_BT_Countdown = 0;
			}	
			break;
		case MTT_ADV:		// adv on,off
		//	printf("MTT_ADV  \r\n");
			if(m_BT_Countdown == 0)
			{
				BT_Uart3_Send_Data("TTM:ADV-ON",10);
				m_BT_Countdown = 0;
			}
			else if(BT_CheckStrExist("TTM:OK"))
			{
				m_BT_WorkState = MTT_COM;
				m_BT_Countdown = 0;
				break;
			}
			m_BT_Countdown++;
			if(m_BT_Countdown > BT_WAIT_TIMES)
			{
				m_BT_Countdown = 0;
			}
			break;
		case MTT_COM:		// wait for server ack info
			if(m_BT_Countdown > 0)
			{
				m_BT_Countdown--;
			}
			BT_RecDataParser();
			break;
		case MTT_PASS:		// pass through
			//		can transmate any data
			break;
		default:
			break;
	}	
}

//==========================================================
//		Crc			function
//==========================================================
static const uint32_t crc16tab[256]=
{ 
	0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef,
	0x1231,0x0210,0x3273,0x2252,0x52b5,0x4294,0x72f7,0x62d6,
	0x9339,0x8318,0xb37b,0xa35a,0xd3bd,0xc39c,0xf3ff,0xe3de,
	0x2462,0x3443,0x0420,0x1401,0x64e6,0x74c7,0x44a4,0x5485,
	0xa56a,0xb54b,0x8528,0x9509,0xe5ee,0xf5cf,0xc5ac,0xd58d,
	0x3653,0x2672,0x1611,0x0630,0x76d7,0x66f6,0x5695,0x46b4,
	0xb75b,0xa77a,0x9719,0x8738,0xf7df,0xe7fe,0xd79d,0xc7bc,
	0x48c4,0x58e5,0x6886,0x78a7,0x0840,0x1861,0x2802,0x3823,
	0xc9cc,0xd9ed,0xe98e,0xf9af,0x8948,0x9969,0xa90a,0xb92b,
	0x5af5,0x4ad4,0x7ab7,0x6a96,0x1a71,0x0a50,0x3a33,0x2a12,
	0xdbfd,0xcbdc,0xfbbf,0xeb9e,0x9b79,0x8b58,0xbb3b,0xab1a,
	0x6ca6,0x7c87,0x4ce4,0x5cc5,0x2c22,0x3c03,0x0c60,0x1c41,
	0xedae,0xfd8f,0xcdec,0xddcd,0xad2a,0xbd0b,0x8d68,0x9d49,
	0x7e97,0x6eb6,0x5ed5,0x4ef4,0x3e13,0x2e32,0x1e51,0x0e70,
	0xff9f,0xefbe,0xdfdd,0xcffc,0xbf1b,0xaf3a,0x9f59,0x8f78,
	0x9188,0x81a9,0xb1ca,0xa1eb,0xd10c,0xc12d,0xf14e,0xe16f,
	0x1080,0x00a1,0x30c2,0x20e3,0x5004,0x4025,0x7046,0x6067,
	0x83b9,0x9398,0xa3fb,0xb3da,0xc33d,0xd31c,0xe37f,0xf35e,
	0x02b1,0x1290,0x22f3,0x32d2,0x4235,0x5214,0x6277,0x7256,
	0xb5ea,0xa5cb,0x95a8,0x8589,0xf56e,0xe54f,0xd52c,0xc50d,
	0x34e2,0x24c3,0x14a0,0x0481,0x7466,0x6447,0x5424,0x4405,
	0xa7db,0xb7fa,0x8799,0x97b8,0xe75f,0xf77e,0xc71d,0xd73c,
	0x26d3,0x36f2,0x0691,0x16b0,0x6657,0x7676,0x4615,0x5634,
	0xd94c,0xc96d,0xf90e,0xe92f,0x99c8,0x89e9,0xb98a,0xa9ab,
	0x5844,0x4865,0x7806,0x6827,0x18c0,0x08e1,0x3882,0x28a3,
	0xcb7d,0xdb5c,0xeb3f,0xfb1e,0x8bf9,0x9bd8,0xabbb,0xbb9a,
	0x4a75,0x5a54,0x6a37,0x7a16,0x0af1,0x1ad0,0x2ab3,0x3a92,
	0xfd2e,0xed0f,0xdd6c,0xcd4d,0xbdaa,0xad8b,0x9de8,0x8dc9,
	0x7c26,0x6c07,0x5c64,0x4c45,0x3ca2,0x2c83,0x1ce0,0x0cc1,
	0xef1f,0xff3e,0xcf5d,0xdf7c,0xaf9b,0xbfba,0x8fd9,0x9ff8,
	0x6e17,0x7e36,0x4e55,0x5e74,0x2e93,0x3eb2,0x0ed1,0x1ef0
};

uint32_t crc16(const char *buf, int len)
{
	int counter;
	uint32_t crc = 0;
	for (counter = 0; counter < len; counter++)
	{
		crc = (crc<<8) ^ crc16tab[((crc>>8) ^ *buf++)&0x00FF];
	}
	return crc;
}


