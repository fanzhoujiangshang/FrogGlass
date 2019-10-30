#ifndef __KEY_TASK_H
#define __KEY_TASK_H	 
#include "sys.h"
#include "key.h"
#include "keytask.h"

/********************************************************************************	 
 * 按键任务驱动代码	
 * 创建日期:2018/6/1
 * 版本：V1.0
 * 版权所有，盗版必究。
 * All rights reserved
********************************************************************************/

//按键状态
#define KEY_C_SHORT_PRESS	1	
#define KEY_C_LONG_PRESS	2
#define KEY_B_SHORT_PRESS	3	
#define KEY_B_LONG_PRESS	4	
#define KEY_A_SHORT_PRESS	5	
#define KEY_A_LONG_PRESS	6

#define	KEY_HANDLER_NEW		1
enum
{
	KEY_ALL_RELEASED,
	KEY_CC_SHORT_PRESS,
	KEY_CC_LONG_PRESS,
	KEY_CC_DOUBLE_PRESS,
	KEY_CC_LONGLONG_PRESS,
	KEY_BB_SHORT_PRESS,
	KEY_BB_LONG_PRESS,	
	KEY_BB_DOUBLE_PRESS,
	KEY_BB_LONGLONG_PRESS,	
	KEY_AA_SHORT_PRESS,	
	KEY_AA_LONG_PRESS,
	KEY_AA_DOUBLE_PRESS,
	KEY_AA_LONGLONG_PRESS,
	KEY_RESERVED
};
void keyTask(void* param);
u8 getKeyStateNew(void);
u8 getKeyState(void);
u32 getKeyTime(void);


#endif



