#include "barometer.h"
#include "MS5837.h"

/********************************************************************************	 
 * �����������	
 * ��������:2018/5/2
 * �汾��V1.2
 * ��Ȩ���У�����ؾ���
 * All rights reserved
********************************************************************************/
static bool isInit = false;
bool baroInit(void)
{
//	return drv_pressure_meas_ms5837_init();
	 ms5837_init();
	 isInit = true;
   	 return isInit;
}
void baroUpdate(pressure_data_t *baro)
{
	drv_pressure_meas_ms5837_read(baro);
}

