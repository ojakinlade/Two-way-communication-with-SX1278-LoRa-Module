#include "system.h"
#include <string.h>
#include "hc06.h"

static bool RxBufferFull = false;

void HC06_Init(uint8_t* rxBuffer, uint8_t len)
{
	memset(rxBuffer, 0, 5);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart2, rxBuffer, len);
	__HAL_DMA_DISABLE_IT(&hdma_usart2_rx, DMA_IT_HT);
}

bool HC06_DoneReceiving(void)
{
	return RxBufferFull;
}

void HC06_Reset(void)
{
	RxBufferFull = false;
}

void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance == USART2)
	{
		RxBufferFull = true;
	}
}
