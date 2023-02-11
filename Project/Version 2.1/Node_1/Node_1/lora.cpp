#include <SPI.h>
#include <LoRa.h>
#include "lora.h"

/**
 * @brief Initialize LoRa transceiver.  
*/
LORA::LORA(uint8_t csPin, uint8_t rstPin, uint8_t irqPin, uint32_t freq, uint8_t deviceAddr)
{
  //Initialize private variables
  chipSel = csPin;
  resetPin = rstPin;
  IRQPin = irqPin;
  addr = deviceAddr;
  packetSize = 0;
  LoRa.setPins(chipSel, resetPin, IRQPin);// set CS, reset, IRQ pin
  //LoRa.begin(freq);
  bufferSize = MAX_BUFFER_SIZE;
  rxDataCounter = 0;
  for(uint8_t i = 0; i < bufferSize; i++)
  {
    txBuffer[i] = 0;
    rxBuffer[i] = 0;
  }
}

/**
 * @brief Sets the size of 'txBuffer' for an LoRa transmitter (node) OR  
 * that of the 'rxBuffer' for a LoRa receiver (master).  
 * @param limit: buffer size limit.
*/
void LORA::SetBufferSize(uint8_t limit)
{
  if(limit < MAX_BUFFER_SIZE)
  {
    bufferSize = limit;
  }
  else
  {
    bufferSize = MAX_BUFFER_SIZE;
  }
}

/**
 * @brief Encodes data (i.e. power measurements) into the 'txBuffer' before 
 * transmission to a LoRa receiver can occur.  
 * @param data:   data to be encoded (e.g. voltage, current).  
 * @param dataID: location of specific data within 'txBuffer' to be encoded.
*/
void LORA::EncodeData(uint16_t data,uint8_t dataID)
{
  dataID %= bufferSize / 2;
  //split 16-bit data into 2 bytes and store in the 'txBuffer'
  txBuffer[2*dataID] = (data & 0xFF00) >> 8; //high byte
  txBuffer[2*dataID + 1] = data & 0xFF; //low byte
}

/**
 * @brief Sends data to a LoRa receiver (i.e. master device).  
 * @param pData: Pointer to data to be sent to LoRa receiver
 * @param deviceAddr: Address of LoRa receiver
*/
void LORA::TransmitData(char* pData, uint8_t deviceAddr)
{
    LoRa.beginPacket();
    LoRa.write(deviceAddr);
    LoRa.write(addr);
    LoRa.print(pData);
    LoRa.endPacket();
    delay(100);
}

/**
 * @brief Sends data to a LoRa receiver (i.e. master device).  
 * @param pData: Pointer to data to be sent to LoRa receiver
 * @param deviceAddr: Address of LoRa receiver
*/
void LORA::TransmitData(uint8_t deviceAddr)
{
  Serial.print("Tx buffer: ");
  for(uint8_t i = 0; i < bufferSize; i++)
  {
    LoRa.beginPacket();
    LoRa.write(deviceAddr);
    LoRa.write(addr);
    LoRa.write(txBuffer[i]);
    Serial.print(txBuffer[i]);
    Serial.print(' ');
    LoRa.endPacket();
    delay(100); 
  }
  Serial.print("\n");
}

/**
 * @brief Sends an ACK to a LoRa data transmitter (i.e. a node).  
 * @param deviceAddr: Address of LoRa receiver
*/
void LORA::TransmitAck(uint8_t deviceAddr)
{
  LoRa.beginPacket();
  LoRa.write(deviceAddr);
  LoRa.write(addr);
  LoRa.write(ACK_CODE);
  LoRa.endPacket();
  delay(100);
}

/**
 * @brief Sends a Query (request for data) to a LoRa transmitter (node).  
*/
void LORA::TransmitQuery(uint8_t deviceAddr)
{
  LoRa.beginPacket();
  LoRa.write(deviceAddr);
  LoRa.write(addr);
  LoRa.write(QUERY);
  LoRa.endPacket();
  delay(100);  
}

/**
 * @brief Returns true if the LoRa receiver acknowledges data from the  
 * transmitter. (e.g. master device sends an ACK to the node, the node sends power measurements 
 * to the master).  
*/
bool LORA::ReceivedAck(void)
{
  uint8_t ack = 0;
  packetSize = LoRa.parsePacket();
  if(packetSize == 0)
  {
    return false;
  }
  int recipient = LoRa.read();
  byte sender = LoRa.read();
  ack = (uint8_t)LoRa.read(); 
  return (ack == ACK_CODE);
}

/**
 * @brief  Receives data from a LoRa transmitter. 
 * @return true:  if data reception is complete,
 *         false: if otherwise.
*/
bool LORA::ReceivedData(void)
{
  bool rxDone = false;
  packetSize = LoRa.parsePacket();   
  if(packetSize != 0)
  {
    int recipient = LoRa.read();
    byte sender = LoRa.read();
    
    if(LoRa.available())
    {
      if(rxDataCounter < bufferSize)
      {
        rxBuffer[rxDataCounter] = LoRa.read();
        rxDataCounter++;
      }
    }
    if(rxDataCounter == bufferSize)
    {
      rxDataCounter = 0;
      rxDone = true;
    }
  }
  return rxDone;
}

/**
 * @brief  Receives query (data request) from an HC12 data receiver (master). 
 * @return true:  if data query was received,
 *         false: if otherwise.
*/
bool LORA::ReceivedQuery(void)
{
  uint8_t query = 0;
  packetSize = LoRa.parsePacket();
  if(packetSize == 0)
  {
    return false; 
  }
  int recipient = LoRa.read();
  byte sender = LoRa.read();
  query = (uint8_t)LoRa.read();  
  return (query == QUERY);
}

/**
 * @brief Decodes data received from HC12 transmitter.  
 * @param dataID: location of specific data within 'rxBuffer' to be decoded.  
 * @return decoded data (or actual value of desired parameter e.g. voltage).
*/
uint16_t LORA::DecodeData(uint8_t dataID)
{
  dataID %= bufferSize / 2;
  //merge 2 bytes stored in 'rxBuffer' into 16-bit data
  return (rxBuffer[2*dataID] << 8) | rxBuffer[2*dataID + 1]; 
}
