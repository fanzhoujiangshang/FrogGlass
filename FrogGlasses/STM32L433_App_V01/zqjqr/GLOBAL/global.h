#ifndef __GLOBAL_H
#define __GLOBAL_H	 

#include <stdio.h>
#include "sys.h"
#include "stdbool.h"
#include "rtc.h"

typedef enum 
{
	DIVE_MODE_NONE,
	DIVE_MODE_AIR, 
	DIVE_MODE_MIX,
	DIVE_MODE_NIX,
} divemode;


extern u8 g_nCurrentDiveMode;
extern u8 g_nDiveMode_Tmp;
extern void InitGlobalData(void);

#endif
