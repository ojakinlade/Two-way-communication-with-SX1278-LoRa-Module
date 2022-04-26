#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "gpio.h"
#include "uart.h"
#include "dma.h"

#define IDLE_CHARACTER_ERROR 	255

void HC12_Init(void)
{
	//GPIO configuration for USART1 Tx
	GPIO_OutputInit(GPIOA,
									GPIO_PORT_REG_HIGH,
									GPIO_PIN9_OUTPUT_MODE_2MHZ,
									GPIO_PIN9_ALT_FUNC_PUSH_PULL);	
	//GPIO configuration for USART1 Rx
	GPIO_InputInit(GPIOA,
								 GPIO_PORT_REG_HIGH,
								 GPIO_PIN10,
								 GPIO_PIN10_INPUT_PULLUP_OR_PULLDOWN,
								 GPIO_PULLUP_ENABLE);	
	//USART1 configuration	
	USART_Init(USART1,
						 BAUD_9600,
						 RX_DMA_ENABLE,
						 (USART_TX_ENABLE | USART_RX_ENABLE));
}

void HC12_RxBufferInit(uint8_t* pBuffer, uint8_t bufferSize)
{
	//DMA1 channel 5 configuration for USART1 Rx
	DMA_USART_Rx_Init(DMA1_Channel5,
										USART1,
										pBuffer,
										bufferSize, 
										DMA_CHANNEL5_MEMORY_INC_MODE |
										DMA_CHANNEL5_CIRCULAR_BUFFER |
										DMA_CHANNEL_ENABLE);
}



void HC12_TransmitBytes(uint8_t* bytes, uint8_t len)
{
	/*
	Description:
	
	Parameters:
	
	Return:
	
	*/
	USART_TransmitBytes(USART1,bytes,len);
}

void HC12_TransmitByte(uint8_t pData)
{
	USART_TransmitByte(USART1, pData);
}

bool HC12_Rx_BufferFull(void)
{
	return DMA_Rx_BufferFull(DMA1, DMA_CHANNEL5);
}

//void HC12_RxBufferErrorHandler(uint8_t* pData)
//{
//	if(pData[0] == IDLE_CHARACTER_ERROR)
//	{
//		pData[0] = 0;
//		HC12_RxBufferInit(pData, 5);
//	}
//}

bool HC12_IncompleteRxData(void)
{
	if(USART_RxIdleLineDetected(USART1))
	{
		return true;
	}
	return false;
}
