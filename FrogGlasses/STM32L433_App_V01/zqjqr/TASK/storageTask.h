#ifndef __STORAGE_TASK_H
#define __STORAGE_TASK_H
#include "sys.h"
#include <stdbool.h>
#include "rtc.h"
 
 #include "global.h"

//#include "vpmb.h"
/********************************************************************************	 
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C) 
 * All rights reserved
********************************************************************************/


enum
{
	LANGUAGE_ENG,
	LANGUAGE_CHI,
	LANGUAGE_MAX
};
enum
{
	FIRMWARE_A,
	FIRMWARE_B,
	FIRMWARE_MAX
};
typedef struct {
	int16_t accZero[3];
	int16_t accGain[3];
} accBias_t;

typedef struct {
	int16_t magZero[3];
} magBias_t;

typedef struct {
    int16_t rollDeciDegrees;
    int16_t pitchDeciDegrees;
    int16_t yawDeciDegrees;
} boardAlignment_t;

typedef struct {      
    uint8_t date;      
    uint8_t month;       
    uint8_t year;     
} diveDate_t;

typedef struct {
    uint8_t sec;         
    uint8_t min;         
    uint8_t hr;           
} diveTime_t;

enum dive_mode_t
{
	SCUBA,
	FREESTYLE,
	METER,	
};

typedef struct {
	u8 recordNumber;		
	u8 diveMode;
	diveDate_t diveDate;
	diveTime_t diveTime;
	diveTime_t currentTime;
	u16 temperature;
	u16 depth[126];
	u16 maxDepth;
} diveRecord_t;			//sizeof 268

extern diveRecord_t *diveRecord;
//extern u8 diveRecordEnable;

typedef struct
{
	u8 m_Updating;
	u16 m_FlowNum;
	u8 m_DataLen;
	u32 m_DataStoreOffset;
	u8* m_DataPtr;
}BT_DataSave;

typedef struct	
{
	u8 	m_FW_Entry;						/*current FW stored in MX25L128*/
	u8	m_FWUpdatedFlag;
	int16_t 	m_Mag_Min[3];
	int16_t 	m_Mag_Max[3];
	u8	m_Language;
	int16_t 	m_Gyro_Bias[3];	
	uint16_t m_OxygenAddr;
	uint32_t m_checkSum;
} configParameters;

typedef struct
{	// 20 byte
	u8 m_TotalRecordNum;
	u8 m_NewestRecordOffset;
}RecIndex_Info;
typedef struct
{	// 20 byte
	// save one point every 2 seconds
	diveDate_t m_DiveDate;
	diveTime_t m_DiveTime;
	uint32_t m_DiveSpan;	
	uint16_t m_PointNum;
	uint16_t m_DepthMax;
	uint16_t m_Temperature;
	uint8_t m_DiveMode;
	uint8_t m_Reserved;
}Dive_Head;

typedef struct
{
	// save one point every 2 seconds
	double m_Latitude;
	double m_Longitude;
	uint16_t m_Temperature;
	uint16_t m_Depth;
	uint16_t m_SecondsSpan;
}Position_Point;

enum
{
	REC_FLAG_NO_DATA		= 0x00,
	REC_FLAG_HAVE_DATA	= 0x01,
	REC_FLAG_NEW_DATA	= 0x02,
	REC_FALG_RESERVED,		// other value
};

extern configParameters m_ConfigParams;
extern RecIndex_Info g_RecordIndexInfo;

#define	MX25L_DIVE_LIST_INDICATOR_ADDR_START			(0)

#define	MX25L_DIVE_LIST_HEAD_ADDR_START					(1024)		// head data save address
#define	MX25L_DIVE_LIST_RECORD_COUNT_MAX				(100)

#define	MX25L_DIVE_LIST_DATA_ADDR_START					(1024 * 5)	// sample points save start address
#define	MX25L_DIVE_DATA_MAX_SIZE_PER_RECORD			(1024 * 80)	// each recored have max size 80k space to save data
#define	MX25L_DIVE_LIST_DATA_N_ADDR_START(x)			(MX25L_DIVE_DATA_MAX_SIZE_PER_RECORD * x + MX25L_DIVE_LIST_DATA_ADDR_START)
#define	MX25L_DIVE_DATA_MAX_COUNT_PER_RECORD			(99 * 30)

#define	MX25L_DIVE_LIST_DATA_ADDR_END					(MX25L_DIVE_LIST_DATA_N_ADDR_START(MX25L_DIVE_LIST_RECORD_COUNT_MAX))



#define	MX25L_DIVE_RECORD_ADDR							(1024 * 1024 * 12)					// 268 x 16 = 4288

#define	MX25L_CONFIG_SAVED_ADDR							(1024 * 1024 * 13)

#define	MX25L_APPLICATION_A_ADDR						(1024 * 1024 * 14)					// 14M	4K-bytes sector
#define	MX25L_APPLICATION_A_SIZE							(256 * 1024)						// 256k		256 * 1024

#define	MX25L_APPLICATION_B_ADDR						(MX25L_APPLICATION_A_ADDR + MX25L_APPLICATION_A_SIZE)		// 4K-bytes sector
#define	MX25L_APPLICATION_B_SIZE							(MX25L_APPLICATION_A_SIZE)									// 256k		256 * 1024

#define	MX25L_APP_DATA_START_OFFSET						(4)




void configParamTask(void* param);	/*参数配置任务*/

void MX25Lxxx_Test(void);
void MX25Lxxx_FastWriteData(uint32_t writeOffset,uint8_t* dataPtr,uint8_t dataLen);
void MX25Lxxx_FastWriteSize(uint32_t writeDataSize);
void MX25Lxxx_WriteApp(BT_DataSave* PparaData,u8 IsLastPkg);
void MX25Lxxx_ReadConfig(void);
void MX25Lxxx_WriteConfig(void);
void MX25Lxxx_ReadMagnet(int16_t* minSave,int16_t* maxSave);
void MX25Lxxx_WriteMagnet(int16_t* minSave,int16_t* maxSave);
void MX25Lxxx_WriteMagnet_2(void);
void MX25Lxxx_ReadLanguage(uint8_t* paraLang);
void MX25Lxxx_WriteLanguage(uint8_t paraLang);
void MX25Lxxx_ReadGyro(int16_t* gyroSave);
void MX25Lxxx_WriteGyro(int16_t* gyroSave);
void MX25Lxxx_WriteConfigDefault(void);
void MX25Lxxx_ReadOxygenAddr(uint16_t* pAddrSave);
void MX25Lxxx_WriteOxygenAddr(uint16_t AddrSave);
void DiveRecordStart(void);
void DiveRecordFinish(void);
void ReadRecordGetHeadInfo(u8 Offset,Dive_Head * pHead);
void ReadRecordGetPointInfo(u8 RecIndex,u8 PointIndex,Position_Point * pPoint);
void DiveRecordStopFromOutside(void);
#endif /*__STORAGE_TASK_H */

