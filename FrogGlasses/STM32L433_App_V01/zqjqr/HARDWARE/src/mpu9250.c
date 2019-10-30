/*
 * Copyright (C) 2015-2017 Alibaba Group Holding Limited
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mpu9250.h"
#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_spi.h"
#include "FreeRTOS.h"
#include "task.h"
#include "storageTask.h"

static int16_t MPU9250_AK8963_ASA[3] = {0, 0, 0};


#define MPU9250_SPIx_SendByte(byte) 
#define MPU9250_SPIx_SetDivisor(divisor) 
#define Delay_Ms(x) vTaskDelay(x);		

#define	MPU_CS(n)		(n?HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOA,GPIO_PIN_15,GPIO_PIN_RESET))
SPI_HandleTypeDef SPI_Mpu_Handler;  //SPI??㊣迆

//extern configParameters m_ConfigParams;
int16_t m_Mag_Bias[3] = {0,0,0};
int16_t m_Gyro_Bias[3] = {0,0,0};
#define	SWITCH_TPYE								(2)
Axis3i16 mag_Old;

void mpu_spi_init(void)
{
    SPI_Mpu_Handler.Instance=SPI3;                         //SP1
    SPI_Mpu_Handler.Init.Mode=SPI_MODE_MASTER;             //谷豕??SPI1∟℅¯?㏒那?㏒?谷豕???a?¯?㏒那?
    SPI_Mpu_Handler.Init.Direction=SPI_DIRECTION_2LINES;   //谷豕??SPI米ㄓ?辰?辰?????辰米?那y?Y?㏒那?:SPI谷豕???a?????㏒那?
    SPI_Mpu_Handler.Init.DataSize=SPI_DATASIZE_8BIT;       //谷豕??SPI米?那y?Y∩車D?:SPI﹞⊿?赤?車那?8?????芍11
    SPI_Mpu_Handler.Init.CLKPolarity=SPI_POLARITY_HIGH;    //∩?DD赤?2?那㊣?車米????D℅∩足??a??米???
    SPI_Mpu_Handler.Init.CLKPhase=SPI_PHASE_2EDGE;         //∩?DD赤?2?那㊣?車米?米迆?t??足?㊣???㏒“谷?谷y?辰???米㏒?那y?Y㊣?2谷?迄
    SPI_Mpu_Handler.Init.NSS=SPI_NSS_SOFT;                 //NSSD?o?車谷車2?t㏒“NSS1邦??㏒??1那?豕赤?t㏒“那1車?SSI??㏒?1邦角赤:?迆2?NSSD?o?車DSSI??????
    SPI_Mpu_Handler.Init.BaudRatePrescaler=SPI_BAUDRATEPRESCALER_256;//?“辰?2“足??那?∟﹞??米米??米:2“足??那?∟﹞??米?米?a256
    SPI_Mpu_Handler.Init.FirstBit=SPI_FIRSTBIT_MSB;        //???“那y?Y∩?那?∩車MSB???1那?LSB???a那?:那y?Y∩?那?∩車MSB???a那?
    SPI_Mpu_Handler.Init.TIMode=SPI_TIMODE_DISABLE;        //1?㊣?TI?㏒那?
    SPI_Mpu_Handler.Init.CRCCalculation=SPI_CRCCALCULATION_DISABLE;//1?㊣?車2?tCRCD㏒?谷
    SPI_Mpu_Handler.Init.CRCPolynomial=7;                  //CRC?米????米??角??那?
    SPI_Mpu_Handler.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    SPI_Mpu_Handler.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
	
    HAL_SPI_Init(&SPI_Mpu_Handler);//3?那??‘
    
    __HAL_SPI_ENABLE(&SPI_Mpu_Handler);                    //那1?邦SPI5
			
}


uint8_t MPU9250_Init(void)
{
	uint8_t ret = 0;
	uint8_t data = 0, state = 0;
	uint8_t response[3] = {0, 0, 0};
	//黍�！D
	uint8_t id = 0x00;

      GPIO_InitTypeDef  GPIO_Initure;
	
      __HAL_RCC_GPIOA_CLK_ENABLE(); 

      GPIO_Initure.Pin=GPIO_PIN_15;	
      GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;//赤?赤足那?3?
      GPIO_Initure.Pull=GPIO_PULLUP;        //谷?角-
      GPIO_Initure.Speed=GPIO_SPEED_FAST;   //???迄

     HAL_GPIO_Init(GPIOA,&GPIO_Initure);//3?那??‘	

	mpu_spi_init();
	MPU_CS(0);

	//MPU9250 Reset
	MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_PWR_MGMT_1, MPU9250_RESET);
	Delay_Ms(100);

	id = MPU9250_SPIx_Read(MPU9250_SPIx_ADDR, MPU9250_WHO_AM_I);
	printf("id =%d \n",id); 

	if (0x71 == id) {
		//MPU9250 Set Clock Source
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_PWR_MGMT_1,  MPU9250_CLOCK_PLLGYROZ);
		Delay_Ms(1);
//	id = MPU9250_SPIx_Read(MPU9250_SPIx_ADDR, MPU9250_PWR_MGMT_1);
//	printf("0x03 =%2x \n",id); 		
		//MPU9250 Set Interrupt
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_INT_PIN_CFG,  MPU9250_INT_ANYRD_2CLEAR);
		Delay_Ms(1);
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_INT_ENABLE, 1);
		Delay_Ms(1);
		//MPU9250 Set Sensors
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_PWR_MGMT_2, MPU9250_XYZ_GYRO & MPU9250_XYZ_ACCEL);
		Delay_Ms(1);
		//MPU9250 Set SampleRate
		//SAMPLE_RATE = Internal_Sample_Rate / (1 + SMPLRT_DIV)
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_SMPLRT_DIV, SMPLRT_DIV);
		Delay_Ms(1);
		//MPU9250 Set Full Scale Gyro Range
		//Fchoice_b[1:0] = [00] enable DLPF
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_GYRO_CONFIG, (MPU9250_FSR_2000DPS << 3));
		Delay_Ms(1);
		//MPU9250 Set Full Scale Accel Range PS:2G
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_ACCEL_CONFIG, (MPU9250_FSR_2G << 3));
		Delay_Ms(1);
		//MPU9250 Set Accel DLPF
		data = MPU9250_SPIx_Read(MPU9250_SPIx_ADDR, MPU9250_ACCEL_CONFIG2);
#if 0	// zhengl
		data |= MPU9250_ACCEL_DLPF_41HZ;
#else
		data &= (~0x07);
		data |= MPU9250_ACCEL_DLPF_20HZ;
#endif
		Delay_Ms(1);
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_ACCEL_CONFIG2, data);
		Delay_Ms(1);
		//MPU9250 Set Gyro DLPF
		//MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_CONFIG, MPU9250_GYRO_DLPF_41HZ);
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_CONFIG, MPU9250_GYRO_DLPF_20HZ);	//zhengl
		Delay_Ms(1);
		//MPU9250 Set SPI Mode
		state = MPU9250_SPIx_Read(MPU9250_SPIx_ADDR, MPU9250_USER_CTRL);
		Delay_Ms(1);
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_USER_CTRL, state | MPU9250_I2C_IF_DIS);
		Delay_Ms(1);
		state = MPU9250_SPIx_Read(MPU9250_SPIx_ADDR, MPU9250_USER_CTRL);
		Delay_Ms(1);
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_USER_CTRL, state | MPU9250_I2C_MST_EN);
		Delay_Ms(1);
		//////////////////////////////////////////////////////////////////////////
		//AK8963 Setup
		//reset AK8963
		MPU9250_AK8963_SPIx_Write(MPU9250_AK8963_I2C_ADDR, MPU9250_AK8963_CNTL2, MPU9250_AK8963_CNTL2_SRST);
		Delay_Ms(2);

		MPU9250_AK8963_SPIx_Write(MPU9250_AK8963_I2C_ADDR, MPU9250_AK8963_CNTL, MPU9250_AK8963_POWER_DOWN);
		Delay_Ms(1);
		MPU9250_AK8963_SPIx_Write(MPU9250_AK8963_I2C_ADDR, MPU9250_AK8963_CNTL, MPU9250_AK8963_FUSE_ROM_ACCESS);
		Delay_Ms(1);
		//
		//AK8963 get calibration data
		MPU9250_AK8963_SPIx_Reads(MPU9250_AK8963_I2C_ADDR, MPU9250_AK8963_ASAX, 3, response);
		//AK8963_SENSITIVITY_SCALE_FACTOR
		//AK8963_ASA[i++] = (int16_t)((data - 128.0f) / 256.0f + 1.0f) ;
		MPU9250_AK8963_ASA[0] = (int16_t)(response[0]) + 128;
		MPU9250_AK8963_ASA[1] = (int16_t)(response[1]) + 128;
		MPU9250_AK8963_ASA[2] = (int16_t)(response[2]) + 128;
		Delay_Ms(1);
		MPU9250_AK8963_SPIx_Write(MPU9250_AK8963_I2C_ADDR, MPU9250_AK8963_CNTL, MPU9250_AK8963_POWER_DOWN);
		Delay_Ms(1);
		//
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_CTRL, 0x5D);
		Delay_Ms(1);
//	id = MPU9250_SPIx_Read(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_CTRL);
//	printf("0x5d =%2x \n",id); 		
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV0_ADDR, MPU9250_AK8963_I2C_ADDR | MPU9250_I2C_READ);
		Delay_Ms(1);
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV0_REG, MPU9250_AK8963_ST1);
		Delay_Ms(1);
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV0_CTRL, 0x88);
		Delay_Ms(1);
		//
		MPU9250_AK8963_SPIx_Write(MPU9250_AK8963_I2C_ADDR, MPU9250_AK8963_CNTL, MPU9250_AK8963_CONTINUOUS_MEASUREMENT);
		Delay_Ms(1);

		//
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_CTRL, 0x09);
		Delay_Ms(1);
		//
		MPU9250_SPIx_Write(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_DELAY_CTRL, 0x81);
		Delay_Ms(100);	
		ret = 1;
		printf("MPU6000 SPI connection [OK].\n");
	} else {
		ret = 0;
		printf("MPU6000 SPI connection [FAIL].\n");
	}
	//---------------------------------------------------------zhengl add begin
	int16_t *pMin,*pMax;
	pMax = m_ConfigParams.m_Mag_Max;
	pMin = m_ConfigParams.m_Mag_Min;	
	for(int i = 0;i < 3;i++)
	{
		m_Mag_Bias[i] = (pMax[i] + pMin[i]) / 2;
		mag_Old.axis[i] = 1;
	}
//#endif
	//---------------------------------------------------------zhengl add end
	return ret;
}

int MPU9250_SPIx_Write(uint8_t addr, uint8_t reg_addr, uint8_t data){

	uint8_t tx_buf[2];
	uint8_t rx_buf[2];

	tx_buf[0] = reg_addr;
	tx_buf[1] = data;
	MPU_CS(0);	
	HAL_SPI_TransmitReceive(&SPI_Mpu_Handler, tx_buf, rx_buf, 2, 10);
	MPU_CS(1);	
	return 0;
}

int MPU9250_SPIx_Writes(uint8_t addr, uint8_t reg_addr, uint8_t len, uint8_t* data){

	uint8_t tx_buf[10];
	uint8_t rx_buf[10];

	memset(tx_buf, 0, 10);
	tx_buf[0] = reg_addr;
//	tx_buf[1] = data;
	memcpy(tx_buf+1, data, len);
	MPU_CS(0);	
	HAL_SPI_TransmitReceive(&SPI_Mpu_Handler, tx_buf, rx_buf, len+1, 10);
	MPU_CS(1);
	return 0;
}

uint8_t MPU9250_SPIx_Read(uint8_t addr, uint8_t reg_addr){

	uint8_t tx_buf[2];
	uint8_t rx_buf[2];

	memset(tx_buf, 0, 2);
	tx_buf[0] = reg_addr | 0x80;
	MPU_CS(0);
	HAL_SPI_TransmitReceive(&SPI_Mpu_Handler, tx_buf, rx_buf, 2, 10);
	MPU_CS(1);	
	return rx_buf[1];
}

int MPU9250_SPIx_Reads(uint8_t addr, uint8_t reg_addr, uint8_t len, uint8_t* data){

	uint8_t tx_buf[10];
	uint8_t rx_buf[10];

	memset(tx_buf, 0, 10);
	tx_buf[0] = reg_addr | 0x80;
	MPU_CS(0);
	HAL_SPI_TransmitReceive(&SPI_Mpu_Handler, tx_buf, rx_buf, len+1, 10);
	MPU_CS(1);	
	memcpy(data, rx_buf+1, len);
	
	return 0;
}

int MPU9250_AK8963_SPIx_Read(uint8_t akm_addr, uint8_t reg_addr, uint8_t* data) {
	uint8_t status = 0;
	uint32_t timeout = 0;

	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_REG, 1, &reg_addr);
	Delay_Ms(1);
	reg_addr = akm_addr | MPU9250_I2C_READ;
	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_ADDR, 1, &reg_addr);
	Delay_Ms(1);
	reg_addr = MPU9250_I2C_SLV4_EN;
	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_CTRL, 1, &reg_addr);
	Delay_Ms(1);

	do {
		if (timeout++ > 50){
			return -2;
		}
		MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_STATUS, 1, &status);
		Delay_Ms(1);
	} while ((status & MPU9250_I2C_SLV4_DONE) == 0);
	MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_DI, 1, data);
	return 0;
}

int MPU9250_AK8963_SPIx_Reads(uint8_t akm_addr, uint8_t reg_addr, uint8_t len, uint8_t* data){
	uint8_t index = 0;
	uint8_t status = 0;
	uint32_t timeout = 0;
	uint8_t tmp = 0;

	tmp = akm_addr | MPU9250_I2C_READ;
	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_ADDR, 1, &tmp);
	Delay_Ms(1);
	while(index < len){
		tmp = reg_addr + index;
		MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_REG, 1, &tmp);
		Delay_Ms(1);
		tmp = MPU9250_I2C_SLV4_EN;
		MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_CTRL, 1, &tmp);
		Delay_Ms(1);

		do {
			if (timeout++ > 50){
				return -2;
			}
			MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_STATUS, 1, &status);
			Delay_Ms(2);
		} while ((status & MPU9250_I2C_SLV4_DONE) == 0);
		MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_DI, 1, data + index);
		Delay_Ms(1);
		index++;
	}
	return 0;
}

int MPU9250_AK8963_SPIx_Write(uint8_t akm_addr, uint8_t reg_addr, uint8_t data)
{
	uint32_t timeout = 0;
	uint8_t status = 0;
	uint8_t tmp = 0;

	tmp = akm_addr;
	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_ADDR, 1, &tmp);
	Delay_Ms(1);
	tmp = reg_addr;
	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_REG, 1, &tmp);
	Delay_Ms(1);
	tmp = data;
	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_DO, 1, &tmp);
	Delay_Ms(1);
	tmp = MPU9250_I2C_SLV4_EN;
	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_CTRL, 1, &tmp);
	Delay_Ms(1);

	do {
		if (timeout++ > 50)
			return -2;

		MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_STATUS, 1, &status);
		Delay_Ms(1);
	} while ((status & MPU9250_I2C_SLV4_DONE) == 0);
	if (status & MPU9250_I2C_SLV4_NACK)
		return -3;
	return 0;
}

int MPU9250_AK8963_SPIx_Writes(uint8_t akm_addr, uint8_t reg_addr, uint8_t len, uint8_t* data)
{
	uint32_t timeout = 0;
	uint8_t status = 0;
	uint8_t tmp = 0;
	uint8_t index = 0;

	tmp = akm_addr;
	MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_ADDR, 1, &tmp);
	Delay_Ms(1);

	while(index < len){
		tmp = reg_addr + index;
		MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_REG, 1, &tmp);
		Delay_Ms(1);
		MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_DO, 1, data + index);
		Delay_Ms(1);
		tmp = MPU9250_I2C_SLV4_EN;
		MPU9250_SPIx_Writes(MPU9250_SPIx_ADDR, MPU9250_I2C_SLV4_CTRL, 1, &tmp);
		Delay_Ms(1);

		do {
			if (timeout++ > 50)
				return -2;
			MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_I2C_MST_STATUS, 1, &status);
			Delay_Ms(1);
		} while ((status & MPU9250_I2C_SLV4_DONE) == 0);
		if (status & MPU9250_I2C_SLV4_NACK)
			return -3;
		index++;
	}
	return 0;
}

signed short test_temp_data[9];
//////////////////////////////////////////////////////////////////////////
//
void MPU9250_Get9AxisRawData(Axis3i16 *accel, Axis3i16 * gyro, Axis3i16 * mag)
{
	uint8_t data[22];
	MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_ACCEL_XOUT_H, 22, data);
	
	accel->x = (data[0] << 8) | data[1];
	accel->y = (data[2] << 8) | data[3];
	accel->z = (data[4] << 8) | data[5];
	
	gyro->x = (data[8] << 8) | data[9];
	gyro->y = (data[10] << 8) | data[11];
	gyro->z = (data[12] << 8) | data[13];

	if (!(data[14] & MPU9250_AK8963_DATA_READY) || (data[14] & MPU9250_AK8963_DATA_OVERRUN)){
		return;
	}
	if (data[21] & MPU9250_AK8963_OVERFLOW){
		return;
	}
	mag->x = (data[16] << 8) | data[15];
	mag->y = (data[18] << 8) | data[17];
	mag->z = (data[20] << 8) | data[19];

	//ned x,y,z
	mag->x = ((long)mag->x * MPU9250_AK8963_ASA[0]) >> 8;
	mag->y = ((long)mag->y * MPU9250_AK8963_ASA[1]) >> 8;
	mag->z = ((long)mag->z * MPU9250_AK8963_ASA[2]) >> 8;
}
//////////////////////////////////////////////////////////////////////////
//
void MPU9250_Get6AxisRawData(Axis3i16 *accel, Axis3i16 * gyro)
{
	uint8_t data[14];
	MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_ACCEL_XOUT_H, 14, data);
	
	accel->x = (data[0] << 8) | data[1];
	accel->y = (data[2] << 8) | data[3];
	accel->z = (data[4] << 8) | data[5];

	gyro->x = (data[8] << 8) | data[9];
	gyro->y = (data[10] << 8) | data[11];
	gyro->z = (data[12] << 8) | data[13];
}
//////////////////////////////////////////////////////////////////////////
//
void MPU9250_Get3AxisAccelRawData(Axis3i16 * accel)
{
	uint8_t data[6];
	MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_ACCEL_XOUT_H, 6, data);

	accel->x = (data[0] << 8) | data[1];
	accel->y = (data[2] << 8) | data[3];
	accel->z = (data[4] << 8) | data[5];
//	printf("ax=%d ,ay=%d ,az=%d \n",accel->x,accel->y,accel->z);	

	test_temp_data[0] = (data[0] << 8) | data[1];
	test_temp_data[1] = (data[2] << 8) | data[3];
	test_temp_data[2] = (data[4] << 8) | data[5];
	/*
	x+		->		y-
	y+		->		Z-
	Z+		->		X
	*/
	int16_t tmpData;
