#ifndef INC_HC06_H
#define INC_HC06_H

enum ENERGY_SOURCES
{
	SOLAR = 0x31,
	WIND_TURBINE = 0x32,
	GENERATOR = 0x33
};

extern void HC06_Init(uint8_t* rxBuffer, uint8_t len);
extern bool HC06_DoneReceiving(void);
extern void HC06_Reset(void);

#endif /* INC_HC06_H */
