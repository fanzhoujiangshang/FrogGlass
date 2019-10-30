/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BQ24295_H
#define __BQ24295_H

#include "sys.h"  
#include "usr_hardware.h"

#define POWER_CTRL_WRITE_ADDR 0xD6
#define POWER_CTRL_READ_ADDR	0xD7
#define POWER_CTRL_REG00			0x00
#define POWER_CTRL_REG01			0x01
#define POWER_CTRL_REG02			0x02
#define POWER_CTRL_REG03			0x03
#define POWER_CTRL_REG04			0x04
#define POWER_CTRL_REG05			0x05
	
#define POWER_CTRL_REG06			0x06
#define POWER_CTRL_REG07			0x07
#define POWER_CTRL_REG08			0x08
#define POWER_CTRL_REG09			0x09
#define POWER_CTRL_REG0A			0x0a

uint8_t write_power_ctrl_reg(uint8_t reg, uint8_t data);
void read_power_ctrl_all_reg(void);
	
#endif /* __BQ24295_H */




























