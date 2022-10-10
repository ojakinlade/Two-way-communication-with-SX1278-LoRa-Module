#ifndef _BME280_H
#define _BME280_H

typedef struct
{
	uint8_t temperature;
	uint8_t humidity;
}bme280_t;

//Initializes the BME280 humidity and temperature sensor
extern void BME280_Init(void);
/*
Obtains the value of humidity and temperature from the BME280
sensor and stores in a variable of type 'bme280_t'. A pointer
to this variable has to be passed into the function.
*/
extern void BME280_GetData(bme280_t* pSensorData);

#endif //_BME280_H
