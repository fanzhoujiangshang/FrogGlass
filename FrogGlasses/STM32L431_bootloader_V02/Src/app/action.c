#include "app\action.h"
#include "usart.h"
#include <string.h>

u8 m_TM_Flag;
u16 m_TM_CountUp;
u8 m_BT_WorkState;
u8 m_BT_Countdown;
u8 m_BT_Name[16];
u8 m_BT_SendBuffer[32];
pFunction Jump_To_Application;

void SelfDefineValueInit(void)
{
	m_TM_Flag = 0;
	m_TM_CountUp = 0;
	m_BT_WorkState = MTT_IDLE;
}
void Jump_Main_App(void)
{
	u32 JumpAddress;
	 /* Test if user code is programmed starting from address "APPLICATION_ADDRESS" */ 
	if(((*(vu32*)APPLICATION_ADDRESS)&0x2FFE0000)==0x20000000)//检查栈顶地址是否合法.
	{
		/* Jump to user application */ 
		JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS+4); // Jump to user application
		Jump_To_Application = (pFunction)JumpAddress;	
		__disable_irq();
		/* Initialize user application's Stack Pointer */ 
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		Jump_To_Application();
	}
}
void Jump_Main_App2(void)
{
	u32 JumpAddress;
	typedef void (*_func)(void);
	__disable_irq();
        /* reset systick */
        SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
        /* disable all peripherals clock. */
        RCC->AHB1ENR = (1<<20); /* 20: F4 CCMDAT ARAMEN. */
        RCC->AHB2ENR = 0;
        RCC->AHB3ENR = 0;
        RCC->APB1ENR2 = 0;
        RCC->APB2ENR = 0;
        /* Switch to default cpu clock. */
        RCC->CFGR = 0;
	/* Disable MPU */
	MPU->CTRL &= ~MPU_CTRL_ENABLE_Msk;	
    /* disable and clean up all interrupts. */
        int i;
        for(i = 0; i < 3; i++)
        {
            /* disable interrupts. */
            NVIC->ICER[i] = 0xFFFFFFFF;
            /* clean up interrupts flags. */
            NVIC->ICPR[i] = 0xFFFFFFFF;
        }	
	/* Set new vector table pointer */
	SCB->VTOR = APPLICATION_ADDRESS;		
    /* reset register values */
    __set_BASEPRI(0);
    __set_FAULTMASK(0);	
	 /* Test if user code is programmed starting from address "BOOTLOADER_ADDRESS" */ 
	if(((*(vu32*)APPLICATION_ADDRESS)&0x2FFE0000)==0x20000000)//检查栈顶地址是否合法.
	{
		/* Jump to user application */ 
		JumpAddress = *(__IO uint32_t *)(APPLICATION_ADDRESS+4); // Jump to user application
		Jump_To_Application = (pFunction)JumpAddress;	
		/* Initialize user application's Stack Pointer */ 
		__set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		__set_PSP(*(__IO uint32_t*) APPLICATION_ADDRESS);
		__set_CONTROL(0);
		/* ensure what we have done could take effect */
		__ISB();		
		__disable_irq();
		/* never return */
		((_func)(*(uint32_t*)(APPLICATION_ADDRESS + 4)))();
		//Jump_To_Application();
	}
}
u8 BT_CheckStrExist(u8 * str)
{
	pBT_DataStruct tmpPBT;	
	char* tmpPtr;
	char* headPtr;
	tmpPBT = get_BT_ReceiveData();
	if(tmpPBT->m_Index < 4)
	{
		return 0;
	}
	if(tmpPBT->m_Index != tmpPBT->m_Index_Old)
	{
		tmpPBT->m_Index_Old = tmpPBT->m_Index;
		return 0;
	}
	headPtr = (char*)tmpPBT->m_DataBuffer;
	headPtr[tmpPBT->m_Index] = 0;
	tmpPBT->m_Index 		= 0;
	tmpPBT->m_Index_Old 	= 0;
	tmpPtr = strstr(headPtr,(const char *)str);
	//printf("---%s---%s===%d+++\r\n",tmpPtr,headPtr,m_BT_WorkState);
	if(tmpPtr)
	{
		switch(m_BT_WorkState)
		{
			case MTT_MAC:
				//printf(",,,%d,,,%d,,,%d,,,%d,,,\r\n",tmpPtr,headPtr + 13,*(tmpPtr - 1),*(tmpPtr - 2));
				if((headPtr + 13 < tmpPtr)&&
					(*(tmpPtr - 1) == 0x0a)&&
					(*(tmpPtr - 2) == 0x0d))
				{
					tmpPtr-= 2;
					*tmpPtr = 0;
					sprintf((char*)m_BT_Name,"ZQJQR_%s",tmpPtr - 6);
				}
				break;
		}
		return 1;
	}
	return 0;
}

