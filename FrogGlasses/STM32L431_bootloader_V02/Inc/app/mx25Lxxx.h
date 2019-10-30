#ifndef	_MX25LXXX_H_
#define	_MX25LXXX_H_
#include "app\sys.h" 
//÷∏¡Ó±Ì
#define W25X_WriteEnable			0x06 
#define W25X_WriteDisable			0x04 
#define W25X_ReadStatusReg1		0x05 
#define W25X_ReadStatusReg2		0x35 
#define W25X_ReadStatusReg3		0x15 
#define W25X_WriteStatusReg1    		0x01 
#define W25X_WriteStatusReg2   		0x31 
#define W25X_WriteStatusReg3    		0x11 
#define W25X_ReadData				0x03 
#define W25X_FastReadData			0x0B 
#define W25X_FastReadDual			0x3B 
#define W25X_PageProgram			0x02 
#define W25X_BlockErase				0xD8 
#define W25X_SectorErase			0x20 
#define W25X_ChipErase				0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown		0xAB 
#define W25X_DeviceID				0xAB 
#define W25X_ManufactDeviceID		0x90 
#define W25X_JedecDeviceID			0x9F 
#define W25X_Enable4ByteAddr    		0xB7
#define W25X_Exit4ByteAddr      		0xE9


typedef struct	
{
	u8 	m_FW_Entry;						/*current FW stored in MX25L128*/
	u8	m_FWUpdatedFlag;
	int16_t 	m_Mag_Min[3];
	int16_t 	m_Mag_Max[3];
	u8	m_Language;
	int16_t 	m_Gyro_Bias[3];	
	uint16_t m_OxygenAddr;
} configParameters;

extern configParameters m_ConfigParams;

#define	MX25L_CONFIG_SAVED_ADDR					(1024 * 1024 * 13)

void MX25Lxxx_Test(void);
void MX25Lxxx_Erase_Chip(void);
void MX25Lxxx_Erase_Sector(u32 Dst_Addr);
void MX25Lxxx_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
void MX25Lxxx_Read(u8* pBuffer,u32 ReadAddr,u16 NumByteToRead);
void MX25Lxxx_PowerDown(void);
void MX25Lxxx_WakeUp(void);
uint8_t Check_FW_Update_Flag(uint8_t * Entry);
#endif

