/*
 * Copyright (C) 2018-2019 RC-AUV.
 * All rights reserved.
 */

#include "MS5837.h"
#include "sensors_types.h"
#include <stdbool.h>
#include <math.h>
#include <stdio.h>

uint16_t size;
uint8_t data[256];

MS5837Device ms583730ba_sensor;
I2C_HandleTypeDef hi2c1;

/* I2C1 init function */
void MX_I2C1_Init(void)
{
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10909CEC;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
	printf("HAL_I2C_Init err \n");
  }
  /**Configure Analogue filter 
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
	printf("HAL_I2C_Init err \n");
  }
  /**Configure Digital filter 
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
	printf("HAL_I2C_Init err \n");
  }

}


static uint16_t to_uint16( uint8_t bytes[] )
{
  return (uint16_t)( (bytes[0] << 8) | bytes[1] );
}

static uint32_t to_uint32( uint8_t bytes[] )
{
  return (uint32_t)( (bytes[0] << 16) | (bytes[1] << 8) | bytes[2] );
}

static uint8_t crc4( uint16_t n_prom[] )
{
	uint16_t n_rem = 0;

	n_prom[0] = ((n_prom[0]) & 0x0FFF);
	n_prom[7] = 0;

	for ( uint8_t i = 0; i < 16; i++ )
	{
		if ( i%2 == 1 )
		{
			n_rem ^= (uint16_t)((n_prom[i>>1]) & 0x00FF);
		}
		else
		{
			n_rem ^= (uint16_t)(n_prom[i>>1] >> 8);
		}
		for ( uint8_t n_bit = 8 ; n_bit > 0 ; n_bit-- )
		{
			if ( n_rem & 0x8000 )
			{
				n_rem = (n_rem << 1) ^ 0x3000;
			}
			else
			{
				n_rem = (n_rem << 1);
			}
		}
	}
	
	n_rem = ((n_rem >> 12) & 0x000F);

	return n_rem ^ 0x00;
}


void MS5837SetModel( struct MS5837Device* dev, MS5837Model model )
{
	dev->model = model;
}

void MS5837SetFluidDensity( struct MS5837Device* dev, float density )
{
	dev->fluidDensity = density;
}
#if IO_MODULATE_I2C
void MS5837Init( struct MS5837Device* dev )
{
	uint8_t cmd;
	uint8_t buffer[2];
#if 1	
	DrvStatus st = DRV_FAILURE;
	
	while ( st != DRV_SUCCESS )
	{	
		// Reset the MS5837 according to datasheet
		cmd = MS5837_RESET;
		
		WIN24C02_write_shot(MS5837_ADDR_WRITE, cmd);
		// Wait for reset to complete
		HAL_Delay(10);

		// Read calibration values and CRC
		for ( uint8_t i = 0; i < 7; i++ )
		{
			cmd = MS5837_PROM_READ + i*2;
			WIN24C02_write_shot(MS5837_ADDR_WRITE,cmd);
			WIN24C02_read_2(MS5837_ADDR_WRITE,buffer,2);
			//WIN24C02_read_new(MS5837_ADDR_WRITE,cmd,buffer,2);
			dev->calibData[i] = to_uint16( buffer );
			printf("+%d+%d\r\n",buffer[0],buffer[1]);
		}
		
		if ( st != DRV_FAILURE )
		{
			continue;
		}

		// Verify data with CRC
		uint8_t crcRead = dev->calibData[0] >> 12;
		uint8_t crcCalculated = crc4( dev->calibData );

		if ( crcCalculated == crcRead )
		{
			st = DRV_SUCCESS; // Initialization success
		}
		else
		{
			st = DRV_CRC_ERROR; // CRC fail
			size = sprintf( (char *)data, "MS5837 CRC error\n\r" );
			printf("%s", data);
		}
	}
	
	size = sprintf( (char *)data, "MS5837 init SUCCESS!\n\r" );
	printf("%s", data);	
#endif
}

float MS5837Pressure( struct MS5837Device* dev, float conversion )
{
	return dev->P * conversion;
}

float MS5837Temperature( struct MS5837Device* dev )
{
	return dev->TEMP / 100.0f;
}

float MS5837Depth( struct MS5837Device* dev )
{
	return ( MS5837Pressure( dev, Pa ) - .101300 ) / ( dev->fluidDensity * 9.80665 );
}

float MS5837Altitude( struct MS5837Device* dev )
{
	return ( 1 - pow( ( MS5837Pressure( dev, mbar ) / 1013.25), .190284 ) ) * 145366.45 * .3048;
}

static void MS5837Calculate( struct MS5837Device* dev )
{
	// Given C1-C6 and D1, D2, calculated TEMP and P
	// Do conversion first and then second order temp compensation	
	int32_t dT 		= 0;
	int64_t SENS 	= 0;
	int64_t OFF 	= 0;
	int32_t SENSi = 0;
	int32_t OFFi 	= 0;  
	int32_t Ti 		= 0;    
	int64_t OFF2 	= 0;
	int64_t SENS2 = 0;
	
	// Terms called
	dT = dev->D2 - (uint32_t)(dev->calibData[5]) * 256l;
	if ( dev->model == MS5837_02BA )
	{
		SENS = (int64_t)(dev->calibData[1]) * 65536l + ( (int64_t)(dev->calibData[3]) * dT ) / 128l;
		OFF = (int64_t)(dev->calibData[2]) * 131072l + ( (int64_t)(dev->calibData[4]) * dT ) / 64l;
		dev->P = ( dev->D1 * SENS/(2097152l) - OFF ) / (32768l);
	}
	else
	{
		SENS = (int64_t)(dev->calibData[1]) * 32768l + ( (int64_t)(dev->calibData[3]) * dT ) / 256l;
		OFF = (int64_t)(dev->calibData[2]) * 65536l + ( (int64_t)(dev->calibData[4]) * dT ) / 128l;
		dev->P = ( dev->D1 * SENS / (2097152l)-OFF ) / (8192l);
	}
	
	// Temp conversion
	dev->TEMP = 2000l + (int64_t)(dT) * dev->calibData[6] / 8388608LL;
	
	//Second order compensation
	if ( dev->model == MS5837_02BA )
	{
		if ( (dev->TEMP / 100) < 20 )
		{
			//Low temp
			Ti = ( 11*(int64_t)(dT) * (int64_t)(dT) ) / (34359738368LL);
			OFFi = ( 31 * ( dev->TEMP-2000 ) * ( dev->TEMP-2000 ) ) / 8;
			SENSi = ( 63 * ( dev->TEMP-2000 ) * ( dev->TEMP-2000 ) ) / 32;
		}
	}
	else
	{
		if ( (dev->TEMP / 100) < 20 )
		{
			//Low temp
			Ti = ( 3 * (int64_t)(dT) * (int64_t)(dT) ) / (8589934592LL);
			OFFi = ( 3 * ( dev->TEMP - 2000 ) * ( dev->TEMP - 2000 ) ) / 2;
			SENSi = ( 5 * ( dev->TEMP - 2000 ) * ( dev->TEMP - 2000 ) ) / 8;
			
			if ( (dev->TEMP / 100) < -15 )
			{
				//Very low temp
				OFFi = OFFi + 7 * ( dev->TEMP + 1500l ) * ( dev->TEMP + 1500l );
				SENSi = SENSi + 4 * ( dev->TEMP + 1500l ) * ( dev->TEMP + 1500l );
			}
		}
		else if ( ( dev->TEMP / 100 ) >= 20 )
		{
			//High temp
			Ti = 2 * ( dT * dT ) / (137438953472LL);
			OFFi = ( 1 * ( dev->TEMP - 2000) * ( dev->TEMP - 2000 ) ) / 16;
			SENSi = 0;
		}
	}
	
	OFF2 = OFF-OFFi;           //Calculate pressure and temp second order
	SENS2 = SENS-SENSi;
	
	if ( dev->model == MS5837_02BA )
	{
		dev->TEMP = (dev->TEMP-Ti);
		dev->P = ( ( ( dev->D1 * SENS2 ) / 2097152l - OFF2 ) / 32768l ) / 100;
	}
	else
	{
		dev->TEMP = (dev->TEMP-Ti);
		dev->P = ( ( ( dev->D1 * SENS2 ) / 2097152l - OFF2 ) / 8192l ) / 10;
	}
}

DrvStatus MS5837Read( struct MS5837Device* dev )
{
	uint8_t cmd;
	uint8_t buffer[3];
#if 1
	// Request D1 conversion
	cmd = MS5837_CONVERT_D1_8192;
	WIN24C02_write_shot(MS5837_ADDR_WRITE,cmd);

	HAL_Delay(100); // Max conversion time per datasheet
	
	cmd = MS5837_ADC_READ;
#if 0
	if ( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
	{
		return DRV_TRANSMIT_FAILURE;
	}
	
	if ( HAL_I2C_Master_Receive( dev->i2c, MS5837_ADDR_READ, buffer, 3, 10000 ) != HAL_OK )
	{
		return DRV_RECIEVE_FAILURE;
	}
#else
	WIN24C02_read_new(MS5837_ADDR_WRITE,cmd,buffer,3);
	//WIN24C02_write_shot(MS5837_ADDR_WRITE,cmd);
	//WIN24C02_read_2(MS5837_ADDR_WRITE,buffer,3);
	printf("%d:%d:%d\r\n",buffer[0],buffer[1],buffer[2]);
#endif	
	dev->D1 = to_uint32( buffer );

	HAL_Delay(100); // Max conversion time per datasheet	
	// Request D2 conversion
	cmd = MS5837_CONVERT_D2_8192;
	WIN24C02_write_shot(MS5837_ADDR_WRITE,cmd);
#if 0
	if ( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
	{
		return DRV_TRANSMIT_FAILURE;
	}	
#endif	
	HAL_Delay(100); // Max conversion time per datasheet
	
	cmd = MS5837_ADC_READ;
#if 0
	if ( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
	{
		return DRV_TRANSMIT_FAILURE;
	}
	
	if ( HAL_I2C_Master_Receive( dev->i2c, MS5837_ADDR_READ, buffer, 3, 10000 ) != HAL_OK )
	{
		return DRV_RECIEVE_FAILURE;
	}
#else	
	WIN24C02_write_shot(MS5837_ADDR_WRITE,cmd);
	WIN24C02_read_2(MS5837_ADDR_WRITE,buffer,3);
#endif
	dev->D2 = to_uint32( buffer );	

	MS5837Calculate( dev );
#endif	
	return DRV_SUCCESS;
}

MS5837Device MS5837GetNewDevice( MS5837Model model, float density, I2C_HandleTypeDef *i2c)
{
	MS5837Device dev;
	dev.model = model;
	dev.fluidDensity = density;
	dev.i2c = i2c;
	
	return dev;
}


void drv_pressure_meas_ms5837_read(void *buf)
{
    pressure_data_t *pdata = (pressure_data_t *)buf;
	
    MS5837Device* dev;

    dev = &ms583730ba_sensor;
		
    MS5837Read( dev );
			
    float pressure = MS5837Pressure( dev, Pa );
    float temperature = MS5837Temperature( dev );
    float depth = MS5837Depth( dev );
    float altitude = MS5837Altitude( dev );
		
    pdata->pressure = pressure;
    pdata->temperature = temperature;
    pdata->depth = depth;
    pdata->altitude = altitude;
		
//    printf("----pressure=%.2f ,temperature=%.2f  ,depth=%.2f -----\n",pressure, temperature,depth);
//	u8 tmpBuf;
//	BQ24295ReadByte(0x02,&tmpBuf);
//	printf("read from BQ24295 reg01:0x%02x-----\r\n",tmpBuf);
//	BQ24295ReadByte(0x08,&tmpBuf);
//	printf("read from BQ24295 reg08:0x%02x++++\r\n",tmpBuf);	
//	BQ24295WriteByte(0x02,0x10);	
}


void ms5837_init(void)
{
	My_I2C_IO_Init();
	ms583730ba_sensor = MS5837GetNewDevice( MS5837_30BA, 1029, &hi2c1);	
	//MS5837Init( &ms583730ba_sensor );	
}
#else
void MS5837Init( struct MS5837Device* dev )
{
	uint8_t cmd;
	uint8_t buffer[2];
	
	DrvStatus st = DRV_FAILURE;
	
	while ( st != DRV_SUCCESS )
	{	
		// Reset the MS5837 according to datasheet
		cmd = MS5837_RESET;
		
		if ( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
		{
			st = DRV_RESET_FAILURE;
			size = sprintf( (char *)data, "MS5837 RESET failed\n\r" );
			printf("%s", data);
			continue;
		}
		
		// Wait for reset to complete
		HAL_Delay(10);

		// Read calibration values and CRC
		for ( uint8_t i = 0; i < 7; i++ )
		{
			cmd = MS5837_PROM_READ + i*2;
			if( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
			{
				st = DRV_TRANSMIT_FAILURE;
				size = sprintf( (char *)data, "MS5837 i2c TRANSMIT failed\n\r" );
				printf("%s", data);
				break;
			}
			
			if ( HAL_I2C_Master_Receive( dev->i2c, MS5837_ADDR_READ, buffer, 2, 10000 ) != HAL_OK )
			{
				st = DRV_RECIEVE_FAILURE;
				size = sprintf( (char *)data, "MS5837 i2c RECIEVE failed\n\r" );
				printf("%s", data);
				break;
			}
			
			dev->calibData[i] = to_uint16( buffer );
		}
		
		if ( st != DRV_FAILURE )
		{
			continue;
		}

		// Verify data with CRC
		uint8_t crcRead = dev->calibData[0] >> 12;
		uint8_t crcCalculated = crc4( dev->calibData );

		if ( crcCalculated == crcRead )
		{
			st = DRV_SUCCESS; // Initialization success
		}
		else
		{
			st = DRV_CRC_ERROR; // CRC fail
			size = sprintf( (char *)data, "MS5837 CRC error\n\r" );
			printf("%s", data);
		}
	}
	
	size = sprintf( (char *)data, "MS5837 init SUCCESS!\n\r" );
	printf("%s", data);	
}

float MS5837Pressure( struct MS5837Device* dev, float conversion )
{
	return dev->P * conversion;
}

float MS5837Temperature( struct MS5837Device* dev )
{
	return dev->TEMP / 100.0f;
}

float MS5837Depth( struct MS5837Device* dev )
{
	return ( MS5837Pressure( dev, Pa ) - .101300f ) / ( dev->fluidDensity * 9.80665f );
}

float MS5837Altitude( struct MS5837Device* dev )
{
	return ( 1 - pow( ( MS5837Pressure( dev, mbar ) / 1013.25f), .190284f ) ) * 145366.45f * .3048f;
}

static void MS5837Calculate( struct MS5837Device* dev )
{
	// Given C1-C6 and D1, D2, calculated TEMP and P
	// Do conversion first and then second order temp compensation	
	int32_t dT 		= 0;
	int64_t SENS 	= 0;
	int64_t OFF 	= 0;
	int32_t SENSi = 0;
	int32_t OFFi 	= 0;  
	int32_t Ti 		= 0;    
	int64_t OFF2 	= 0;
	int64_t SENS2 = 0;
	
	// Terms called
	dT = dev->D2 - (uint32_t)(dev->calibData[5]) * 256l;
	if ( dev->model == MS5837_02BA )
	{
		SENS = (int64_t)(dev->calibData[1]) * 65536l + ( (int64_t)(dev->calibData[3]) * dT ) / 128l;
		OFF = (int64_t)(dev->calibData[2]) * 131072l + ( (int64_t)(dev->calibData[4]) * dT ) / 64l;
		dev->P = ( dev->D1 * SENS/(2097152l) - OFF ) / (32768l);
	}
	else
	{
		SENS = (int64_t)(dev->calibData[1]) * 32768l + ( (int64_t)(dev->calibData[3]) * dT ) / 256l;
		OFF = (int64_t)(dev->calibData[2]) * 65536l + ( (int64_t)(dev->calibData[4]) * dT ) / 128l;
		dev->P = ( dev->D1 * SENS / (2097152l)-OFF ) / (8192l);
	}
	
	// Temp conversion
	dev->TEMP = 2000l + (int64_t)(dT) * dev->calibData[6] / 8388608LL;
	
	//Second order compensation
	if ( dev->model == MS5837_02BA )
	{
		if ( (dev->TEMP / 100) < 20 )
		{
			//Low temp
			Ti = ( 11*(int64_t)(dT) * (int64_t)(dT) ) / (34359738368LL);
			OFFi = ( 31 * ( dev->TEMP-2000 ) * ( dev->TEMP-2000 ) ) / 8;
			SENSi = ( 63 * ( dev->TEMP-2000 ) * ( dev->TEMP-2000 ) ) / 32;
		}
	}
	else
	{
		if ( (dev->TEMP / 100) < 20 )
		{
			//Low temp
			Ti = ( 3 * (int64_t)(dT) * (int64_t)(dT) ) / (8589934592LL);
			OFFi = ( 3 * ( dev->TEMP - 2000 ) * ( dev->TEMP - 2000 ) ) / 2;
			SENSi = ( 5 * ( dev->TEMP - 2000 ) * ( dev->TEMP - 2000 ) ) / 8;
			
			if ( (dev->TEMP / 100) < -15 )
			{
				//Very low temp
				OFFi = OFFi + 7 * ( dev->TEMP + 1500l ) * ( dev->TEMP + 1500l );
				SENSi = SENSi + 4 * ( dev->TEMP + 1500l ) * ( dev->TEMP + 1500l );
			}
		}
		else if ( ( dev->TEMP / 100 ) >= 20 )
		{
			//High temp
			Ti = 2 * ( dT * dT ) / (137438953472LL);
			OFFi = ( 1 * ( dev->TEMP - 2000) * ( dev->TEMP - 2000 ) ) / 16;
			SENSi = 0;
		}
	}
	
	OFF2 = OFF-OFFi;           //Calculate pressure and temp second order
	SENS2 = SENS-SENSi;
	
	if ( dev->model == MS5837_02BA )
	{
		dev->TEMP = (dev->TEMP-Ti);
		dev->P = ( ( ( dev->D1 * SENS2 ) / 2097152l - OFF2 ) / 32768l ) / 100;
	}
	else
	{
		dev->TEMP = (dev->TEMP-Ti);
		dev->P = ( ( ( dev->D1 * SENS2 ) / 2097152l - OFF2 ) / 8192l ) / 10;
	}
}

DrvStatus MS5837Read( struct MS5837Device* dev )
{
	uint8_t cmd;
	uint8_t buffer[3];
	
	// Request D1 conversion
	cmd = MS5837_CONVERT_D1_8192;
	if ( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
	{
		return DRV_TRANSMIT_FAILURE;
	}

	HAL_Delay(100); // Max conversion time per datasheet
	
	cmd = MS5837_ADC_READ;
	if ( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
	{
		return DRV_TRANSMIT_FAILURE;
	}
	
	if ( HAL_I2C_Master_Receive( dev->i2c, MS5837_ADDR_READ, buffer, 3, 10000 ) != HAL_OK )
	{
		return DRV_RECIEVE_FAILURE;
	}
	
	dev->D1 = to_uint32( buffer );

	HAL_Delay(100); // Max conversion time per datasheet	
	// Request D2 conversion
	cmd = MS5837_CONVERT_D2_8192;
	if ( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
	{
		return DRV_TRANSMIT_FAILURE;
	}	
	
	HAL_Delay(100); // Max conversion time per datasheet
	
	cmd = MS5837_ADC_READ;
	if ( HAL_I2C_Master_Transmit( dev->i2c, MS5837_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
	{
		return DRV_TRANSMIT_FAILURE;
	}
	
	if ( HAL_I2C_Master_Receive( dev->i2c, MS5837_ADDR_READ, buffer, 3, 10000 ) != HAL_OK )
	{
		return DRV_RECIEVE_FAILURE;
	}

	dev->D2 = to_uint32( buffer );	

	MS5837Calculate( dev );
	
	return DRV_SUCCESS;
}

MS5837Device MS5837GetNewDevice( MS5837Model model, float density, I2C_HandleTypeDef *i2c)
{
	MS5837Device dev;
	dev.model = model;
	dev.fluidDensity = density;
	dev.i2c = i2c;
	
	return dev;
}

#define depth_array_space 20
void drv_pressure_meas_ms5837_read(void *buf)
{
		
    pressure_data_t *pdata = (pressure_data_t *)buf;
	
    MS5837Device* dev;

    dev = &ms583730ba_sensor;
		
    MS5837Read( dev );
			
    float pressure = MS5837Pressure( dev, Pa );
    float temperature = MS5837Temperature( dev );
    float depth = MS5837Depth( dev );
    float altitude = MS5837Altitude( dev );

    pdata->pressure = pressure;
    pdata->temperature = temperature;
    pdata->depth = depth;
    pdata->altitude = altitude;

//    printf("----pressure=%.2f ,temperature=%.2f  ,depth=%.2f -----\n",pressure, temperature,depth);
//	u8 tmpBuf;
//	BQ24295ReadByte(0x02,&tmpBuf);
//	printf("read from BQ24295 reg01:0x%02x-----\r\n",tmpBuf);
//	BQ24295ReadByte(0x08,&tmpBuf);
//	printf("read from BQ24295 reg08:0x%02x++++\r\n",tmpBuf);	
//	BQ24295WriteByte(0x02,0x10);	
}


void ms5837_init(void)
{

    MX_I2C1_Init();	

    ms583730ba_sensor = MS5837GetNewDevice( MS5837_30BA, 1029, &hi2c1);	
    MS5837Init( &ms583730ba_sensor );

    printf("%s successfully \n", __func__);

}
#endif
//-------------------------------------------------------------------------------------
u8 BQ24295ReadByte(u8 reg,u8* pData)
{
#if IO_MODULATE_I2C
	*pData = WIN24C02_read(BQ24295_ADDR_,reg);
#else
	u8 cmd;
	cmd = reg;
	if (HAL_I2C_Master_Transmit(&hi2c1, BQ24295_ADDR_WRITE, &cmd, 1, 10000 ) != HAL_OK )
	{
		return 0;
	}
	
	if (HAL_I2C_Master_Receive(&hi2c1, BQ24295_ADDR_READ, pData, 1, 10000 ) != HAL_OK )
	{
		return 0;
	}
#endif
	return 1;
}
u8 BQ24295WriteByte(u8 reg,u8 Data)
{
#if IO_MODULATE_I2C
	WIN24C02_write(BQ24295_ADDR_,reg,Data);
#else
//	u8 cmd;
	u8 dataBuf[2];
	dataBuf[0] = reg;
	dataBuf[1] = Data;		

	if ( HAL_I2C_Master_Transmit(&hi2c1, BQ24295_ADDR_WRITE, dataBuf, 2, 10000 ) != HAL_OK )
	{
		return 0;
	}
#endif
	return 1;
}

//===============================================================================
#define GPIO_PORT_I2C GPIOB /* GPIO端口 */

