
#include <stdbool.h>
#include "keyTask.h"
#include <stdio.h>
//#include "displaytask.h"
/*FreeRtos includes*/
#include "FreeRTOS.h"
#include "task.h"
#include "Gpio_control.h"

/********************************************************************************	 
 * 按键任务驱动代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/


#define LONG_PRESS_COUNT 			500	/*判断为长按时间（ms）*/
#define SHORT_RELEASE_COUNT 		900	/*判断为短按release时间（ms）*/

#define KEY_EXIST_MASK 				0x03
#define KEY_EXIST_CHECK 			0x01
#define KEY_LONGPRESS_CHECK 		10

#define PRESSED		0
#define RELEASED	1

static u8 keyState;
static bool keyWaitForReleaseLL;
 u32 Key_A_Sample;
 u32 Key_B_Sample;
 u32 Key_C_Sample;

u8 KeyExist(u32 Sample)
{
	u32 sampleCmp;
	u8 cmpareBuf;
	u8 pressCnt;

	cmpareBuf = 0;
	sampleCmp = 0x80000000;
	while(sampleCmp)
	{
		if(sampleCmp & Sample)
		{
			cmpareBuf |= 1;
		}
		if((cmpareBuf & KEY_EXIST_MASK) == KEY_EXIST_CHECK)
		{
			pressCnt++;
		}
		cmpareBuf <<= 1;		
		sampleCmp >>= 1;
	}
	return pressCnt;
}
u8 KeyReleased(u32 Sample)
{
	if(Sample & 0x000000ff)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}
