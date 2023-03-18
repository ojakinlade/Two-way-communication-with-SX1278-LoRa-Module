#include <stdint.h>
#include <stdbool.h>
#include "system.h"
#include "lcd.h"

static sysTimer_t timer;
uint8_t ticks = 0;

void Display_UpdateTime(uint8_t* tick)
{
	static uint8_t prevTick;
	if(prevTick != *tick)
	{
		LCD_Set_Cursor(0,5);
		if((*tick % 24) < 10)
		{
			LCD_Write_Data('0');
			LCD_Write_Int(*tick % 24);
		}
		else
		{
			LCD_Write_Int(*tick % 24);
		}
		LCD_Write_String(":00");
	}
}

int main(void)
{
	System_Init();
	System_TimerInit(&timer, 5000);
	LCD_Init();
	LCD_Set_Cursor(0,0);
	LCD_Write_String("Time:");
	LCD_Set_Cursor(1,0);
	LCD_Write_String("Source:");
	LCD_Set_Cursor(0,5);
	LCD_Write_String("00");
	LCD_Write_String(":00");
	LCD_Set_Cursor(1,7);
	LCD_Write_String("Utility");


	while(1)
	{
		if(System_TimerDoneCounting(&timer))
		{
			ticks++;
			Display_UpdateTime(&ticks);
			if((ticks % 24) >= 0 && (ticks % 24) <= 6)
			{
				LCD_Set_Cursor(1,7);
				LCD_Write_String("         ");
				LCD_Set_Cursor(1,7);
				LCD_Write_String("Utility");
			}
			if((ticks % 24) > 6 && (ticks % 24) <= 12)
			{
				LCD_Set_Cursor(1,7);
				LCD_Write_String("         ");
				LCD_Set_Cursor(1,7);
				LCD_Write_String("Generator");
			}
			if((ticks % 24) > 12 && (ticks % 24) <= 16)
			{
				LCD_Set_Cursor(1,7);
				LCD_Write_String("         ");
				LCD_Set_Cursor(1,7);
				LCD_Write_String("Solar");
			}
			if((ticks % 24) > 16 && (ticks % 24) <= 20)
			{
				LCD_Set_Cursor(1,7);
				LCD_Write_String("         ");
				LCD_Set_Cursor(1,7);
				LCD_Write_String("Wind");
			}
			if((ticks % 24) > 20 && (ticks % 24) <= 23)
			{
				LCD_Set_Cursor(1,7);
				LCD_Write_String("         ");
				LCD_Set_Cursor(1,7);
				LCD_Write_String("Generator");
			}
		}	
	}
	
}