#define I2C_SCL_PIN GPIO_PIN_6 /* 连接到SCL时钟线的GPIO */
#define I2C_SDA_PIN GPIO_PIN_7 /* 连接到SDA数据线的GPIO */
#define	SCL							GPIO_PIN_6 	//24C02 SCL
#define	SDA      					     	GPIO_PIN_7 	//24C02 SDA


/* 定义读写SCL和SDA的宏 */
#define I2C_SCL_1() ?GPIO_PORT_I2C->BSRRL = I2C_SCL_PIN /* SCL = 1 */
#define I2C_SCL_0() ?GPIO_PORT_I2C->BSRRH = I2C_SCL_PIN /* SCL = 0 */
#define I2C_SDA_1() ?GPIO_PORT_I2C->BSRRL = I2C_SDA_PIN /* SDA = 1 */
#define I2C_SDA_0() ?GPIO_PORT_I2C->BSRRH = I2C_SDA_PIN /* SDA = 0 */

#define I2C_SDA_READ()		HAL_GPIO_ReadPin(GPIO_PORT_I2C,I2C_SDA_PIN)/* 读SDA口线状态 */
#define I2C_SCL_READ()		HAL_GPIO_ReadPin(GPIO_PORT_I2C,I2C_SCL_PIN) /* 读SCL口线状态 */


