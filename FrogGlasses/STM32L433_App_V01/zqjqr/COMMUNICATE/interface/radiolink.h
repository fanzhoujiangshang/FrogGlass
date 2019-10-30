#ifndef __RADIO_H
#define __RADIO_H
#include <stdint.h>
#include <stdbool.h>
#include "atkp.h"

/********************************************************************************	 
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C) 
 * All rights reserved
********************************************************************************/

#define	FW_VERSION_PART_0					(0x01)
#define	FW_VERSION_PART_1					(0x00)

void BT_Init(void);
void BT_RxTask(void *param);
//====================================================
//			zhengl
//====================================================
#define	BT_PKG_OFFSET_HEAD0				(0x00)
#define	BT_PKG_OFFSET_HEAD1				(0x01)

#define	BT_PKG_OFFSET_CMD				(0x02)
#define	BT_PKG_OFFSET_FLOW_L				(0x03)
#define	BT_PKG_OFFSET_FLOW_H				(0x04)
#define	BT_PKG_OFFSET_SIZE				(0x05)
#define	BT_PKG_OFFSET_DATA				(0x06)


//-------------------------------------------------------const value
#define	BT_PKG_CONST_HEAD0				(0xff)
#define	BT_PKG_CONST_HEAD1				(0xdf)

#define	BT_PKG_CONST_NODATA_SIZE		(BT_PKG_OFFSET_DATA + 2)
#define	BT_PKG_CONST_MAX_DATA_SIZE		(128)//(168)

enum
{
	//---------------------------------------------------update begin
	BT_CMD_UPDATE_FW						=	0x01,
	BT_CMD_UPDATE_FW_ACK				=	0x81,
	
	BT_CMD_DATA_RECV						=	0x02,
	BT_CMD_DATA_RECV_ACK				=	0x82,
	//---------------------------------------------------update end
	BT_CMD_RD_FWVERSION					=	0x03,
	BT_CMD_RD_FWVERSION_ACK			=	0x83,

	BT_CMD_SET_DATETIME					=	0x04,
	BT_CMD_SET_DATETIME_ACK				=	0x84,	

	BT_CMD_BIND_OXYGEN_ADDR			=	0x05,
	BT_CMD_BIND_OXYGEN_ADDR_ACK		=	0x85,	

	BT_CMD_UNBIND_OXYGEN_ADDR			=	0x06,
	BT_CMD_UNBIND_OXYGEN_ADDR_ACK		=	0x86,

	BT_CMD_READ_OXYGEN_ADDR			=	0x07,
	BT_CMD_READ_OXYGEN_ADDR_ACK		=	0x87,	
	//---------------------------------------------------fast update begin
	BT_CMD_FAST_UPDATE_FW				=	0x08,
	BT_CMD_FAST_UPDATE_FW_ACK			=	0x88,	

	BT_CMD_FAST_DATA_RECV				=	0x09,
	BT_CMD_FAST_DATA_RECV_ACK			=	0x89,	

	BT_CMD_FAST_DATALOST_ASK			=	0x0A,
	BT_CMD_FAST_DATALOST_ASK_ACK		=	0x8A,
	
	BT_CMD_DIVING_LOG             = 0x0B,
	BT_CMD_DIVING_LOG_ACK          = 0x8B,
	//---------------------------------------------------fast update end
	BT_CMD_RESERVED						=	0xFF
};

enum
{
	MTT_IDLE,		// power on
	MTT_RST,		// reset	
	MTT_TST,		// test
	MTT_MAC,		// get mac
	MTT_REN,		// rename
	MTT_ADV,		// adv on,off
	MTT_COM,		// communicate
	MTT_FAST,		// change bandrate to 115200 or 38400	
	MTT_FAST_1,
	MTT_PASS		// pass through
};

#define	BT_WAIT_TIMES					(3)

void BT_Handler(void);
u8 BT_Is_FW_Updating(void);
u8 BT_Fetch_Percent(void);
u8 BT_QueryConnectedOrNot(void);
#endif /*__RADIO_H */

