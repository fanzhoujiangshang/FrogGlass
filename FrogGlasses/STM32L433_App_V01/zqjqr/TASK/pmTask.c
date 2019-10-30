#include "pmTask.h"
//#include "bat_adc.h"
#include "stdio.h"
#include "usr_hardware.h"

/********************************************************************************	 
 * 电源管理驱动代码	
 * 创建日期:2018/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

#define ADCVREF 3300                 //单位mV (3300 = 3.3V)
#define BATTERY_SCALE	1000		 //1100 = 11:1 voltage divider (10k:1k)
#define VOLTAGE_CELLMAX 424			 //单节电池最高电压4.24V
#define VOLTAGE_CELLMIN	330			 //单节电池最高电压3.3V
#define VOLTAGE_CELLWARNING	350		 //单节低压报警值3.5V

#define VBATT_CELL_FULL_MAX_DIFF 14  // Max difference with cell max voltage for the battery to be considered full (10mV steps)
#define VBATT_PRESENT_THRESHOLD 100  // Minimum voltage to consider battery present
#define VBATT_HYSTERESIS 10          // Batt Hysteresis of +/-100mV for changing battery state


uint8_t batteryCellCount = 3;       //默认电池节数
uint16_t batteryFullVoltage;
uint16_t batteryWarningVoltage;
uint16_t batteryCriticalVoltage;
bool batteryFullWhenPluggedIn = false;

uint16_t vbat = 0;                  // battery voltage in 0.1V steps (filtered)
uint16_t vbatLatestADC = 0;         // most recent unsmoothed raw reading from vbat ADC



batteryState_e batteryState;

void pmInit(void)
{
//	adc1Init();	
    batteryState = BATTERY_NOT_PRESENT;
    batteryCellCount = 1;
    batteryFullVoltage = 0;
    batteryWarningVoltage = 0;
    batteryCriticalVoltage = 0;
}

uint16_t batteryAdcToVoltage(uint16_t src)
{
    // result is Vbatt in 0.01V steps. 3.3V = ADC Vref, 0xFFF = 12bit adc, 1100 = 11:1 voltage divider (10k:1k)
    return((uint64_t)src * BATTERY_SCALE * ADCVREF / (0xFFF * 1000));
//	return((uint64_t)src * ADCVREF / (0xFFF * 1000));
}

static void updateBatteryVoltage(float vbatTimeDelta)
{
	uint16_t vbatSample;

	get_adc_value (&vbatSample,ADC1_PORT_NUM1);
	vbatLatestADC = vbatSample;
	vbat = batteryAdcToVoltage(vbatSample);
	printf("vbatLatestADC=%d ,vbat=%d \n",vbatLatestADC,vbat);			
}


/* 电源管理任务 */
void pmTask(void)	
{
//	while(1)
	{
//		aos_msleep(100);//100ms
		updateBatteryVoltage(0.1);
		
		/* battery has just been connected*/
		if (batteryState == BATTERY_NOT_PRESENT && vbat > VBATT_PRESENT_THRESHOLD)
		{
			/* Actual battery state is calculated below, this is really BATTERY_PRESENT */
			batteryState = BATTERY_OK;
			/* wait for VBatt to stabilise then we can calc number of cells
			(using the filtered value takes a long time to ramp up)
			We only do this on the ground so don't care if we do block, not
			worse than original code anyway*/
			updateBatteryVoltage(0.1);

			unsigned cells = (batteryAdcToVoltage(vbatLatestADC) / VOLTAGE_CELLMAX) + 1;
			if (cells > 8) cells = 8; // something is wrong, we expect 8 cells maximum (and autodetection will be problematic at 6+ cells)

			batteryCellCount = cells;
			batteryFullVoltage = batteryCellCount * VOLTAGE_CELLMAX;
			batteryWarningVoltage = batteryCellCount * VOLTAGE_CELLWARNING;
			batteryCriticalVoltage = batteryCellCount * VOLTAGE_CELLMIN;

 			printf("full=%d ,warn=%d ,crital=%d \n",batteryFullVoltage,batteryWarningVoltage,batteryCriticalVoltage);
	
			batteryFullWhenPluggedIn = batteryAdcToVoltage(vbatLatestADC) >= (batteryFullVoltage - cells * VBATT_CELL_FULL_MAX_DIFF);
		}
		/* battery has been disconnected - can take a while for filter cap to disharge so we use a threshold of VBATT_PRESENT_THRESHOLD */
		else if (batteryState != BATTERY_NOT_PRESENT && vbat <= VBATT_PRESENT_THRESHOLD) 
		{
			batteryState = BATTERY_NOT_PRESENT;
			batteryCellCount = 0;
			batteryWarningVoltage = 0;
			batteryCriticalVoltage = 0;
		}

		if (batteryState != BATTERY_NOT_PRESENT) 
		{
			switch (batteryState)
			{
				case BATTERY_FULL:
					if (vbat <= (batteryFullVoltage - VBATT_HYSTERESIS))
					{
						batteryState = BATTERY_OK;
					}			
				case BATTERY_OK:
					if (vbat <= (batteryWarningVoltage - VBATT_HYSTERESIS))
					{
						batteryState = BATTERY_WARNING;
					}
					else if(batteryFullVoltage + VBATT_HYSTERESIS )
					{
						batteryState = BATTERY_FULL;
					}
					break;
				case BATTERY_WARNING:
					if (vbat <= (batteryCriticalVoltage - VBATT_HYSTERESIS)) 
					{
						batteryState = BATTERY_CRITICAL;
					} else if (vbat > (batteryWarningVoltage + VBATT_HYSTERESIS))
					{
						batteryState = BATTERY_OK;
					}
					break;
				case BATTERY_CRITICAL:
					if (vbat > (batteryCriticalVoltage + VBATT_HYSTERESIS))
						batteryState = BATTERY_WARNING;
					break;
				default:
					break;
			}
		}

		// handle beeper
		switch (batteryState) 
		{
			case BATTERY_WARNING:
//				beeper(BEEPER_BAT_LOW);
				break;
			case BATTERY_CRITICAL:
//				beeper(BEEPER_BAT_CRIT_LOW);
				break;
			default:
				break;
		}
	}
}

float pmGetBatteryVoltage(void)
{
	return ((float)vbat/100);
}

batteryState_e getBatteryState(void)
{
	return batteryState;
}


