#include "system.h"
#include "hc06.h"
#include "sx1278.h"
#include "bme280.h"
#include "lcd.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define TX					//Uncomment for Transmission
//#define RX				//Uncomment for Reception

char buf[20] = {0};
uint8_t bluetoothData[5] = {0};
uint8_t storedData[5] = {0};
bme280_t sensorData;


void storeRxData(uint8_t* mainBuf, uint8_t* rxBuffer)
{
	memcpy(mainBuf, rxBuffer, 5);
}

void Utility_Send(char* data)
{
	LoRa_beginPacket();
	LoRa_Transmit((uint8_t*)data, strlen(buf));
	LoRa_endPacket();
	HAL_Delay(1000);
}

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
	System_Init();
	HC06_Init(bluetoothData,5);
	LoRa_Init();
	BME280_Init();
	#ifdef TX
		sprintf(buf, "Smart Grid");
	#else
		uint8_t ret;
	#endif
	LCD_Init();
	LCD_Set_Cursor(0,3);
	LCD_Write_String("EMBEDDED");
	LCD_Set_Cursor(1,3);
	LCD_Write_String("SYSTEMS");
  while (1)
  {
		BME280_GetData(&sensorData);

//		if(HC06_DoneReceiving())
//		{
//			HC06_Reset();
//			storeRxData(storedData, bluetoothData);
//			HC06_Init(bluetoothData,5);
//		}
//		#ifdef TX
//			Utility_Send(buf);
//		#endif
		
		#ifdef RX
		ret = LoRa_parsePacket();
		if(ret)
		{
			uint8_t i = 0;
			while(LoRa_Available())
			{
				buf[i] = LoRa_Read();
				i++;
			}
			buf[i] = '\0';
		}
		#endif	
  }
}


