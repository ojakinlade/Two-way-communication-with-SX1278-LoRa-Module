#include "stm32f10x.h"                  // Device header
#include "systick.h"

static volatile uint32_t numberOfTicks = 0;

void SysTick_Init(void)
{
	/*
	Description:
	Initializes systick timer
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	
	//Systick default clock source = AHB/8 = 1MHz
	SysTick->LOAD = 1000 - 1; //1ms timebase
	SysTick->VAL = 0;
	SysTick->CTRL |= (SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk); //Enable systick interrupt and systick timer
}

uint32_t SysTick_GetTick(void)
{
	/*
	Description:
	Gets current tick (or count or clock cycle) of the systick timer.
	The tick is equal to the value of the 'numberOfTicks' variable which
	is increment by the systick interrupt service routine.
	
	Parameters:
	None
	
	Return:
	Current tick
	
	*/
	return numberOfTicks;
}

void SysTick_DelayMs(uint32_t delayTime)
{
	/*
	Description:
	Creates a delay in milliseconds. This delay is a busy-wait
	mechanism hence it blocks other tasks from being executed
	while the CPU is waiting for the delay time to elapse
	
	Parameters:
	1.) delayTime: time in milliseconds through which the delay
	lasts.
	
	Return:
	None
	
	*/
	uint32_t startTick = numberOfTicks;
	while((numberOfTicks - startTick) < delayTime){}
}

void SysTick_Handler(void)
{
	/*
	Description:
	Systick timer interrupt service routine
	
	Parameters:
	None
	
	Return:
	None
	
	*/
	numberOfTicks++;
}