void I2C_GPIO_Config(void)  
{  
	GPIO_InitTypeDef GPIO_Initure;

	__HAL_RCC_GPIOB_CLK_ENABLE();           //?a??GPIOBê±?ó

	GPIO_Initure.Pin=I2C_SCL_PIN|I2C_SDA_PIN;            //PA0
	GPIO_Initure.Mode=GPIO_MODE_OUTPUT_OD;//¨a?¨a¨?¨o?3?
	GPIO_Initure.Pull=GPIO_NOPULL;        //¨|?¨¤-
	GPIO_Initure.Speed=GPIO_SPEED_FAST;   //???¨′

	HAL_GPIO_Init(GPIO_PORT_I2C,&GPIO_Initure);
}  

void delay1(u32 x)
{
   u32 i;
   for(i=0;i<x;i++);
}

void delay2(u32 x)
{
   delay1(x);
   delay1(x);
}

void delay_nop(void)
{
  u8 i=10; //i=10延时1.5us//这里可以优化速度 ，经测试最低到5还能写入
   while(i--);

}

void AT24C02_SDA_IO_SET(unsigned char io_set) //SDA引脚输入输出设置
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	if(io_set==0)
	{
		GPIO_InitStructure.Pin = SDA;          //24C02 SDA 作为输出
		GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStructure.Pull=GPIO_NOPULL;        //¨|?¨¤-
		GPIO_InitStructure.Speed = GPIO_SPEED_FAST;
		HAL_GPIO_Init(GPIO_PORT_I2C,&GPIO_InitStructure);		
	}
	else if(io_set==1)
	{
		GPIO_InitStructure.Pin = SDA;          //24C02 SDA 作为输出
		GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
		GPIO_InitStructure.Pull=GPIO_PULLUP;        //¨|?¨¤-
		HAL_GPIO_Init(GPIO_PORT_I2C,&GPIO_InitStructure);			
	}
	else
	{;}
}
void WIN24C02_init(void)
{
   
   //SCL=1;
   HAL_GPIO_WritePin(GPIOB,SCL,GPIO_PIN_SET);
  delay_nop();
   //SDA=1;
   HAL_GPIO_WritePin(GPIOB,SDA,GPIO_PIN_SET);
   delay_nop();
}
void start(void)
{
   //SDA=1;
   HAL_GPIO_WritePin(GPIOB,SDA,GPIO_PIN_SET);
   delay_nop();
   //SCL=1;
   HAL_GPIO_WritePin(GPIOB,SCL,GPIO_PIN_SET);
   delay_nop();
   //SDA=0;
    HAL_GPIO_WritePin(GPIOB, SDA,GPIO_PIN_RESET);
   delay_nop();
   //SCL=0;
   HAL_GPIO_WritePin(GPIOB, SCL,GPIO_PIN_RESET);
   delay_nop();
}
void stop(void)

