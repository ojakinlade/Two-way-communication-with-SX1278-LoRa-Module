#pragma once

#include <SPI.h>
#include <LoRa.h>
#define MAX_BUFFER_SIZE  12
#define QUERY            0xAA
#define ACK_CODE         0xBB

enum FREQUENCY
{
  FREQ_433MHZ = 433000000,
  FREQ_868MHZ = 868000000,
  FREQ_915MHZ = 915000000
};

enum POWER_PARAM
{
  VOLTAGE = 0,
  CURRENT,
  PFACTOR,
  FREQ,
  POWER,
  ENERGY  
};
     
class LORA
{
  private:
    uint8_t chipSel;
    uint8_t resetPin;
    uint8_t IRQPin;
    uint8_t addr;
    uint8_t packetSize;
    uint8_t bufferSize; /*Size of data buffer containing power parameters*/
    uint8_t rxDataCounter;
    uint8_t txBuffer[MAX_BUFFER_SIZE]; 
    uint8_t rxBuffer[MAX_BUFFER_SIZE];
    
  public:
    LORA(uint8_t csPin, uint8_t rstPin, uint8_t irqPin, uint32_t freq, uint8_t deviceAddr);
    void SetBufferSize(uint8_t limit);
    /*Transmitter*/
    void EncodeData(uint16_t data,uint8_t dataID);
    void TransmitData(char* pData,uint8_t deviceAddr);
    void TransmitData(uint8_t deviceAddr);
    void TransmitAck(uint8_t deviceAddr);
    void TransmitQuery(uint8_t deviceAddr);
    /*Non-blocking Receiver*/
    bool ReceivedAck(void);
    bool ReceivedData(void);
    bool ReceivedQuery(void);
    uint16_t DecodeData(uint8_t dataID);
};
