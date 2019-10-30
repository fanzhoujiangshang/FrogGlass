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
 * ATKflight�ɿع̼�
 * ���ٶȼ���������	
 * ��������:2018/5/2
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * All rights reserved
********************************************************************************/

//������׵�ͨ�˲���������λHz��
#define ACCEL_LPF_CUTOFF_FREQ 	15.0f

//���������붨��Ļ�������ϵ�İ�װ����
#define ACCEL_ALIGN		CW270_DEG

//mpu6000��ʼ�����ٶ�����ΪFSR_8G��ACC_1G_ADC = 65536 / (2 * 8) = 4096
#define ACC_1G_ADC	4096.0f 

typedef struct accCalibration_s
{
	int32_t accSamples[6][3];
	int calibratedAxisCount;
	bool calibratedAxis[6];
	uint16_t cycleCount;
} accCalibration_t;


Axis3i16 accADCRaw;		//���ٶ�ԭʼAD����
Axis3i16 accADC;		//У׼���AD����			
Axis3f accf;			//ת����λΪG������

static accCalibration_t accCalibration;	//���ٶ�У׼�ṹ�����

biquadFilter_t accFilterLPF[XYZ_AXIS_COUNT];//���׵�ͨ�˲���

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

//��ȡ����������������������ݼ��ٶ�ֵ����
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

//ִ������У׼