#if(SWITCH_TPYE == 0)	
#elif(SWITCH_TPYE == 1)
	tmpData = accel->x;
	accel->x = -accel->z;
	accel->y = accel->y;
	accel->z = tmpData;
#elif(SWITCH_TPYE == 2)
	tmpData = accel->x;
	accel->x = -accel->y;
	accel->y = -accel->z;
	accel->z = tmpData;
#endif
}
//////////////////////////////////////////////////////////////////////////
//
void MPU9250_Get3AxisGyroRawData(Axis3i16 * gyro)
{
	uint8_t data[6];
	MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_GYRO_XOUT_H, 6, data);

	gyro->x = (data[0] << 8) | data[1];
	gyro->y = (data[2] << 8) | data[3];
	gyro->z = (data[4] << 8) | data[5];

//	printf("gx=%d ,gy=%d ,gz=%d \n",gyro->x,gyro->y,gyro->z);	
	
	test_temp_data[3] = (data[0] << 8) | data[1];
	test_temp_data[4] = (data[2] << 8) | data[3];
	test_temp_data[5] = (data[4] << 8) | data[5];
	/*
	x+		->		y-
	y+		->		Z-
	Z+		->		X
	*/
	int16_t tmpData;
#if(SWITCH_TPYE == 0)	
#elif(SWITCH_TPYE == 1)
	tmpData = gyro->x;
	gyro->x = -gyro->z;
	gyro->y = gyro->y;
	gyro->z = tmpData;
