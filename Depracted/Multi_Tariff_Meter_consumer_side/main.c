#include <stdbool.h>
#include <stdint.h>
#include "system.h"
#include "hc12.h"
#include "lcd.h"
#include "string.h"

uint8_t discoData[5] = {0};
uint8_t auxBuf[5] = {0};

sysTimer_t timer1;
sysTimer_t timer2;

int main(void)
{
	System_Init();
	System_TimerInit(&timer1, 10000); //Increments energy every 60 seconds
	System_TimerInit(&timer2, 10000);
	LCD_Init();
	HC12_Init();
	HC12_RxBufferInit(discoData, 5);
		
	while(1)
	{
		
		if(HC12_Rx_BufferFull())
		{
			System_TimerDelayMs(100);
			strcpy((char*)auxBuf, (char*)discoData);
		}
		
		if(HC12_IncompleteRxData() || (discoData[0] == IDLE_CHARACTER))
		{
			HC12_RxBufferInit(discoData, 5);
		}	
			
		if(!strcmp((char*)auxBuf, "Hello"))
		{
			static uint32_t energy_consumed;
			//timer2.isCounting = true;
			LCD_Set_Cursor(0,0);
			LCD_Write_String("Tariff 1");
			LCD_Set_Cursor(1,0);
			LCD_Write_String("Energy(KWH): ");
			LCD_Write_Int(energy_consumed);
			if(System_TimerDoneCounting(&timer1))
			{
				energy_consumed++;
			}
		}
		
		else if(!strcmp((char*)auxBuf, "Mikun"))
		{
			static uint32_t energy_consumed;
			//timer1.isCounting = true;
			LCD_Set_Cursor(0,0);
			LCD_Write_String("Tariff 2");
			LCD_Set_Cursor(1,0);
			LCD_Write_String("Energy(KWH): ");
			LCD_Write_Int(energy_consumed);
			if(System_TimerDoneCounting(&timer2))
			{
				energy_consumed++;
			}
		}
	}
}
