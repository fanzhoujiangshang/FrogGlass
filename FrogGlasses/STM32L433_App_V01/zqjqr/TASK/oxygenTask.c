#include "oxygenTask.h"
//#include "bat_adc.h"
#include "stdio.h"
#include "usr_hardware.h"

/********************************************************************************	 
 * 氧气管理驱动代码	
 * 创建日期:2018/5/2
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

#define OXYGENVREF 3300                 //单位mV (3300 = 3.3V)
#define BOXYGEN_SCALE	1000		 //1100 = 11:1 voltage divider (10k:1k)
#define VOLTAGE_FULL 424			 
#define VOLTAGE_CRITICAL	330			
#define VOLTAGE_WARNING	350		 

#define VOXYGEN_CELL_FULL_MAX_DIFF 14  // Max difference with cell max voltage for the battery to be considered full (10mV steps)
#define VOXYGEN_PRESENT_THRESHOLD 100  // Minimum voltage to consider battery present
#define VOXYGEN_HYSTERESIS 10          // Batt Hysteresis of +/-100mV for changing battery state

uint16_t oxygenFullVoltage;
uint16_t oxygenWarningVoltage;
uint16_t oxygenCriticalVoltage;
bool oxygenFullWhenPluggedIn = false;

uint16_t voxygen = 0;                  // battery voltage in 0.1V steps (filtered)
uint16_t voxygenLatestADC = 0;         // most recent unsmoothed raw reading from vbat ADC



OxygenState_e oxygenState;

void oxygenInit(void)
{
//	adc1Init();	
    oxygenState = OXYGEN_NOT_PRESENT;
    oxygenFullVoltage = 0;
    oxygenWarningVoltage = 0;
    oxygenCriticalVoltage = 0;
}

uint16_t oxygenAdcToVoltage(uint16_t src)
{
    // result is Vbatt in 0.01V steps. 3.3V = ADC Vref, 0xFFF = 12bit adc, 1100 = 11:1 voltage divider (10k:1k)
    return((uint64_t)src * BOXYGEN_SCALE * OXYGENVREF / (0xFFF * 1000));
//	return((uint64_t)src * ADCVREF / (0xFFF * 1000));
}

static void updateOxygenVoltage(float vbatTimeDelta)
{
    uint16_t voxygenSample;

	get_adc_value (&voxygenSample,ADC1_PORT_NUM2);
	voxygenLatestADC = voxygenSample;
	voxygen = oxygenAdcToVoltage(voxygenSample);
	printf("voxygenLatestADC=%d ,voxygen=%d \n",voxygenLatestADC,voxygen);			
}


/* 氧气管理任务 */
void oxygenTask(void)	
{
//	while(1)
	{
//		aos_msleep(100);//100ms
		updateOxygenVoltage(0.1);
		
		/* oxygen has just been connected*/
		if (oxygenState == OXYGEN_NOT_PRESENT && voxygen > VOXYGEN_PRESENT_THRESHOLD)
		{
			/* Actual oxygen state is calculated below, this is really OXYGEN_PRESENT */
			oxygenState = OXYGEN_OK;
			
			oxygenFullVoltage = VOLTAGE_FULL;
			oxygenWarningVoltage = VOLTAGE_WARNING;
			oxygenCriticalVoltage =  VOLTAGE_CRITICAL;
			
			oxygenFullWhenPluggedIn = oxygenAdcToVoltage(voxygenLatestADC) >= (oxygenFullVoltage - VOXYGEN_CELL_FULL_MAX_DIFF);
		}
		/* oxygenhas been disconnected - can take a while for filter cap to disharge so we use a threshold of VOXYGEN_PRESENT_THRESHOLD */
		else if (oxygenState != OXYGEN_NOT_PRESENT && voxygen <= VOXYGEN_PRESENT_THRESHOLD) 
		{
			oxygenState = OXYGEN_NOT_PRESENT;
			oxygenFullVoltage = 0;
    			oxygenWarningVoltage = 0;
    			oxygenCriticalVoltage = 0;
		}

		if (oxygenState != OXYGEN_NOT_PRESENT) 
		{
			switch (oxygenState)
			{
				case OXYGEN_FULL:
					if (voxygen <= (oxygenFullVoltage - VOXYGEN_HYSTERESIS))
					{
						oxygenState = OXYGEN_OK;
					}
					break;			
				case OXYGEN_OK:
					if (voxygen <= (oxygenWarningVoltage - VOXYGEN_HYSTERESIS))
					{
						oxygenState = OXYGEN_WARNING;
					} else if (voxygen > (oxygenFullVoltage + VOXYGEN_HYSTERESIS))
					{
						oxygenState = OXYGEN_FULL;
					}
					break;
				case OXYGEN_WARNING:
					if (voxygen <= (oxygenCriticalVoltage - VOXYGEN_HYSTERESIS)) 
					{
						oxygenState = OXYGEN_CRITICAL;
					} else if (voxygen > (oxygenWarningVoltage + VOXYGEN_HYSTERESIS))
					{
						oxygenState = OXYGEN_OK;
					}
					break;
				case OXYGEN_CRITICAL:
					if (voxygen > (oxygenCriticalVoltage + VOXYGEN_HYSTERESIS))
						oxygenState = OXYGEN_WARNING;
					break;
				default:
					break;
			}
		}

		// handle beeper
		switch (oxygenState) 
		{
			case OXYGEN_WARNING:
//				beeper(BEEPER_BAT_LOW);
				break;
			case OXYGEN_CRITICAL:
//				beeper(BEEPER_BAT_CRIT_LOW);
				break;
			default:
				break;
		}
	}
}

float GetOxygenVoltage(void)
{
	return ((float)voxygen/100);
}

OxygenState_e getOxygenState(void)
{
	return oxygenState;
}


