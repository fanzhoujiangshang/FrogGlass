#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include "atkp.h"
#include "radiolink.h"
#include "stabilizerTask.h"
//#include "pm.h"
#include "sensorsTask.h"
//#include "mpuTask.h"
#include "storageTask.h"
#include "axis.h"
#include "imu.h"
#include "gyro.h"
#include "accelerometer.h"
#include "compass.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "queue.h"
//#include "usr_sys.h"

/********************************************************************************	 
 * 本程序只供学习使用，未经作者许可，不得用于其它任何用途
 * 无线通信驱动代码	
 * 创建日期:2017/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
 * 说明：此文件程序基于于匿名科创地面站V4.34通信协议下位机
 *     示例代码修改。
********************************************************************************/

//数据拆分宏定义
#define  BYTE0(dwTemp)       ( *( (u8 *)(&dwTemp)	)  )
#define  BYTE1(dwTemp)       ( *( (u8 *)(&dwTemp) + 1) )
#define  BYTE2(dwTemp)       ( *( (u8 *)(&dwTemp) + 2) )
#define  BYTE3(dwTemp)       ( *( (u8 *)(&dwTemp) + 3) )

//????????(??ms)
#define  PERIOD_STATUS		0
#define  PERIOD_STATUS1 		1

#define ATKP_RX_QUEUE_SIZE 	10 /*ATKP包接收队列消息个数*/

#define  DEVICE_LOCK				0x01
#define  DEEP_LOCK					0x02


typedef struct
{
	float roll;			//横滚角
	float pitch;			//俯仰角
	float yaw;			//偏航角
	float depth;			//深度
	float heading;		//相对位置
	float ObstacleDistance;//障碍物距离
	u16 IO_switch;	        //位0  ：设备锁定    位1  ：深度锁定  （1：开启，0：关闭）
}status;

typedef struct
{
	u8 masterslave;  	//主从设备
	float temp;			//温度
	float DeviceBatt; 		//设备电量  
	float BuoyBatt ; 		//浮标电量 
}status_1;

typedef struct
{
	u16 Hardware;
	u16 Software;
}version_info;

static u16 IOSwitch = 0;

static void atkpSendPacket(atkp_t *p)
{
	//radiolinkSendPacket(p);	
//	radiolinkAckPacket(p);
}

static void atkpAckPacket(atkp_t *p)
{	
//	radiolinkAckPacket(p);
}

/***************************??????????******************************/
static void sendStatus(status status )
{
	u8 _cnt=0;
	atkp_t p;
	#if 1
	vs16 _temp;
	
	p.msgID = UP_STATUS;
	
	_temp = (int)(status.roll*100);
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	_temp = (int)(status.pitch*100);
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	_temp = (int)(status.yaw*100);
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	 _temp = (int)(status.depth*100);	
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	 _temp = (int)(status.heading*100);	
	p.data[_cnt++] = BYTE1(_temp);
	p.data[_cnt++] = BYTE0(_temp);
	 _temp = status.IO_switch;	
	p.data[_cnt++] = BYTE1(_temp);
	p.data[_cnt++] = BYTE0(_temp);	
	#endif
	p.dataLen = _cnt;
	atkpSendPacket(&p);
}

static void sendStatus1(status_1 status)
{
	u8 _cnt=0;
	atkp_t p;
	#if 1
	vs16 _temp;
	
	p.msgID = UP_STATUS1;
	
	p.data[_cnt++]=status.masterslave;
	_temp = (int)(status.temp*100);
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	 _temp = (int)(status.DeviceBatt*100);	
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);
	 _temp = (int)(status.BuoyBatt*100);	
	p.data[_cnt++]=BYTE1(_temp);
	p.data[_cnt++]=BYTE0(_temp);	
	
	p.dataLen = _cnt;
	#endif
	atkpSendPacket(&p);
}

static void sendCheck(u8 head, u8 check_sum)
{
	atkp_t p;
	
	p.msgID = UP_CHECK;
	p.dataLen = 2;
	p.data[0] = head;
	p.data[1] = check_sum;
	atkpAckPacket(&p);
}
/****************************************************************************/

