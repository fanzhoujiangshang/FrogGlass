#ifndef __PM_TASK_H
#define __PM_TASK_H
#include "sys.h"
#include <stdbool.h>
/********************************************************************************	 
 * 创建日期:2018/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

#ifndef VBAT_SCALE_DEFAULT
#define VBAT_SCALE_DEFAULT 1100
#endif
#define VBAT_SCALE_MIN 0
#define VBAT_SCALE_MAX 65535

#ifndef CURRENT_METER_SCALE
#define CURRENT_METER_SCALE 400 // for Allegro ACS758LCB-100U (40mV/A)
#endif

typedef enum {
    BATTERY_OK = 0,
    BATTERY_FULL,		
    BATTERY_WARNING,
    BATTERY_CRITICAL,
    BATTERY_NOT_PRESENT
} batteryState_e;

extern uint16_t vbat;
extern uint16_t vbatRaw;
extern uint16_t vbatLatestADC;
extern uint8_t batteryCellCount;
extern uint16_t batteryCriticalVoltage;
extern uint16_t batteryWarningVoltage;

extern bool batteryFullWhenPluggedIn;
extern batteryState_e batteryState;

uint16_t batteryAdcToVoltage(uint16_t src);
batteryState_e getBatteryState(void);
void pmInit(void);
void pmTask(void);	
float pmGetBatteryVoltage(void);

#endif /* __PM_H */
