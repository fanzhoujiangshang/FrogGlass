#ifndef __UART_SI446_H
#define __UART_SI446_H
#include <stdbool.h>
#include "sys.h"

/********************************************************************************	 
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C) 
 * All rights reserved
********************************************************************************/
extern UART_HandleTypeDef huart1;

#define	SI446_DATA_VALUE_HEAD				(0xe2)

enum
{
	SI446_DATA_OFFSET_HEAD,
	SI446_DATA_OFFSET_ADDH,
	SI446_DATA_OFFSET_ADDL,
	SI446_DATA_OFFSET_DATALEN,
	SI446_DATA_OFFSET_DEVTYPE,	
	SI446_DATA_OFFSET_DATA_PWR,
	SI446_DATA_OFFSET_DATA_OXYGEN,	
	SI446_DATA_OFFSET_CHKSUM,
	SI446_DATA_OFFSET_MAX
};
enum
{
	SI446_DEV_OXYGEN,
	SI446_DEV_MAX
};

enum
{
	SI446_ADDRESS_H,
	SI446_ADDRESS_L
};


void uartSi446Init(void)	;/*串口初始化*/
bool uartsi446GetOxygen(u8 *Oxygen,u8 * Power);
void USR_UART1_RxCpltCallback(void);

u8 uartSI446WriteData(void);
void SI446_ReceiveDataHandler(void);
void WriteModuleConfig(void);
void PushInNewData(u8 data);
#endif /* __UART_SI446_H */