#elif(SWITCH_TPYE == 2)
	tmpData = gyro->x;
	gyro->x = -gyro->y;
	gyro->y = -gyro->z;
	gyro->z = tmpData;	
#endif
}
//////////////////////////////////////////////////////////////////////////
//
void MPU9250_Get3AxisMagnetRawData(Axis3i16 *mag)
{
	uint8_t data[8];

	MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_EXT_SENS_DATA_00, 8, data);

#if 1
	if (!(data[0] & MPU9250_AK8963_DATA_READY) || (data[0] & MPU9250_AK8963_DATA_OVERRUN)){
		return;
	}
#endif
#if 0
	while(!(data[0] & MPU9250_AK8963_DATA_READY) || (data[0] & MPU9250_AK8963_DATA_OVERRUN)){
		Delay_Ms(1000);
		MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_EXT_SENS_DATA_00, 8, data);
		printf("mag wait... \n");	
	}
#endif
	if (data[7] & MPU9250_AK8963_OVERFLOW){
		return;
	}
	mag->x = (data[2] << 8) | data[1];
	mag->y = (data[4] << 8) | data[3];
	mag->z = (data[6] << 8) | data[5];

	mag->x = ((long)mag->x * MPU9250_AK8963_ASA[0]) >> 8;
	mag->y = ((long)mag->y * MPU9250_AK8963_ASA[1]) >> 8;
	mag->z = ((long)mag->z * MPU9250_AK8963_ASA[2]) >> 8;

