#include "system.h"
#include "hc06.h"
#include "sx1278.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

#define TX					//Uncomment for Transmission
//#define RX				//Uncomment for Reception

char buf[20] = {0};
uint8_t bluetoothData[5] = {0};
uint8_t storedData[5] = {0};


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
	#ifdef TX
		sprintf(buf, "Smart Grid");
	#else
		uint8_t ret;
	#endif
  while (1)
  {

		if(HC06_DoneReceiving())
		{
			HC06_Reset();
			storeRxData(storedData, bluetoothData);
			HC06_Init(bluetoothData,5);
		}
		#ifdef TX
			Utility_Send(buf);
		#endif
		
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


