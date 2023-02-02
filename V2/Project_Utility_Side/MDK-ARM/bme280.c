#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "i2c.h"
#include "gpio.h"
#include "bme280.h"

//Private enums
enum Register_address
{
	BME280_ADDR = 0x76,
	CALIB_00_25_ADDR = 0x88,
	CALIB_26_41_ADDR = 0xE1,
	CTRL_HUM_ADDR = 0xF2,
	CTRL_MEAS_ADDR = 0xF4,
	DATA_REG_ADDR = 0xF7
};

enum Bit_field
{
	CTRL_HUM_OSRS_H_OVR1 = 0x01,
	CTRL_HUM_OSRS_H_OVR2 = 0x02,
	CTRL_HUM_OSRS_H_OVR4 = 0x03,
	CTRL_HUM_OSRS_H_OVR8 = 0x04,
	CTRL_HUM_OSRS_H_OVR16 = 0x05,
	
	CTRL_MEAS_MODE_SLEEP = 0x00,
	CTRL_MEAS_MODE_FORCED1 = 0x01,
	CTRL_MEAS_MODE_FORCED2 = 0x02,
	CTRL_MEAS_MODE_NORMAL = 0x03,
	
	CTRL_MEAS_OSRS_T_OVR1 = 1<<5,
	CTRL_MEAS_OSRS_T_OVR2 = 2<<5,
	CTRL_MEAS_OSRS_T_OVR4 = 3<<5,
	CTRL_MEAS_OSRS_T_OVR8 = 4<<5,
	CTRL_MEAS_OSRS_T_OVR16 = 5<<5
};

//Private typedefs
typedef struct Compensation_parameter
{
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;
	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
	
}bme280_comp_param_t;

//Private defines
#define ADC_REGISTER_SIZE		8
#define CALIB_00_25_SIZE		26
#define CALIB_26_41_SIZE		16

//Private globals
static bme280_comp_param_t bme280;
static uint8_t rawAdcValue[ADC_REGISTER_SIZE];
static uint8_t bme280Calib00_25[CALIB_00_25_SIZE];
static uint8_t bme280Calib26_41[CALIB_26_41_SIZE]; 
static int32_t t_fine;

static void BME280_Store_Compensation_Parameters(uint8_t* calibReg1, 
																								 uint8_t* calibReg2, 
																								 bme280_comp_param_t* bme){
	/*
	Description:
	Function to store bme280 calibration data into a structure variable of type
	bme280_comp_param_t.
	
	Parameters:
	1.)calibReg1: pointer to calibration register 00 - 25.
	2.)calibReg2: pointer to calibration register 26 - 41.
	3.)bme: pointer to structure containing compensation parameters.
	
	Return:
	None.
	*/
	bme->dig_T1 = (calibReg1[0] | (calibReg1[1] << 8));
	bme->dig_T2 = (calibReg1[2] | (calibReg1[3] << 8));
	bme->dig_T3 = (calibReg1[4] | (calibReg1[5] << 8));
	bme->dig_H1 = calibReg1[25];
	bme->dig_H2 = (calibReg2[0] | (calibReg2[1] << 8));
	bme->dig_H3 = calibReg2[2];
	bme->dig_H4 = ((calibReg2[3] << 4) | (calibReg2[4] & 0x0F));
	bme->dig_H5 = (((calibReg2[4] & ~(0x0F)) >> 4) | (calibReg2[5] << 4));
	bme->dig_H6 = calibReg2[6];
}

void BME280_Init(void)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_LOW,
								 (GPIO_PIN6_OUTPUT_MODE_2MHZ | GPIO_PIN7_OUTPUT_MODE_2MHZ),
								 (GPIO_PIN6_ALT_FUNC_OPEN_DRAIN | GPIO_PIN7_ALT_FUNC_OPEN_DRAIN));
	
	I2C_Init(I2C1,
					 I2C_PERIPH_FREQ_8MHZ,
					 I2C_STANDARD_MODE_8MHZ_CCR, 
					 I2C_STANDARD_MODE_8MHZ_TRISE);
	
	I2C_ReadMultiByte(I2C1,BME280_ADDR, CALIB_00_25_ADDR, bme280Calib00_25, CALIB_00_25_SIZE);
	//read the first 7 calibration data of calib26_41 register of bme280
	I2C_ReadMultiByte(I2C1,BME280_ADDR, CALIB_26_41_ADDR, bme280Calib26_41,7);
	BME280_Store_Compensation_Parameters(bme280Calib00_25,bme280Calib26_41,&bme280);
}

void BME280_GetData(bme280_t* pSensorData)
{
	/*
	Description:
	Function to obtain compensated bme280 temperature and humidity readings.
	
	Parameters:
	1.)pSensorData: pointer to bme280_t structure containing temperature
	and humidity readings.
	
	Return:
	None
	
	Note:
	The actual temperature = sensor reading / 100 (see Bosch Sensortec BME280 datasheet).
	The actual humidity = sensor reading / 1024 (see Bosch Sensortec BME280 datasheet).
	*/
	I2C_WriteByte(I2C1,BME280_ADDR, CTRL_HUM_ADDR, CTRL_HUM_OSRS_H_OVR16);
	I2C_WriteByte(I2C1,BME280_ADDR, CTRL_MEAS_ADDR, (CTRL_MEAS_OSRS_T_OVR16 | CTRL_MEAS_MODE_FORCED1));
	I2C_ReadMultiByte(I2C1,BME280_ADDR, DATA_REG_ADDR, rawAdcValue, ADC_REGISTER_SIZE);
	
	//The calculations below are based on equations from the.....
	//Bosch Sensortec BME280 datasheet.
	
	//Temperature calculation
	int32_t adc_T, var1, var2, T;
	adc_T = ((rawAdcValue[3] << 12) | (rawAdcValue[4] << 4) | (rawAdcValue[5] >> 4));
	var1 = ((((adc_T>>3) - ((int32_t)bme280.dig_T1<<1))) * ((int32_t)bme280.dig_T2)) >> 11;
	
	var2 = (((((adc_T>>4) - ((int32_t)bme280.dig_T1)) * 
						((adc_T>>4) - ((int32_t)bme280.dig_T1))) >> 12) * ((int32_t)bme280.dig_T3)) >> 14;

	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	
	pSensorData->temperature = T / 100;
	
	//Humidity calculation
	int32_t adc_H, v_x1_u32r;
	adc_H = ((rawAdcValue[6] << 8) | rawAdcValue[7]);

	v_x1_u32r = (t_fine - ((uint32_t)76800));
	
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)bme280.dig_H4) << 20) -
              (((int32_t)bme280.dig_H5) * v_x1_u32r)) +
              ((int32_t)16384)) >> 15) *
              (((((((v_x1_u32r * ((int32_t)bme280.dig_H6)) >> 10) *
              (((v_x1_u32r * ((int32_t)bme280.dig_H3)) >> 11) +
              ((int32_t)32768))) >> 10) + ((int32_t)2097152)) *
              ((int32_t)bme280.dig_H2) + 8192) >> 14));
	
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)bme280.dig_H1)) >> 4));

	//change the ternary operators to if..else statements if you like
  v_x1_u32r = (v_x1_u32r < 0) ? 0 : v_x1_u32r;
  v_x1_u32r = (v_x1_u32r > 419430400) ? 419430400 : v_x1_u32r;
	
  uint32_t rawHumidity = (uint32_t)(v_x1_u32r >> 12);
	pSensorData->humidity = rawHumidity / 1024;
}
