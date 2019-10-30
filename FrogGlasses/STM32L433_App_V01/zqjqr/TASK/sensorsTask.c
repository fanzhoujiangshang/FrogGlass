#include <math.h>
#include "stdio.h"
#include "storageTask.h"
#include "sensorsTask.h"
#include "stabilizerTask.h"
#include "stabilizer_types.h"
#include "imu.h"
#include "gyro.h"
#include "accelerometer.h"
#include "compass.h"
#include "barometer.h"
#include "sensorsalignment.h"
#include "displaytask.h"
#include "MS5837.h"
#include "atkp.h"

/*FreeRTOS相关头文件*/
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"
#include "bat_adc.h"

/********************************************************************************	 
 * 传感器控制代码	
 * 创建日期:2018/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

sensorData_t sensors;
state_t state;		/*四轴姿态*/


/* 传感器器件初始化 */
void sensorsInit(void)
{	
	IMU_Init();
	ms5837_init();  
	BAT_I2C_Init();
}

/*传感器任务*/
void sensorsTask(void *param)
{	
	u32 tick = 0;
	u32 lastWakeTime = xTaskGetTickCount();
	sensorsInit();		//传感器初始化

	while (1)
	{
//	printf("sensorsTask \n");

		vTaskDelayUntil(&lastWakeTime, F2T(RATE_100_HZ));		//1KHz运行频率
		if (RATE_DO_EXECUTE(GYRO_UPDATE_RATE, tick))
		{
			IMU_GetYawPitchRoll(&state) ;
			//printf("roll=%6.2f ,pitch=%6.2f ,yaw=%6.2f \r\n",state.attitude.roll,state.attitude.pitch,state.attitude.yaw);	
			//printf("r:%6.2f p:%6.2f y:%6.2f \r\n",state.attitude.roll,state.attitude.pitch,state.attitude.yaw);	
		}		
		if (RATE_DO_EXECUTE(BARO_UPDATE_RATE, tick))
		{
			baroUpdate(&sensors.baro);	
//			printf("depth=%6.2f ,temperature=%6.2f \n",sensors.baro.depth,sensors.baro.temperature);	
		}
/*		if (RATE_DO_EXECUTE(MAG_UPDATE_RATE, tick))
		{
			atkpSendPeriod();
		}*/
		if (RATE_DO_EXECUTE(CHARGE_UPDATE_RATE, tick))
		{
			u8 tmpU8;
			BQ24295ReadByte(0x08,&tmpU8);		// read charge status register
			set_display_data_no_bat_status(tmpU8);
		}
		tick++;
	}	
}


