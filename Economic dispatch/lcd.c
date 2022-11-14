#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include <stdio.h>
#include "systick.h"
#include "gpio.h"
#include "i2c.h"
#include "lcd.h"


#define PCF8574_ADDR													0x27
#define REGISTER_ADDR													0x00
#define CMD_FUNCTION_SET_8BIT									0x03 
#define CMD_FUNCTION_SET_4BIT									0x02 
#define CMD_FUNCTION_SET_2LINE_5X8_DOT				0x28 
#define CMD_CLEAR_DISPLAY											0x01
#define CMD_DISPLAY_ON_CURSOR_ON							0x0E
#define CMD_DISPLAY_ON_CURSOR_BLINK						0x0F
#define CMD_DISPLAY_ON_CURSOR_OFF							0x0C
#define CMD_ENTRY_MODE_INCREMENT_CURSOR				0x06

/*Two-dimensional array to map column and row
combinations to 16x2 LCD DDRAM addresses. (see HD44780 datasheet)
*/
const uint8_t ddramAddr[2][16] = {{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F},
																	{0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4A,0x4B,0x4C,0x4D,0x4E,0x4F}};

	


void LCD_Write_Data( char byte)
{
	/*
	@brief:
	@param:
	@return:
	*/
	char lowNibble = byte << 4;
	uint8_t data_t[4] = {0};
	data_t[0] = (byte & 0xF0) | 0x0D; 		// EN=1; RS=1
	data_t[1] = (byte & 0xF0) | 0x09; 		// EN=0; RS=1
	data_t[2] = (lowNibble & 0xF0) | 0x0D;	// EN=1; RS=1 
	data_t[3] = (lowNibble & 0xF0) | 0x09;	// EN=0; RS=1
	
	I2C_WriteByte(I2C1, PCF8574_ADDR, REGISTER_ADDR, data_t[0]);
	SysTick_DelayMs(1);
	I2C_WriteByte(I2C1, PCF8574_ADDR, REGISTER_ADDR, data_t[1]);
	SysTick_DelayMs(1);
	I2C_WriteByte(I2C1, PCF8574_ADDR, REGISTER_ADDR, data_t[2]);
	SysTick_DelayMs(1);
	I2C_WriteByte(I2C1, PCF8574_ADDR, REGISTER_ADDR, data_t[3]);
	SysTick_DelayMs(1);
	
}

static void LCD_Write_Cmd(char cmd)
{
	/*
	@brief:
	@param:
	@return:
	*/
	char lowNibble = cmd << 4;
	uint8_t data_t[4] = {0};
	data_t[0] = (cmd & 0xF0) | 0x0C; 				// EN=1; RS=0
	data_t[1] = (cmd & 0xF0) | 0x08; 				// EN=0; RS=0
	data_t[2] = (lowNibble & 0xF0) | 0x0C;	// EN=1; RS=0 
	data_t[3] = (lowNibble & 0xF0) | 0x08;	// EN=0; RS=0
	
	I2C_WriteByte(I2C1, PCF8574_ADDR, REGISTER_ADDR, data_t[0]);
	SysTick_DelayMs(1);
	I2C_WriteByte(I2C1, PCF8574_ADDR, REGISTER_ADDR, data_t[1]);
	SysTick_DelayMs(1);
	I2C_WriteByte(I2C1, PCF8574_ADDR, REGISTER_ADDR, data_t[2]);
	SysTick_DelayMs(1);
	I2C_WriteByte(I2C1, PCF8574_ADDR, REGISTER_ADDR, data_t[3]);
	SysTick_DelayMs(1);
	
}

void LCD_Init(void)
{
	//I2C Initialization
	GPIO_OutputInit(GPIOB,
									GPIO_PORT_REG_LOW,
								 (GPIO_PIN6_OUTPUT_MODE_2MHZ | GPIO_PIN7_OUTPUT_MODE_2MHZ),
								 (GPIO_PIN6_ALT_FUNC_OPEN_DRAIN | GPIO_PIN7_ALT_FUNC_OPEN_DRAIN));
	
	I2C_Init(I2C1,
					 I2C_PERIPH_FREQ_8MHZ,
					 I2C_STANDARD_MODE_8MHZ_CCR, 
					 I2C_STANDARD_MODE_8MHZ_TRISE);
	
	//LCD Init Sequence
	SysTick_DelayMs(16); //wait for more than 15ms after power-on
	LCD_Write_Cmd(CMD_FUNCTION_SET_8BIT);
	SysTick_DelayMs(5);	 //Wait for more than 4.1ms
	LCD_Write_Cmd(CMD_FUNCTION_SET_8BIT);
	SysTick_DelayMs(1);	 //Wait for more than 100us
	LCD_Write_Cmd(CMD_FUNCTION_SET_8BIT);
	
	//Select all init commands for 4-bit interface
	LCD_Write_Cmd(CMD_FUNCTION_SET_4BIT);
	LCD_Write_Cmd(CMD_FUNCTION_SET_2LINE_5X8_DOT);
	LCD_Write_Cmd(CMD_CLEAR_DISPLAY);
	LCD_Write_Cmd(CMD_DISPLAY_ON_CURSOR_OFF);
	LCD_Write_Cmd(CMD_ENTRY_MODE_INCREMENT_CURSOR);
	LCD_Write_Cmd(CMD_DISPLAY_ON_CURSOR_BLINK);

}

void LCD_Write_String(char* pData)
{
	while(*pData != '\0')
	{
		LCD_Write_Data(*pData);
		pData++;
	}
}

void LCD_Clear(void)
{
	LCD_Write_Cmd(CMD_CLEAR_DISPLAY);
}

void LCD_Set_Cursor(uint8_t row, uint8_t column)
{
	if((row > 1) || (column > 16))
	{
		return;
	}
	
	LCD_Write_Cmd(((1<<7) | ddramAddr[row][column]));
}

void LCD_Write_Int(uint32_t data)
{
	char intToChar[10];
	sprintf(intToChar, "%d", data);
	LCD_Write_String(intToChar);
	
}
