#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "system.h"
#include "button.h"

void Button_Init(void)
{
	GPIO_InputInit(GPIOA, GPIO_PORT_REG_LOW, GPIO_PIN5,
									GPIO_PIN5_INPUT_PULLUP_OR_PULLDOWN,
									GPIO_PULLUP_ENABLE);
	GPIO_OutputInit(GPIOC, GPIO_PORT_REG_HIGH, GPIO_PIN13_OUTPUT_MODE_2MHZ, GPIO_GEN_PUR_OUTPUT_PUSH_PULL);
}

static bool Debounce(Button_t* pButton)
{
	pButton->state = !(GPIO_InputRead(GPIOA, GPIO_PIN5));
	if(pButton->state == BUTTON_PRESSED)
	{
		System_TimerDelayMs(10);
		if(pButton->state == BUTTON_PRESSED)
		{
			return true;
		}
	}
	
	return false;
}

void Button_Poll(Button_t* pButton)
{
	pButton->isDebounced = Debounce(pButton);
}
