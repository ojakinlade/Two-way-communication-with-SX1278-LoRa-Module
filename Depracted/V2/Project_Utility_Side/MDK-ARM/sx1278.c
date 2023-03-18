#include "stm32f1xx_hal.h"
#include "main.h"
#include "system.h"
#include "sx1278.h"

#define NSS_PORT		GPIOA
#define NSS_PIN			GPIO_PIN_4
#define RESET_PORT	GPIOB
#define RESET_PIN		GPIO_PIN_0

#define FREQ_433MHZ			0
#define FREQ_865MHZ			1
#define FREQ_866MHZ			2
#define FREQ_867MHZ			3

static const uint64_t FREQUENCY[4] = { 433E6, 865E6, 866E6, 867E6}; 



uint8_t packetIndex;

static uint8_t LoRa_readReg(uint8_t addr)
{
	uint8_t txByte = addr & 0x7F;
	uint8_t rxByte = 0;
	HAL_GPIO_WritePin(NSS_PORT, NSS_PIN, GPIO_PIN_RESET);
	HAL_SPI_Transmit(&hspi1, &txByte, 1, 1000);
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	
	HAL_SPI_Receive(&hspi1, &rxByte, 1, 1000);
	while(HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	HAL_GPIO_WritePin(NSS_PORT, NSS_PIN, GPIO_PIN_SET);
	return rxByte;
}

static void LoRa_writeReg(uint8_t addr, uint8_t cmd)
{
	uint8_t add = addr | 0x80;
  HAL_GPIO_WritePin(NSS_PORT, NSS_PIN, GPIO_PIN_RESET);
  HAL_SPI_Transmit(&hspi1, &add, 1, 1000);
	while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	HAL_SPI_Transmit(&hspi1, &cmd, 1, 1000);
	while (HAL_SPI_GetState(&hspi1) != HAL_SPI_STATE_READY);
	HAL_GPIO_WritePin(NSS_PORT, NSS_PIN, GPIO_PIN_SET);
}

uint8_t LoRa_Init(void)
{
	uint8_t ret;
	HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(RESET_PORT, RESET_PIN, GPIO_PIN_SET);
	
	ret = LoRa_readReg(REG_VERSION);
	if(ret != 0x12)
	{
		return 1;
	}
	LoRa_writeReg(REG_OP_MODE, (MODE_LONG_RANGE_MODE | MODE_SLEEP));
	LoRa_setFrequency(FREQUENCY[FREQ_433MHZ]);
	LoRa_writeReg(REG_FIFO_TX_BASE_ADDR, 0);
	LoRa_writeReg(REG_FIFO_RX_BASE_ADDR, 0);
	ret = LoRa_readReg(REG_LNA);
	LoRa_writeReg(REG_LNA, ret | 0x03);
	LoRa_writeReg(REG_MODEM_CONFIG_3, 0x04);
	LoRa_writeReg(REG_PA_CONFIG, 0x8F);
	LoRa_writeReg(REG_OP_MODE, (MODE_LONG_RANGE_MODE | MODE_STDBY));
	return 0;
}

int LoRa_parsePacket(void)
{
	int packetLength = 0;
	int irqFlags;
	
	irqFlags = LoRa_readReg(REG_IRQ_FLAGS);
	LoRa_writeReg(REG_IRQ_FLAGS, irqFlags);
	
	if((irqFlags & IRQ_RX_DONE_MASK) && ((irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK) == 0))
	{
		packetLength = LoRa_readReg(REG_RX_NB_BYTES);
		LoRa_writeReg(REG_FIFO_ADDR_PTR, LoRa_readReg(REG_FIFO_RX_CURRENT_ADDR));
		LoRa_writeReg(REG_OP_MODE, 0x81);
		packetIndex = 0;
	}
	else if((LoRa_readReg(REG_OP_MODE)) != (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE)){
		LoRa_writeReg(REG_FIFO_ADDR_PTR, 0);
		LoRa_writeReg(REG_OP_MODE, (MODE_LONG_RANGE_MODE | MODE_RX_SINGLE));
	}
	if((irqFlags & IRQ_PAYLOAD_CRC_ERROR_MASK)== IRQ_PAYLOAD_CRC_ERROR_MASK){
		return -1;
	}
	return packetLength;
}

uint8_t LoRa_Available(void)
{
	return (LoRa_readReg(REG_RX_NB_BYTES) - packetIndex);
}

uint8_t LoRa_Read(void)
{
	if(!LoRa_Available())
	{
		return 0;
	}
	packetIndex++;
	return LoRa_readReg(REG_FIFO);
}

uint8_t LoRa_beginPacket(void)
{
	if((LoRa_readReg(REG_OP_MODE) & MODE_TX) == MODE_TX)
	{
		return 1;
	}
	LoRa_writeReg(REG_OP_MODE, (MODE_LONG_RANGE_MODE | MODE_STDBY));
	LoRa_writeReg(REG_MODEM_CONFIG_1, 0x72);
	LoRa_writeReg(REG_FIFO_ADDR_PTR, 0);
	LoRa_writeReg(REG_PAYLOAD_LENGTH, 0);
	return 0;
}

void LoRa_Transmit(uint8_t* buf, uint8_t size)
{
	int currentLength = LoRa_readReg(REG_PAYLOAD_LENGTH);
	if((currentLength + size) > MAX_PKT_LENGTH)
	{
		size = MAX_PKT_LENGTH - currentLength;
	}
	for(int i = 0; i < size; i++)
	{
		LoRa_writeReg(REG_FIFO, buf[i]);
	}
	LoRa_writeReg(REG_PAYLOAD_LENGTH, currentLength + size);
}

uint8_t LoRa_endPacket(void)
{
	uint8_t timeout = 100;
	LoRa_writeReg(REG_OP_MODE, MODE_LONG_RANGE_MODE | MODE_TX);
	while((LoRa_readReg(REG_IRQ_FLAGS) & IRQ_TX_DONE_MASK) == 0)
	{
		if(--timeout == 0)
		{
			HAL_Delay(1);
			return 1;
		}
	}
	LoRa_writeReg(REG_IRQ_FLAGS, IRQ_TX_DONE_MASK);
	return 0;
}

void LoRa_setFrequency(uint64_t freq)
{
	uint64_t frf = ((uint64_t)freq << 19) / 32000000;
	LoRa_writeReg(REG_FRF_MSB, (uint8_t)(frf >> 16));
	LoRa_writeReg(REG_FRF_MID, (uint8_t)(frf >> 8));
	LoRa_writeReg(REG_FRF_LSB, (uint8_t)(frf >> 0));
}


