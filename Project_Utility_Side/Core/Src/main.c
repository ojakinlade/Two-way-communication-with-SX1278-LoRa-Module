#include "system.h"
#include "hc06.h"
#include "sx1278.h"
#include "bme280.h"
#include "lcd.h"
#include <string.h>
#include <stdio.h>
#include <stdbool.h>

typedef enum 
{
	MOD_MANUAL = 0,
	MOD_AUTOMATIC
}mode_t;

char buf[20] = {0};
uint8_t bluetoothData[5] = {0};
uint8_t storedData[5] = {0};
bme280_t sensorData;
mode_t mode = MOD_MANUAL;


void storeRxData(uint8_t* mainBuf, uint8_t* rxBuffer)
{
	memcpy(mainBuf, rxBuffer, 5);
}

void Utility_SendToConsumer(char* data)
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
	LCD_Init();
	LCD_Set_Cursor(0,2);
	LCD_Write_String("HELLO!!!!");
	HAL_Delay(2000);
	LCD_Set_Cursor(1,5);
	LCD_Write_String("WELCOME");
	HAL_Delay(2000);
	LCD_Clear();
	LCD_Set_Cursor(0,2);
	LCD_Write_String("MULTI SOURCE");
	LCD_Set_Cursor(1,2);
	LCD_Write_String("ENERGY METER");
	HAL_Delay(3000);
	LCD_Clear();
  while (1)
  {
		switch(mode)
		{
			case MOD_MANUAL:
				LCD_Set_Cursor(0,0);
				LCD_Write_String("Energy Source:");
				if(HC06_DoneReceiving())
				{
					HC06_Reset();
					storeRxData(storedData, bluetoothData);
					HC06_Init(bluetoothData,5);
					if(!(strcmp((const char*)storedData, "1")))
					{
						LCD_Clear();
						LCD_Set_Cursor(0,0);
						LCD_Write_String("Energy Source:");
						LCD_Set_Cursor(1,0);
						LCD_Write_String("SOLAR");
						Utility_SendToConsumer("SOLAR");
					}
					if(!(strcmp((const char*)storedData, "2")))
					{
						LCD_Clear();
						LCD_Set_Cursor(0,0);
						LCD_Write_String("Energy Source:");
						LCD_Set_Cursor(1,0);
						LCD_Write_String("WIND TURBINE");
						Utility_SendToConsumer("WIND TURBINE");
					}
					if(!(strcmp((const char*)storedData, "3")))
					{
						LCD_Clear();
						LCD_Set_Cursor(0,0);
						LCD_Write_String("Energy Source:");
						LCD_Set_Cursor(1,0);
						LCD_Write_String("GENERATOR");
						Utility_SendToConsumer("GENERATOR");
					}	
				}
			break;
			
			case MOD_AUTOMATIC:
				BME280_GetData(&sensorData);
				LCD_Set_Cursor(0,0);
				LCD_Write_String("TEMP:");
				LCD_Set_Cursor(0,5);
				LCD_Write_Int(sensorData.temperature);
				LCD_Set_Cursor(0,8);
				LCD_Write_String("HUM:");
				LCD_Set_Cursor(0,12);
				LCD_Write_Int(sensorData.humidity);
				LCD_Set_Cursor(1,0);
				LCD_Write_String("POW FROM:");
			break;		
		}	
  }
}