u8 KeyLongLongPressed(u32 Sample)
{
	u32 tmpSample = ~Sample;
	if(tmpSample == 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
u8 KeyLongPressed(u32 Sample)
{
	u32 tmpSample = Sample;
	u8 tmpLoopCnt;
	u8 checkCnt;

	for(tmpLoopCnt = 0;tmpLoopCnt < 32;tmpLoopCnt++)
	{
		if((tmpSample & 0x00000001) == 0)
		{
			tmpSample>>= 1;
		}
		else
		{
			break;
		}
	}
	checkCnt = 0;
//	printf("key st1:%08x ;;;;;\r\n",tmpSample);
	for(tmpLoopCnt = 0;tmpLoopCnt < 32;tmpLoopCnt++)
	{
		if((tmpSample & 0x00000001) == 0)
		{
			break;
		}
		else
		{
			checkCnt++;
		}
		tmpSample>>= 1;
	}
//	printf("key st2:%08x ;;;;;\r\n",tmpSample);
//	printf("key long:%d ====\r\n",checkCnt);
	if(checkCnt > KEY_LONGPRESS_CHECK)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

void keyTask(void* param)
{
	u8 tmpPressTime;
	u8 tmpKeyState;

	Key_A_Sample = 0;
 	Key_B_Sample = 0;
 	Key_C_Sample = 0;
	keyWaitForReleaseLL = false;

	bool bBuzzer = false;
	
	while(1)
	{
	
		if(bBuzzer)
		{
			BUZZER(1);
			bBuzzer = false;
		}
		else
		{
			BUZZER(0);
			bBuzzer = true;
		}
	 
		

		vTaskDelay(80);	
		Key_A_Sample <<= 1;
		Key_B_Sample <<= 1;
		Key_C_Sample <<= 1;
		if(READ_KEY_A()==PRESSED)
		{
			Key_A_Sample |= 1;
		}
		if(READ_KEY_B()==PRESSED)
		{
			Key_B_Sample |= 1;
		}
		if(READ_KEY_C()==PRESSED)
		{
			Key_C_Sample |= 1;
		}

		if(keyWaitForReleaseLL)
		{
			if(KeyReleased(Key_A_Sample)&&KeyReleased(Key_B_Sample)&&KeyReleased(Key_C_Sample))
			{
				Key_A_Sample = 0x00;
				Key_B_Sample = 0x00;
				Key_C_Sample = 0x00;
				keyWaitForReleaseLL = false;
			}
		}
		tmpKeyState = 0;
		if(Key_A_Sample > 0)
		{
			tmpPressTime = KeyExist(Key_A_Sample);
			if(KeyReleased(Key_A_Sample))
			{
				switch(tmpPressTime)
				{
					case 0:
						tmpKeyState = KEY_ALL_RELEASED;
						break;
					case 1:
						if(KeyLongPressed(Key_A_Sample))
						{
							tmpKeyState = KEY_AA_LONG_PRESS;
						}
						else
						{
							tmpKeyState = KEY_AA_SHORT_PRESS;
						}
						break;
					default:
						tmpKeyState = KEY_AA_DOUBLE_PRESS;
						break;
				}
			}
			else
			{
				if(KeyLongLongPressed(Key_A_Sample))
				{
					tmpKeyState = KEY_AA_LONGLONG_PRESS;
					keyWaitForReleaseLL = true;
				}
			}
		}
		else if(Key_B_Sample > 0)
		{
			tmpPressTime = KeyExist(Key_B_Sample);
			if(KeyReleased(Key_B_Sample))
			{
				switch(tmpPressTime)
				{
					case 0:
						tmpKeyState = KEY_ALL_RELEASED;
						break;
					case 1:
						if(KeyLongPressed(Key_B_Sample))
						{
							tmpKeyState = KEY_BB_LONG_PRESS;
						}
						else
						{
							tmpKeyState = KEY_BB_SHORT_PRESS;
						}
						break;
					default:
						tmpKeyState = KEY_BB_DOUBLE_PRESS;
						break;
				}
			}
			else
			{
				if(KeyLongLongPressed(Key_B_Sample))
				{
					tmpKeyState = KEY_BB_LONGLONG_PRESS;
					keyWaitForReleaseLL = true;
				}
			}	
		}
		else if(Key_C_Sample > 0)
		{
			tmpPressTime = KeyExist(Key_C_Sample);
			if(KeyReleased(Key_C_Sample))
			{
				switch(tmpPressTime)
				{
					case 0:
						tmpKeyState = KEY_ALL_RELEASED;
						break;
					case 1:
						if(KeyLongPressed(Key_C_Sample))
						{
							tmpKeyState = KEY_CC_LONG_PRESS;
						}
						else
						{
							tmpKeyState = KEY_CC_SHORT_PRESS;
						}
						break;
					default:
						tmpKeyState = KEY_CC_DOUBLE_PRESS;
						break;
				}
			}
			else
			{
				if(KeyLongLongPressed(Key_C_Sample))
				{
					tmpKeyState = KEY_CC_LONGLONG_PRESS;
					keyWaitForReleaseLL = true;
				}
			}		
		}
		keyState = tmpKeyState;
		if(keyState > 0)
		{
	//		printf("key state:%d ====\r\n",keyState);
	//		printf("Smp state:%08x ==1==\r\n",Key_A_Sample);
	//		printf("Smp state:%08x ==2==\r\n",Key_B_Sample);
	//		printf("Smp state:%08x ==3==\r\n",Key_C_Sample);
			Key_A_Sample = 0x00;
			Key_B_Sample = 0x00;
			Key_C_Sample = 0x00;
		}
	//	keyState = 0;
//		displayTask();
	}
}

//读取按键状态
u8 getKeyStateNew(void)
{
	u8 temp;
	temp = keyState;
	keyState = 0;//读取按键之后清零
	if(temp > 0)
	{
		printf("key state:%d ++++\r\n",temp);
	}
	return temp;
}

//读取按键状态
u8 getKeyState(void)
{
#if KEY_HANDLER_NEW
	return 0;
#else
	u8 temp;
	temp = keyState;
	keyState = 0;//读取按键之后清零
	return temp;
#endif
}


u32 getKeyTime(void)
{
	return xTaskGetTickCount();
}


