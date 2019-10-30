#ifndef _ACTION_H_
#define	_ACTION_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "app\sys.h"

#define	TM_EVENT_100MS				(0x0001)
#define	TM_EVENT_200MS				(0x0002)
#define	TM_EVENT_500MS				(0x0004)
#define	TM_EVENT_1SECOND				(0x0008)

#define	APPLICATION_ADDRESS			(0x8000000 + 0x10000)
typedef  void (*pFunction)(void);

enum
{
	MTT_IDLE,		// power on
	MTT_TST,		// test
	MTT_MAC,		// get mac
	MTT_REN,		// rename
	MTT_ADV,		// adv on,off
	MTT_CHK,		// wait for server ack info
	MTT_PASS		// pass through
};
#define	BT_WAIT_TIMES					(3)
void Jump_Main_App(void);
void Jump_Main_App2(void);
void BT_Handler(void);
void User_SysTick_Handler(void);
void User_SysTick_Handler(void);
u16 Get_TM_Flag(void);
void Clr_TM_Flag(u16 Flag);
void SelfDefineValueInit(void);
#ifdef __cplusplus
}
#endif

#endif

