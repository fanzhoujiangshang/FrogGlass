#include <math.h>
#include "compass.h"
//#include "hmc5883l.h"
#include "mpu9250.h"
#include "sensorsalignment.h"
#include "storageTask.h"
//#include "nvic.h"
#include "maths.h"
#include "axis.h"
#include "FreeRTOS.h"
#include "task.h"

/*���������붨��Ļ�������ϵ�İ�װ����*/
#define MAG_ALIGN			CW270_DEG

#define MAG_CALIBRATION_TIME	30000 //3000ms


Axis3i16 magADCRaw;		//������ԭʼAD����
Axis3i16 magADC;		//У׼�󲢶����AD����

static bool isInit = false;
static uint8_t magUpdatedAtLeastOnce = 0;
static uint32_t calStartTime = 0;
static Axis3i16 magPrev;
static sensorCalibrationState_t calState;

bool compassInit(void)
{
//	isInit = hmc5883lInit();
	  isInit = true;
    return isInit;
}

bool compassIsHealthy(void)
{
    return (magADC.x != 0) || (magADC.y != 0) || (magADC.z != 0);
}

bool compassIsReady(void)
{
    return magUpdatedAtLeastOnce;
}

void compassSetCalibrationStart(void)
{
	if (isInit)
	{
		calStartTime = xTaskGetTickCount();

		magPrev.x = 0;
		magPrev.y = 0;
		magPrev.z = 0;

//		beeper(BEEPER_ACTION_SUCCESS);
		sensorCalibrationResetState(&calState);
	}
}