{
   //SDA=0;
   HAL_GPIO_WritePin(GPIOB, SDA,GPIO_PIN_RESET);
   delay_nop();
   //SCL=1;
   HAL_GPIO_WritePin(GPIOB,SCL,GPIO_PIN_SET);
   delay_nop();
   //SDA=1;
   HAL_GPIO_WritePin(GPIOB,SDA,GPIO_PIN_SET);
   delay_nop();
}
void writex(u8 j)

{
	u8 i,temp,temp1;

	temp=j;
	//AT24C02_SDA_IO_SET(0);
	for (i=0;i<8;i++)
	{
		temp1=temp & 0x80;
		temp=temp<<1;
	 
		//SCL=0;
		HAL_GPIO_WritePin(GPIOB, SCL,GPIO_PIN_RESET);
		delay_nop();

		//SDA=CY;
		if(temp1==0x80)
		{HAL_GPIO_WritePin(GPIOB, SDA,GPIO_PIN_SET);}
		else
		{HAL_GPIO_WritePin(GPIOB, SDA,GPIO_PIN_RESET);}

		delay_nop();
		// SCL=1;
		HAL_GPIO_WritePin(GPIOB,SCL,GPIO_PIN_SET);
		delay_nop();
	}
	//AT24C02_SDA_IO_SET(0);
	//SCL=0;
	HAL_GPIO_WritePin(GPIOB, SCL,GPIO_PIN_RESET);
	delay_nop();
	//SDA=1;
	HAL_GPIO_WritePin(GPIOB,SDA,GPIO_PIN_SET);
	delay_nop();
}

