#ifndef __MAIN_UI_H
#define __MAIN_UI_H
#include <stdint.h>
#include <stdbool.h>
#include "rtc.h"
#include "pmTask.h"
#include "oxygenTask.h"
#include "storageTask.h"


enum
{
	MAIN_UI_BAT_VALUE0,		// 0
	MAIN_UI_BAT_VALUE1,		// 5
	MAIN_UI_BAT_VALUE2,		// 10
	MAIN_UI_BAT_VALUE3,		// 15
	MAIN_UI_BAT_VALUE4,		// 20
	MAIN_UI_BAT_VALUE5,		// 25
};
#define	BAT_CHARGE_STATE_MASK			(0x30)
#define	BAT_CHARGE_STATE_CHECK			(0x20)

void main_ui(void);
void Main_UI_ShowBattery(uint16_t dispX,uint16_t dispY,uint16_t bat_Value,uint8_t bat_Status);
#endif /*__MAIN_UI_H*/
