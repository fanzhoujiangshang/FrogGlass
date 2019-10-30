//#include <aos/aos.h>
#include <stdbool.h>
#include "rtcTask.h"
#include "rtc.h"
#include <stdio.h>
#include "FreeRTOS.h"
#include "task.h"
#include "displaytask.h"
/********************************************************************************	 
 * RTC任务驱动代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/
void rtcTask(void* param)
{
//	RTC_TimeTypeDef diving_time;	
	while(1)
	{
		vTaskDelay(800);	
	/* Get the RTC current Time ,must get time first*/
	       HAL_RTC_GetTime(&RTC_Handler, &stimestructure, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
		HAL_RTC_GetDate(&RTC_Handler, &sdatestructure, RTC_FORMAT_BIN);

//		set_display_data_zebra_time(stimestructure);
//		set_display_data_zebra_date(sdatestructure);
//		diving_time.Minutes = stimestructure.Minutes - init_stimestructure.Minutes;
//		diving_time.Seconds= stimestructure.Seconds- init_stimestructure.Seconds;
//		set_display_data_diving_time(diving_time);		
		 
	/* Display date Format : yy/mm/dd */
//		printf("%02d/%02d/%02d\r\n",2000 + sdatestructure.Year, sdatestructure.Month, sdatestructure.Date);
	/* Display time Format : hh:mm:ss */
//		printf("%02d:%02d:%02d\r\n",stimestructure.Hours, stimestructure.Minutes, stimestructure.Seconds);
//		printf("\r\n");
	}
}















