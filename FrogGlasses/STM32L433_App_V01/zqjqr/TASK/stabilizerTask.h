#ifndef __STABALIZER_TASK_H
#define __STABALIZER_TASK_H
#include <stdbool.h>
#include <stdint.h>
#include "stabilizer_types.h"

/********************************************************************************	 
 * �������ȿ��ƴ���	
 * ��������:2018/5/2
 * �汾��V1.0
 * ��Ȩ���У�����ؾ���
 * All rights reserved
********************************************************************************/

#define MAIN_LOOP_RATE 			RATE_500_HZ				//��ѭ������
#define MAIN_LOOP_DT			(1.0/MAIN_LOOP_RATE)	

#define ATTITUDE_ESTIMAT_RATE	RATE_500_HZ				//��̬��������
#define ATTITUDE_ESTIMAT_DT		(1.0/ATTITUDE_ESTIMAT_RATE)

#define POSITION_ESTIMAT_RATE	ATTITUDE_ESTIMAT_RATE	//λ��Ԥ�����ʣ�����̬��������һ�£�
#define POSITION_ESTIMAT_DT		(1.0/POSITION_ESTIMAT_RATE)

#define RATE_PID_RATE			MAIN_LOOP_RATE 			//���ٶȻ�PID���ʣ�����ѭ������һ�£�
#define RATE_PID_DT				(1.0/RATE_PID_RATE)

#define ANGLE_PID_RATE			ATTITUDE_ESTIMAT_RATE 	//�ǶȻ�PID���ʣ�����̬��������һ�£�
#define ANGLE_PID_DT			(1.0/ANGLE_PID_RATE)

#define VELOCITY_PID_RATE		POSITION_ESTIMAT_RATE 	//λ���ٶȻ�PID���ʣ���λ��Ԥ������һ�£�
#define VELOCITY_PID_DT			(1.0/VELOCITY_PID_RATE)

#define POSITION_PID_RATE		POSITION_ESTIMAT_RATE 	//λ�û�PID���ʣ���λ��Ԥ������һ�£�
#define POSITION_PID_DT			(1.0/POSITION_PID_RATE)

extern setpoint_t		setpoint;	/*����Ŀ��״̬*/
extern sensorData_t 	sensorData;	/*����������*/
extern state_t 			state;		/*������̬*/
extern control_t 		control;	/*������Ʋ���*/

void stabilizerInit(void);
void stabilizerTask(void* param);

#endif /* __STABALIZER_TASK_H */