#ifndef _SYSTEM_H
#define _SYSTEM_H

/**
@brief System timer structure. Contains attributes for  
providing timing services to the application.  
@member isCounting: is set to true if the sysTimer_t timer is 
currently counting. 
@member start: is the tick at which the sysTimer_t timer starts counting.  
@member ticksToWait: number of ticks required for the sysTimer_t timer to count.   
*/
typedef struct
{
	bool isCounting; 
	uint32_t start; 
	uint32_t ticksToWait; 
}sysTimer_t;

extern void System_Init(void);
extern void System_TimerDelayMs(uint32_t delayTime);
extern void System_TimerInit(sysTimer_t* pSysTimer, uint32_t timerRepTime);
extern bool System_TimerDoneCounting(sysTimer_t* pSysTimer);
extern void System_ClearStandbyFlag(void);
extern void System_GoToStandbyMode(void);

#endif //_SYSTEM_H
