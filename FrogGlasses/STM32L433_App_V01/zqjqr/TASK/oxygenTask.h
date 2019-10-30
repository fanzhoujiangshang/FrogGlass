#ifndef __OXYGEN_TASK_H
#define __OXYGEN_TASK_H
#include "sys.h"
#include <stdbool.h>
/********************************************************************************	 
 * 创建日期:2018/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

typedef enum {
    OXYGEN_OK = 0,
    OXYGEN_FULL,	
    OXYGEN_WARNING,
    OXYGEN_CRITICAL,
    OXYGEN_NOT_PRESENT
} OxygenState_e;

extern uint16_t voxygen;
extern uint16_t voxygenLatestADC;
extern uint16_t oxygenCriticalVoltage;
extern uint16_t oxygenWarningVoltage;

extern bool oxygenFullWhenPluggedIn;
extern OxygenState_e oxygenState;

uint16_t oxygenAdcToVoltage(uint16_t src);
OxygenState_e getOxygenState(void);
void oxygenInit(void);
void oxygenTask(void);	
float GetOxygenVoltage(void);

#endif /* __OXYGEN_TASK_H */