u8 readx()
{
	u8 i,j,k=0;

	//SCL=0;
	HAL_GPIO_WritePin(GPIOB, SCL,GPIO_PIN_RESET);
	delay_nop(); 
	//SDA=1;
	HAL_GPIO_WritePin(GPIOB,SDA,GPIO_PIN_SET);

	AT24C02_SDA_IO_SET(1);
	for (i=0;i<8;i++)
	{
		delay_nop();
		//SCL=1;
		HAL_GPIO_WritePin(GPIOB,SCL,GPIO_PIN_SET);
		delay_nop();
		//if (SDA==1) j=1;
		if(HAL_GPIO_ReadPin(GPIOB,SDA)==GPIO_PIN_SET ) 
		{j=1;}
		  else 
		{j=0;}
		k=(k<<1)|j;
		//SCL=0;
		HAL_GPIO_WritePin(GPIOB, SCL,GPIO_PIN_RESET);
	}
	AT24C02_SDA_IO_SET(0);
	delay_nop();
	return(k);
}

void clock()
{
	u32 i=0;
	AT24C02_SDA_IO_SET(1);
	//SCL=1;
	HAL_GPIO_WritePin(GPIOB,SCL,GPIO_PIN_SET);
	delay_nop();
	while ((HAL_GPIO_ReadPin(GPIOB,SDA)==1)&&(i<5000))i++;
	//SCL=0;
	HAL_GPIO_WritePin(GPIOB, SCL,GPIO_PIN_RESET);
	delay_nop();
	AT24C02_SDA_IO_SET(0);
}