void BT_Handler(void)
{
	switch(m_BT_WorkState)
	{
		case MTT_IDLE:		// power on
			BT_USART3_SendData("TTM:TST-",8);
			m_BT_WorkState = MTT_TST;		
			m_BT_Countdown = 0;
			break;
		case MTT_TST:		// test
			if(BT_CheckStrExist("TTM:OK"))
			{
				BT_USART3_SendData("TTM:MAC-?",9);
				m_BT_WorkState = MTT_MAC;
				m_BT_Countdown = 0;
			}	
			else
			{
				m_BT_Countdown++;
				if(m_BT_Countdown > BT_WAIT_TIMES)
				{
					BT_USART3_SendData("TTM:TST-",8);
					m_BT_Countdown = 0;
				}
			}
			break;
		case MTT_MAC:		// get mac
			if(BT_CheckStrExist("TTM:OK"))
			{
				if(m_BT_Name[0] == 0)
				{
					BT_USART3_SendData("TTM:MAC-?",9);
					m_BT_Countdown = 0;
				}
				else
				{
					sprintf((char*)m_BT_SendBuffer,"TTM:REN-%s",m_BT_Name);
					printf("%s\r\n",m_BT_SendBuffer);
					BT_USART3_SendData(m_BT_SendBuffer,strlen((const char*)m_BT_SendBuffer));
					m_BT_WorkState = MTT_REN;	
					m_BT_Countdown = 0;
				}
			}	
			else
			{
				m_BT_Countdown++;
				if(m_BT_Countdown > BT_WAIT_TIMES)
				{
					BT_USART3_SendData("TTM:MAC-?",9);
					m_BT_Countdown = 0;
				}
			}
			break;
		case MTT_REN:		// rename
			if(BT_CheckStrExist("TTM:OK"))
			{
				BT_USART3_SendData("TTM:ADV-ON",10);
				m_BT_WorkState = MTT_ADV;
				m_BT_Countdown = 0;
			}	
			else
			{
				m_BT_Countdown++;
				if(m_BT_Countdown > BT_WAIT_TIMES)
				{
					BT_USART3_SendData(m_BT_SendBuffer,strlen((const char*)m_BT_SendBuffer));
					m_BT_Countdown = 0;
				}
			}		
			break;
		case MTT_ADV:		// adv on,off
			if(BT_CheckStrExist("TTM:OK"))
			{
				BT_USART3_SendData("ZQJQR_1234",10);
				m_BT_WorkState = MTT_CHK;
				m_BT_Countdown = 0;
			}	
			else
			{
				m_BT_Countdown++;
				if(m_BT_Countdown > BT_WAIT_TIMES)
				{
					BT_USART3_SendData("TTM:ADV-ON",10);
					m_BT_Countdown = 0;
				}
			}		
			break;
		case MTT_CHK:		// wait for server ack info
			if(BT_CheckStrExist("ZQJQR_5678"))
			{
				//BT_USART3_SendData("ZQJQR_1234",10);
				m_BT_WorkState = MTT_PASS;
				m_BT_Countdown = 0;
			}	
			else
			{
				m_BT_Countdown++;
				if(m_BT_Countdown > BT_WAIT_TIMES)
				{
					BT_USART3_SendData("ZQJQR_1234",10);
					m_BT_Countdown = 0;
				}
			}				
			break;
		case MTT_PASS:		// pass through
			//		can transmate any data
			break;
		default:
			break;
	}	
}

void User_SysTick_Handler(void)
{
	m_TM_CountUp++;
	if(m_TM_CountUp%100 == 0)
	{
		m_TM_Flag |= TM_EVENT_100MS;
	}
	if(m_TM_CountUp%200 == 0)
	{
		m_TM_Flag |= TM_EVENT_200MS;
	}
	if(m_TM_CountUp%500 == 0)
	{
		m_TM_Flag |= TM_EVENT_500MS;
	}
	if(m_TM_CountUp%1000 == 0)
	{
		m_TM_Flag |= TM_EVENT_1SECOND;
	}	
}

u16 Get_TM_Flag(void)
{
	return m_TM_Flag;
}
void Clr_TM_Flag(u16 Flag)
{
	m_TM_Flag &= (~Flag);
}

