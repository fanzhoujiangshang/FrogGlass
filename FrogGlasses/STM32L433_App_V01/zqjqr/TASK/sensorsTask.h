#ifndef __SENSORS_TASK_H
#define __SENSORS_TASK_H
#include "stabilizer_types.h"


/********************************************************************************	 
 * ���������ƴ���	
 * ��������:2018/5/2
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * All rights reserved
********************************************************************************/

/*��������ȡ����Ƶ��*/
#define GYRO_UPDATE_RATE			RATE_500_HZ
#define ACC_UPDATE_RATE			RATE_500_HZ
#define MAG_UPDATE_RATE			RATE_1000_HZ
#define BARO_UPDATE_RATE			RATE_10_HZ
#define CHARGE_UPDATE_RATE		RATE_10_HZ

extern sensorData_t sensors;
extern state_t state;		/*������̬*/

void sensorsTask(void *param);
void sensorsInit(void);			/*��������ʼ��*/
void sensorsAcquire(sensorData_t *sensors, const u32 tick);/*��ȡ����������*/

bool sensorsIsMagPresent(void);

/* ������������������ */
bool sensorsReadGyro(Axis3f *gyro);
bool sensorsReadAcc(Axis3f *acc);
bool sensorsReadMag(Axis3f *mag);
bool sensorsReadBaro(pressure_data_t *baro);
#endif //__SENSORS_TASK_H