//	printf("mx=%d ,my=%d ,mz=%d \n",mag->x,mag->y,mag->z);		
	
	test_temp_data[6] = (data[2] << 8) | data[1];
	test_temp_data[7] = (data[4] << 8) | data[3];
	test_temp_data[8] = (data[6] << 8) | data[5];

//	printf("acce:%d	 :%d :%d\r\n",test_temp_data[0],test_temp_data[1],test_temp_data[2]);
//	printf("gyro:%d	 :%d :%d\r\n",test_temp_data[3],test_temp_data[4],test_temp_data[5]);
//	printf("magn:%d	 :%d :%d\r\n",test_temp_data[6],test_temp_data[7],test_temp_data[8]);	
}
//////////////////////////////////////////////////////////////////////////
//
void MPU9250_GetTemperatureRawData(long *temperature)
{
	uint8_t data[2];
	MPU9250_SPIx_Reads(MPU9250_SPIx_ADDR, MPU9250_TEMP_OUT_H, 2, data);
	temperature[0] = (((int16_t)data[0]) << 8) | data[1];
}
void Zhengl_GetAverage_4(Axis3i16 *mag)
{
	static Axis3i16 LPData[4];
	for(int i = 0;i < 4 - 1;i++)
	{
		LPData[i].x = LPData[i + 1].x;
		LPData[i].y = LPData[i + 1].y;
		LPData[i].z = LPData[i + 1].z;		
	}
	LPData[3].x = mag->x;
	LPData[3].y = mag->y;
	LPData[3].z = mag->z;	
	mag->x = (LPData[0].x + LPData[1].x + LPData[2].x + LPData[3].x) / 4;
	mag->y = (LPData[0].y + LPData[1].y + LPData[2].y + LPData[3].y) / 4;
	mag->z = (LPData[0].z + LPData[1].z + LPData[2].z + LPData[3].z) / 4;	
}
/*
	x+			->				z+
	y+			->				x-
	z+			->				y-
*/
void MPU9250_Get3AxisMagnetRawData_ZL(Axis3i16 *mag)
{
	Axis3i16 Tmag;
	uint8_t tmpUpdated;
	int16_t *pMin,*pMax;
	
	MPU9250_Get3AxisMagnetRawData(&Tmag);
	for(int i = 0;i < 3;i++)
	{
		if((Tmag.axis[i] > 1000)||(Tmag.axis[i] < -1000))
		{
			mag->axis[0] = mag_Old.axis[0];
			mag->axis[1] = mag_Old.axis[1];
			mag->axis[2] = mag_Old.axis[2];
			return;
		}
	}
	Zhengl_GetAverage_4(&Tmag);
	pMax = m_ConfigParams.m_Mag_Max;
	pMin = m_ConfigParams.m_Mag_Min;
	for(int i = 0;i < 3;i++)
	{
		tmpUpdated = 0;
		if(Tmag.axis[i] > pMax[i])
		{
			pMax[i] = Tmag.axis[i];
			tmpUpdated = 1;
			printf("X1:%d  Y1:%d  Z1:%d  \r\n",Tmag.axis[0],Tmag.axis[1],Tmag.axis[2]);
			printf("X1b:%d  Y1b:%d  Z1b:%d  \r\n",pMax[0],pMax[1],pMax[2]);
			printf("X1s:%d  Y1s:%d  Z1s:%d  \r\n",pMin[0],pMin[1],pMin[2]);
		}
		if(Tmag.axis[i] < pMin[i])
		{
			pMin[i] = Tmag.axis[i];
			tmpUpdated = 1;
			printf("X2:%d  Y2:%d  Z2:%d  \r\n",Tmag.axis[0],Tmag.axis[1],Tmag.axis[2]);
			printf("X2b:%d  Y2b:%d  Z2b:%d  \r\n",pMax[0],pMax[1],pMax[2]);
			printf("X2s:%d  Y2s:%d  Z2s:%d  \r\n",pMin[0],pMin[1],pMin[2]);
		}
		if(tmpUpdated)
		{
			if(pMax[i] >= pMin[i])
			{
				m_Mag_Bias[i] = (pMax[i] + pMin[i]) / 2;
			}
			MX25Lxxx_WriteMagnet_2();
		}
		//----------------------create result
		Tmag.axis[i] = Tmag.axis[i] - m_Mag_Bias[i];
	}
//	mag->x = Tmag.z;
//	mag->y = -Tmag.x;
//	mag->z = -Tmag.y;	
#if(SWITCH_TPYE == 0)	
	mag->x = Tmag.x;
	mag->y = Tmag.y;
	mag->z = Tmag.z;
#elif(SWITCH_TPYE == 1)
	mag->x = Tmag.x;
	mag->y = Tmag.z;
	mag->z = -Tmag.y;
#elif(SWITCH_TPYE == 2)
	mag->x = Tmag.z;
	mag->y = -Tmag.x;
	mag->z = -Tmag.y;
#endif
	mag_Old.x = mag->x;
	mag_Old.y = mag->y;
	mag_Old.z = mag->z;
	//printf("a:%d  b:%d  c:%d  \r\n",m_Mag_Bias[0],m_Mag_Bias[1],m_Mag_Bias[2]);
}

