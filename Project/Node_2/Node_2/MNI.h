#pragma once

#include <SPI.h>
#include <LoRa.h>

enum FREQUENCY
{
  FREQ_433MHZ = 433000000,
  FREQ_868MHZ = 868000000,
  FREQ_915MHZ = 915000000
};
     
class MNI
{
  private:
    enum BufferSize{TX = 6, RX = 4};
    uint8_t chipSel;
    uint8_t resetPin;
    uint8_t IRQPin;
    uint8_t addr;
    uint8_t packetSize;
    uint8_t rxDataCounter;
    uint8_t txBuffer[BufferSize::TX]; 
    uint8_t rxBuffer[BufferSize::RX];
    
  public:
    enum{utilityAddr = 0xFF, node1Addr = 0xCC, 
         localAddr = 0xDD, QUERY = 0xAA, ACK = 0xBB};
    enum TxDataId
    {
      DATA_ACK = 0,
      TX_NODE_ADDR = 2,
      POWER = 4
    };
    enum RxDataId{NODE_ADDR = 0, DATA_QUERY = 2};
    MNI(uint8_t csPin, uint8_t rstPin, uint8_t irqPin, uint32_t freq, uint8_t deviceAddr);
    void EncodeData(uint16_t dataToEncode,TxDataId id);
    void TransmitData(void);
    bool ReceivedData(void);
    uint16_t DecodeData(RxDataId id);
};
