#include "stm32f10x.h"                  // Device header
#include <stdbool.h>
#include "uart.h"

/**
@brief Initializes USART peripheral.  
@param uartPort: pointer to USART_TypeDef struct which contains  
all registers for the desired USART peripheral e.g. if uartPort   
is passed an argument of USART1, the USART1 peripheral is configured and  
if passed an argument of USART2, the USART2 peripheral is configured etc.  

@param baud: configures the baud rate of the USART peripheral. If this parameter  
is passed BAUD_9600, the baud rate is set to 9600 baud.  

@param dmaMode: enables DMA mode for transmission or reception. 

@param uartMode: enables USART for transmission or reception. If this parameter is passed   
USART_TX_ENABLE, the USART transmitter is enabled and if passed with USART_RX_ENABLE, the USART   
receiver is enabled. If both transmitter and receiver are required, the USART_TX_ENABLE and   
USART_RX_ENABLE must be logically ORed.  

@return None  
*/
void USART_Init(USART_TypeDef* uartPort,
								uint32_t baud,
								uint8_t dmaMode,
								uint8_t uartMode)
{
	uartPort->CR1 |= USART_CR1_UE;
	uartPort->CR1 &= ~uartMode;
	switch (dmaMode)
	{
		case TX_RX_DMA_DISABLE:
			uartPort->CR3 &= ~(USART_CR3_DMAT | USART_CR3_DMAR);
			break;
		case RX_DMA_ENABLE:
			uartPort->CR3 |= USART_CR3_DMAR;
			break;
	}
	switch (baud)
	{
		case BAUD_2400:
			uartPort->BRR = BRR_2400;
			break;
		case BAUD_9600:
			uartPort->BRR = BRR_9600;
			break;
		case BAUD_115200:
			uartPort->BRR = BRR_115200;
			break;
	}
	uartPort->CR1 |= uartMode;
}

/**
@brief Transmits a byte of data through the Tx pin of the configured USART peripheral.  
@param uartPort: pointer to USART_TypeDef struct which contains all registers for the   
desired USART peripheral e.g. if uartPort is passed an argument of USART1, the USART1 peripheral  
is configured and if passed an argument of USART2, the USART2 peripheral is configured etc.  
@param byte: Data to be transmitted. The data to be transmitted
is a byte.
@return None
*/
void USART_TransmitByte(USART_TypeDef* uartPort, uint8_t byte)
{
	while ((uartPort->SR & USART_SR_TXE) != USART_SR_TXE);
	uartPort->DR = byte;
}

/**
@brief Transmits multiple bytes of data through the Tx pin of the configured USART peripheral.  
@param uartPort: pointer to USART_TypeDef struct which contains all registers for the desired   
USART peripheral e.g. if uartPort is passed an argument of USART1, the USART1 peripheral  
is configured and if passed an argument of USART2, the USART2 peripheral is configured etc.  
@param bytes: pointer to data array to be transmitted. The data to be transmitted is an array of bytes.    
@param len: number of bytes in the data array to be transmitted.  
@return None  
*/
void USART_TransmitBytes(USART_TypeDef* uartPort, uint8_t* bytes, uint8_t len)
{
	uint8_t i = 0;
	
	while (i < len)
	{
		while ((uartPort->SR & USART_SR_TXE) != USART_SR_TXE);
		uartPort->DR = bytes[i];
		i++;
	}
	while((uartPort->SR & USART_SR_TC) != USART_SR_TC );
}

/**
@brief Indicates if a USART's data register contains data or not.  
@param uartPort: pointer to USART_TypeDef struct which contains all registers  
for the desired USART peripheral.  
@return true if an IDLE line is detected during data reception.  
*/
bool USART_RxIdleLineDetected(USART_TypeDef* uartPort)
{
	bool idleLineDetected = false;
	if((uartPort->SR & USART_SR_IDLE) == USART_SR_IDLE)
	{
		idleLineDetected = true;
		volatile uint8_t readDR = uartPort->DR;
	}
	return idleLineDetected;
}