#if 0
 void magcalMPU9250(float * dest1, float * dest2) 
 {
 uint16_t ii = 0, sample_count = 0;
 int32_t mag_bias[3] = {0, 0, 0}, mag_scale[3] = {0, 0, 0};
 int16_t mag_max[3] = {-32767, -32767, -32767}, mag_min[3] = {32767, 32767, 32767}, mag_temp[3] = {0, 0, 0};

 Serial.println("Mag Calibration: Wave device in a figure eight until done!");
 delay(4000);

// shoot for ~fifteen seconds of mag data
if(MPU9250Mmode == 0x02) sample_count = 128;  // at 8 Hz ODR, new mag data is available every 125 ms
if(MPU9250Mmode == 0x06) sample_count = 1500;  // at 100 Hz ODR, new mag data is available every 10 ms
for(ii = 0; ii < sample_count; ii++) {
MPU9250readMagData(mag_temp);  // Read the mag data   
for (int jj = 0; jj < 3; jj++) {
  if(mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
  if(mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
}
if(MPU9250Mmode == 0x02) delay(135);  // at 8 Hz ODR, new mag data is available every 125 ms
if(MPU9250Mmode == 0x06) delay(12);  // at 100 Hz ODR, new mag data is available every 10 ms
}


// Get hard iron correction
 mag_bias[0]  = (mag_max[0] + mag_min[0])/2;  // get average x mag bias in counts
 mag_bias[1]  = (mag_max[1] + mag_min[1])/2;  // get average y mag bias in counts
 mag_bias[2]  = (mag_max[2] + mag_min[2])/2;  // get average z mag bias in counts

 dest1[0] = (float) mag_bias[0]*MPU9250mRes*MPU9250magCalibration[0];  // save mag biases in G for main program
 dest1[1] = (float) mag_bias[1]*MPU9250mRes*MPU9250magCalibration[1];   
 dest1[2] = (float) mag_bias[2]*MPU9250mRes*MPU9250magCalibration[2];  
   
// Get soft iron correction estimate
 mag_scale[0]  = (mag_max[0] - mag_min[0])/2;  // get average x axis max chord length in counts
 mag_scale[1]  = (mag_max[1] - mag_min[1])/2;  // get average y axis max chord length in counts
 mag_scale[2]  = (mag_max[2] - mag_min[2])/2;  // get average z axis max chord length in counts

 float avg_rad = mag_scale[0] + mag_scale[1] + mag_scale[2];
 avg_rad /= 3.0;

 dest2[0] = avg_rad/((float)mag_scale[0]);
 dest2[1] = avg_rad/((float)mag_scale[1]);
 dest2[2] = avg_rad/((float)mag_scale[2]);

 Serial.println("Mag Calibration done!");
 }
#endif

