#include <math.h>
#include "accelerometer.h"
//#include "mpu6000.h"
#include "mpu9250.h"
#include "axis.h"
#include "maths.h"
#include "storageTask.h"
#include "filter.h"
#include "sensorsTask.h"
#include "sensorsalignment.h"

/********************************************************************************	 
 * ATKflight飞控固件
 * 加速度计驱动代码	
 * 创建日期:2018/5/2
 * 版本：V1.2
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

//软件二阶低通滤波参数（单位Hz）
#define ACCEL_LPF_CUTOFF_FREQ 	15.0f

//传感器对齐定义的机体坐标系的安装方向
#define ACCEL_ALIGN		CW270_DEG

//mpu6000初始化加速度量程为FSR_8G，ACC_1G_ADC = 65536 / (2 * 8) = 4096
#define ACC_1G_ADC	4096.0f 

typedef struct accCalibration_s
{
	int32_t accSamples[6][3];
	int calibratedAxisCount;
	bool calibratedAxis[6];
	uint16_t cycleCount;
} accCalibration_t;


Axis3i16 accADCRaw;		//加速度原始AD数据
Axis3i16 accADC;		//校准后的AD数据			
Axis3f accf;			//转换单位为G的数据

static accCalibration_t accCalibration;	//加速度校准结构体参数

biquadFilter_t accFilterLPF[XYZ_AXIS_COUNT];//二阶低通滤波器

void accSetCalibrationCycles(uint16_t calibrationCyclesRequired)
{
    accCalibration.cycleCount = calibrationCyclesRequired;
}

bool accIsCalibrationComplete(void)
{
    return accCalibration.cycleCount == 0;
}

static bool isOnFinalAccelerationCalibrationCycle(void)
{
    return accCalibration.cycleCount == 1;
}

static bool isOnFirstAccelerationCalibrationCycle(void)
{
    return accCalibration.cycleCount == CALIBRATING_ACC_CYCLES;
}

//获取六面索引，六面的索引根据加速度值计算
static int getPrimaryAxisIndex(Axis3i16 accADCSample)
{
    // Tolerate up to atan(1 / 1.5) = 33 deg tilt (in worst case 66 deg separation between points)
    if ((ABS(accADCSample.z) / 1.5f) > ABS(accADCSample.x) && (ABS(accADCSample.z) / 1.5f) > ABS(accADCSample.y)) 
	{
        //Z-axis
        return (accADCSample.z > 0) ? 0 : 1;
    }
    else if ((ABS(accADCSample.x) / 1.5f) > ABS(accADCSample.y) && (ABS(accADCSample.x) / 1.5f) > ABS(accADCSample.z)) 
	{
        //X-axis
        return (accADCSample.x > 0) ? 2 : 3;
    }
    else if ((ABS(accADCSample.y) / 1.5f) > ABS(accADCSample.x) && (ABS(accADCSample.y) / 1.5f) > ABS(accADCSample.z)) 
	{
        //Y-axis
        return (accADCSample.y > 0) ? 4 : 5;
    }
    else
        return -1;
}

//执行六面校准


