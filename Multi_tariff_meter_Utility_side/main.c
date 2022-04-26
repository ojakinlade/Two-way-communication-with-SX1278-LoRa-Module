#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "system.h"
#include "hc12.h"
#include "button.h"

Button_t BUTTON = {BUTTON1, BUTTON_NOT_PRESSED, false};
uint8_t dataToSend[5] = "Hello"; 

int main(void)
{
	System_Init();
	Button_Init();
	HC12_Init();
	
	while(1)
	{
		Button_Poll(&BUTTON);
		
		if(BUTTON.isDebounced && !BUTTON.prevPressed)
		{
			BUTTON.prevPressed = true;
			GPIO_OutputWrite(GPIOC, GPIO_PIN13, true);
			HC12_TransmitBytes(dataToSend, 5);
			
		}
		else if(!BUTTON.isDebounced && BUTTON.prevPressed)
		{
			GPIO_OutputWrite(GPIOC, GPIO_PIN13, false);
			BUTTON.prevPressed = false;
		}
	}
}

