#include "HARDWARE/POWER_CTRL/BQ24295.h"

static uint8_t read_power_ctrl_reg(uint8_t reg)
{
	
	uint8_t buf;
	
	if ( usr_i2c_write_bytes(1, POWER_CTRL_WRITE_ADDR, &reg, 1, 1000) != HAL_OK ) {
		printf("usr_i2c_write_bytes error!\r\n");
	}

	
	if ( usr_i2c_read_bytes(1, POWER_CTRL_READ_ADDR, &buf, 1, 1000) != HAL_OK ) {
		printf("usr_i2c_read_bytes error!\r\n");
	}
	
	return buf;
}

uint8_t write_power_ctrl_reg(uint8_t reg, uint8_t data)
{
	uint8_t ret = 1;
	uint8_t buf[2];
	
	buf[0] = reg;
	buf[1] = data;
	
	ret = usr_i2c_write_bytes(1, POWER_CTRL_WRITE_ADDR, buf, 2, 1000);
	
	return ret;	
}

void read_power_ctrl_all_reg(void)
{
	uint8_t buffer[10];
	uint8_t i;
	
	for (i=0; i<10; i++) {
		buffer[i] = read_power_ctrl_reg(i);
		printf("reg:%d\tdata:%02x\r\n", i, buffer[i]);
	}
	printf("\r\n\r\n");
}

