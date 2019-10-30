#include "stabilizerTask.h"
#include "sensorsTask.h"
//#include "mpuTask.h"
#include "gyro.h"
#include "imu.h"
#include <stdio.h>

/*FreeRTOS相关头文件*/
#include "FreeRTOS.h"
#include "task.h"

/********************************************************************************	 
 * 四轴自稳控制代码	
 * 创建日期:2018/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

setpoint_t		setpoint;	/*设置目标状态*/
sensorData_t 	sensorData;	/*传感器数据*/
state_t 		state;		/*四轴姿态*/
control_t 		control;	/*四轴控制参数*/

void stabilizerInit(void)
{
	imuInit();				/*姿态解算初始化*/
}
#if 1
void stabilizerTask(void* param)
{
	u32 tick = 0;
	u32 lastWakeTime = xTaskGetTickCount();
	
	//等待陀螺仪校准完成
	while(!gyroIsCalibrationComplete())
	{
		vTaskDelayUntil(&lastWakeTime, M2T(1));
		printf("stabilizerTask \n");			
	}
//	while(!sensorsAreCalibrated())
//	{
//		vTaskDelayUntil(&lastWakeTime, MAIN_LOOP_DT);
//	}
	
	while(1) 
	{
//	printf("stabilizerTask \n");
		//1KHz运行频率
		vTaskDelayUntil(&lastWakeTime, F2T(RATE_100_HZ));	
//		vTaskDelayUntil(&lastWakeTime, RATE_1000_HZ);	
		
		//获取传感器数据
		if (RATE_DO_EXECUTE(MAIN_LOOP_RATE, tick))
		{
			sensorsAcquire(&sensorData, tick);	
//	printf("gyro_x=%.1f ,gyro_y=%.1f ,gyro_z=%.1f \n",sensorData.gyro.x,sensorData.gyro.y,sensorData.gyro.z);	
//	printf("acc_x=%.1f ,acc_y=%.1f ,acc_z=%.1f \n",sensorData.acc.x,sensorData.acc.y,sensorData.acc.z);	
//	printf("mag_x=%.1f ,mag_y=%.1f ,mag_z=%.1f \n",sensorData.mag.x,sensorData.mag.y,sensorData.mag.z);	
//	printf("pressure=%.1f ,depth=%.1f ,temperature=%.1f \n",sensorData.baro.pressure,sensorData.baro.depth,sensorData.baro.temperature);				
		}
		
		//四元数和欧拉角计算
		if (RATE_DO_EXECUTE(ATTITUDE_ESTIMAT_RATE, tick))
		{
			imuUpdateAttitude(&sensorData, &state, ATTITUDE_ESTIMAT_DT);		
		printf("roll=%6.2f ,pitch=%6.2f ,yaw=%6.2f \n",state.attitude.roll,state.attitude.pitch,state.attitude.yaw);						
		}
		
		tick++;
	}
}
#endif
