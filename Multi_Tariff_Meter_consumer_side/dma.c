#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "dma.h"
/**
@brief Initializes a USART Rx DMA channel.  
@param dmaChannel: pointer to DMA_Channel_TypeDef struct which contains  
all registers for the desired DMA channel e.g. if dmaChannel is passed an   
argument of DMA1_Channel5, DMA1 Channel 5 is configured and if passed an   
argument of DMA2_Channel1, DMA2 Channel 1 is configured.

@param uartPort: pointer to USART_TypeDef struct which contains all registers   
for the desired USART peripheral e.g. if uartPort is passed an argument of USART1,   
the USART1 peripheral is configured and if passed an argument of USART2, the USART2   
peripheral is configured etc.

@param uartRxBuffer: pointer to first element of character array (or buffer) which stores
data from the configured USART's receiver via the configured DMA channel. e.g.   
if dmaChannel = DMA1_Channel5 and uartPort = USART1, USART1 receives multiple byte data which  
will be directly transferred to memory(i.e. uartRxBuffer) via DMA1 channel 5.

@param bufferSize: size of uartRxBuffer (i.e. number of bytes or characters to receive from USART Rx  
via DMA transfer). e.g. if this parameter is passed with an argument of 25, it means 25 bytes  
of data will be directly transferred from the USART Rx to memory (i.e. uartRxBuffer).  

@param dmaConfig: this parameter enables different types of configurations for a DMA channel.
The possible arguments that can be passed to this parameter are defined in the dma.h header
and they can be logically ORed depending on the desired configuration. Some of these 
arguments (in generic form) include:

* DMA_CHANNELX_MEMORY_INC_MODE: Enables memory increment mode for channel 'x' of a DMA peripheral.
* DMA_CHANNELX_CIRCULAR_BUFFER: Enables circular buffer mode for channel 'x' of a DMA peripheral.  
* DMA_CHANNELX_ENABLE: Enables channel 'x' of a DMA peripheral.  

@return None
*/ 
void DMA_USART_Rx_Init(DMA_Channel_TypeDef* dmaChannel,
											 USART_TypeDef* uartPort,
											 uint8_t* uartRxBuffer, 
											 uint8_t bufferSize,
											 uint32_t dmaConfig)
{
	volatile uint32_t* pUart_DR = (uint32_t*)&uartPort->DR;
	dmaChannel->CCR &= ~dmaConfig;
	dmaChannel->CPAR = (uint32_t)pUart_DR;
	dmaChannel->CMAR = (uint32_t)uartRxBuffer;
	dmaChannel->CNDTR = bufferSize;
	dmaChannel->CCR |= dmaConfig;
}

/**
@brief Signals if the Rx buffer of a DMA channel is full.  
@param dmaPort: e.g. DMA1,DMA2
@param dmaChannel: The channel of a particular DMA port e.g. DMA1 channel 5.  
@return true if buffer is full and false if otherwise.  
*/
bool DMA_Rx_BufferFull(DMA_TypeDef* dmaPort, uint8_t dmaChannel)
{
	bool bufferIsFull = false;
	uint8_t bitLocation = (dmaChannel * 4) - 3;
	
	if ( (dmaPort->ISR & (1<<bitLocation)) == (1<<bitLocation) )
	{
		bufferIsFull = true;
		dmaPort->IFCR |= (1<<bitLocation);
	}
	return bufferIsFull;
}

/**
@brief Gets the value in CNDTR register of a DMA channel. This value is  
the number of bytes left to be received.  
@param dmaChannel: The channel of a particular DMA port e.g. DMA1 channel 5.  
@return number of bytes left to be received by the Rx buffer of a DMA channel.   
*/
uint8_t DMA_Rx_CNDTR(DMA_Channel_TypeDef* dmaChannel)
{
	return dmaChannel->CNDTR;
}
