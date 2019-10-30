#ifndef __UART_LINK_H
#define __UART_LINK_H
#include <stdbool.h>
#include "sys.h"

/********************************************************************************	 
 * 创建日期:2018/6/22
 * 版本：V1.2
 * 版权所有，盗版必究。
 * Copyright(C) 
 * All rights reserved
********************************************************************************/
extern UART_HandleTypeDef huart3;

#define	BT_BUFFER_SIZE			(200)
typedef struct
{
	u8 m_Index;
	u8 m_Index_Old;
	u8 m_DataBuffer[BT_BUFFER_SIZE];
}BT_DataStruct,*pBT_DataStruct;

void BT_Uart3_Init(void)	;/*串口初始化*/
void BT_Uart3_Send_Data(uint8_t* data, uint32_t size);
void USR_UART3_RxCpltCallback(void);
pBT_DataStruct get_BT_ReceiveData(void);
void BT_Uart3_SwitchToFaster(void);
void BT_Switch_RecBuffer(pBT_DataStruct pST);
#endif /* __UART_LINK_H */