/*数据周期性发送给上位机，每1ms调用一次*/
void atkpSendPeriod(void)
{
	static u16 count_ms = 0;
	status status;
	status_1 status_1;	
	switch(count_ms%2)
	{
		case PERIOD_STATUS:
		{
			status.roll = state.attitude.roll;
			status.pitch = -state.attitude.pitch;
			status.yaw = -state.attitude.yaw;
			status.depth = sensors.baro.depth;
			status.heading = 10;
			status.ObstacleDistance = 100;
			status.IO_switch = IOSwitch;
//printf("roll=%6.2f ,pitch=%6.2f ,yaw=%6.2f \n",state.attitude.roll,state.attitude.pitch,state.attitude.yaw);			
//printf("depth=%6.2f ,pressure=%6.2f  \n",sensors.baro.depth, sensors.baro.pressure);		
			sendStatus(status);	
		}
		break;
										
		case PERIOD_STATUS1:
		{   
			status_1.masterslave = 0xFF;
			status_1.temp = sensors.baro.temperature;			
//			status_1.DeviceBatt = pmGetBatteryVoltage();
//			status_1.BuoyBatt = pmGetBatteryVoltage();	
//printf("DeviceBatt =%6.2f \n",status_1.DeviceBatt);			
//printf("temperature=%6.2f \n",sensorData.baro.temperature);			
			sendStatus1(status_1);	
		}
		break;
				
	}
	if(count_ms++>=2) 		
		count_ms = 0;	
	
}

static u8 atkpCheckSum(atkp_t *packet)
{
	u8 sum;
	sum = DOWN_BYTE1;
	sum += DOWN_BYTE2;
	sum += packet->msgID;
	sum += packet->dataLen;
	for(int i=0; i<packet->dataLen; i++)
	{
		sum += packet->data[i];
	}
	return sum;
}

//static void atkpReceiveAnl(atkp_t *anlPacket)
void atkpReceiveAnl(atkp_t *anlPacket)
{
	if(anlPacket->msgID	== DOWN_COMMAND)
	{
		switch(anlPacket->data[0])
		{
			case D_COMMAND_VERSION_INFO:
			{
				break;
			}
			
			case D_COMMAND_DEFAULT_PARAMETER:
				break;
			
			case D_COMMAND_ACC_CALIB:				
				break;
			
			case D_COMMAND_GYRO_CALIB:			
				break;
			
			case D_COMMAND_MAG_CALIB:			
				break;
		}
		if(anlPacket->data[0] != D_COMMAND_VERSION_INFO)
		{
			u8 cksum = atkpCheckSum(anlPacket);
			sendCheck(anlPacket->msgID,cksum);	
		}

	}			
	else if(anlPacket->msgID == DOWN_COMMAND1)
	{	
		switch(anlPacket->data[0])
		{
			case D_COMMAND1_DEVICE_LOCK:
				IOSwitch |= DEVICE_LOCK;
				break;
			
			case D_COMMAND1_DEVICE_UNLOCK:
				IOSwitch &= (~DEVICE_LOCK);				
				break;
			
			case D_COMMAND1_DEEP_LOCK:
				IOSwitch |= DEEP_LOCK;				
				break;
			
			case D_COMMAND1_DEEP_UNLOCK:
				IOSwitch &= (~DEEP_LOCK);						
				break;
			
		}
		u8 cksum = atkpCheckSum(anlPacket);
		sendCheck(anlPacket->msgID,cksum);		

	}
	else if(anlPacket->msgID == DOWN_LED_BRIGHTNESS)
	{
				
	}
	else if(anlPacket->msgID == DOWN_MOTO_SPEED)	
	{

	}
	else if(anlPacket->msgID == DOWN_UPDOWN_REMOTE)	
	{

	}	
	else if(anlPacket->msgID == DOWN_DIRECTION_REMOTE)	
	{

	}	
	
} 

