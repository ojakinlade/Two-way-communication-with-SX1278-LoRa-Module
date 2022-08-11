#include "system.h"
#include "hc06.h"

void HC06_Receive(uint8_t* rxBuffer)
{
	HAL_UART_Receive_DMA(&huart2, rxBuffer, 1);
}
