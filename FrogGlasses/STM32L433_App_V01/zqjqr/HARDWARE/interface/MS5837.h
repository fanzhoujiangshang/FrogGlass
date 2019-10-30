/*
 * Copyright (C) 2018-2019 RC-AUV.
 * All rights reserved.
 */

#ifndef _MS5837_H_
#define _MS5837_H_

#ifdef __cplusplus
	extern "C" {
#endif

#include "stm32l4xx_hal.h"
#include "sys.h"  

#define IO_MODULATE_I2C					(0)

typedef enum EDrvStatus
{
	DRV_SUCCESS =  0,
	DRV_FAILURE  = -1,
	
	DRV_RESET_FAILURE,
	DRV_TRANSMIT_FAILURE,
	DRV_RECIEVE_FAILURE,
	DRV_CRC_ERROR,
 
}DrvStatus;

#define MS5837_ADDR_WRITE         	0xEC
#define MS5837_ADDR_READ          	0xED
#define MS5837_RESET              		0x1E
#define MS5837_ADC_READ           		0x00
#define MS5837_PROM_READ          	0xA0
#define MS5837_CONVERT_D1_8192    	0x4A
#define MS5837_CONVERT_D2_8192    	0x5A

static const float Pa   = 100.0f;
static const float bar  = 0.001f;
static const float mbar = 1.0f;

typedef enum EMS5837Model
{
	MS5837_30BA = 0, // uint8_t
	MS5837_02BA = 1, // uint8_t
}MS5837Model;

typedef struct MS5837Device
{
	I2C_HandleTypeDef* i2c;
	MS5837Model model;	
	float fluidDensity;

	uint16_t calibData[8];
	
	uint32_t D1;
	uint32_t D2;
	int32_t  TEMP;
	int32_t  P;	
	
}MS5837Device;

extern I2C_HandleTypeDef hi2c1;

void MS5837SetModel( struct MS5837Device* dev, MS5837Model model );

/* Provide the density of the working fluid in kg/m^3. Default is for 
 * seawater. Should be 997 for freshwater.
 */
void MS5837SetFluidDensity( struct MS5837Device* dev, float density );
void MS5837Init( struct MS5837Device* dev );
float MS5837Pressure( struct MS5837Device* dev, float conversion );
float MS5837Temperature( struct MS5837Device* dev );
float MS5837Depth( struct MS5837Device* dev );
float MS5837Altitude( struct MS5837Device* dev );
static void MS5837Calculate( struct MS5837Device* dev );
DrvStatus MS5837Read( struct MS5837Device* dev );
void MS5837DataToUART( struct MS5837Device* dev );
MS5837Device MS5837GetNewDevice( MS5837Model model, float density, I2C_HandleTypeDef *i2c);
void drv_pressure_meas_ms5837_read(void *buf);
void ms5837_init(void);

//void MS5837DeleteDevice( MS5837Device* dev );

//======================================================================BT Charge
//			0x6B
#define BQ24295_ADDR_    		     	0xD6
#define BQ24295_ADDR_WRITE         	0xD6
#define BQ24295_ADDR_READ          	0xD7
#define BQ24295_RESET              		0x1E
#define BQ24295_ADC_READ           	0x00
#define BQ24295_PROM_READ          	0xA0
#define BQ24295_CONVERT_D1_8192    0x4A
#define BQ24295_CONVERT_D2_8192    0x5A

uint8_t BQ24295ReadByte(uint8_t reg,uint8_t* pData);
uint8_t BQ24295WriteByte(uint8_t reg,uint8_t Data);


void WIN24C02_init(void);   //24c02初始化子程序
void delay_nop(void);
void delay2(u32 x);
void start(void);
void stop(void);
void writex(u8 j);
u8 readx(void);
void clock(void);
u8 WIN24C02_read(u8 device,u8 address);  //从24c02的地址address中读取一个字节数据
void WIN24C02_write(u8 device,u8 address,u8 info);  //向24c02的address地址中写入一字节
void WIN24C02_write_shot(u8 device,u8 address);
u8 WIN24C02_read_new(u8 device,u8 address,u8* pbuf,u8 size);
u8 WIN24C02_read_2(u8 device,u8* pbuf,u8 size);
void WIN24C02_write_new(u8 device,u8 address,u8* pinfo,u8 size);
void My_I2C_IO_Init(void);
#ifdef __cplusplus
}
#endif

#endif /* _MS5837_H_ */
