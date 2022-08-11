#ifndef INC_SYSTEM_H
#define INC_SYSTEM_H

#include "main.h"

extern SPI_HandleTypeDef hspi1;
extern UART_HandleTypeDef huart2;
extern DMA_HandleTypeDef hdma_usart2_rx;


extern void System_Init(void);


#endif /*INC_SYSTEM_H*/