u8 WIN24C02_read(u8 device,u8 address)
{
   u8 i;
   start();
   writex(device);
   clock();
   writex(address);
   clock();
   start();
   writex(device | 1);
   clock();
   i=readx();
   stop();
   //delay2(10);
   delay2(50);
   return(i);

}
u8 WIN24C02_read_new(u8 device,u8 address,u8* pbuf,u8 size)
{
   start();
   writex(device);
   clock();
   writex(address);
   clock();
   stop();
   start();
   writex(device | 1);
   for(int j = 0;j <size;j++)
   {
	   clock();
	   pbuf[j]=readx();
   }
   clock();
   stop();
   //delay2(10);
   delay2(50);
   return(size);

}
u8 WIN24C02_read_2(u8 device,u8* pbuf,u8 size)
{
   start();
   writex(device | 1);
   for(int j = 0;j <size;j++)
   {
	   clock();
	   pbuf[j]=readx();
   }
   clock();
   stop();
   //delay2(10);
   delay2(50);
   return(size);

}
void WIN24C02_write(u8 device,u8 address,u8 info)
{
   start();
   writex(device);
   clock();
   writex(address);
   clock();
   writex(info);
   clock();
   stop();
   //delay2(50);
   delay2(250);
}
void WIN24C02_write_new(u8 device,u8 address,u8* pinfo,u8 size)
{
   start();
   writex(device);
   clock();
   writex(address);
   clock();
   for(int i = 0;i < size;i++)
   {
	   writex(pinfo[i]);
	   clock();
   }
   stop();
   //delay2(50);
   delay2(250);
}
void WIN24C02_write_shot(u8 device,u8 address)
{
   start();
   writex(device);
   clock();
   writex(address);
   clock();
   stop();
   //delay2(50);
   delay2(250);
}
void My_I2C_IO_Init(void)
{
	I2C_GPIO_Config();	
	WIN24C02_init();
}
