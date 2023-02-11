#include <SPI.h>
#include <LoRa.h>
#include "MNI.h"

/**
 * @brief Initialize LoRa transceiver.  
*/
MNI::MNI(uint8_t csPin, uint8_t rstPin, uint8_t irqPin, uint32_t freq, uint8_t deviceAddr)
{
  //Initialize private variables
  chipSel = csPin;
  resetPin = rstPin;
  IRQPin = irqPin;
  addr = deviceAddr;
  packetSize = 0;
  LoRa.setPins(chipSel, resetPin, IRQPin);// set CS, reset, IRQ pin
  //LoRa.begin(freq);
  rxDataCounter = 0;
  for(uint8_t i = 0; i < BufferSize::TX; i++)
  {
    txBuffer[i] = 0;
  }
  for(uint8_t j = 0; j < BufferSize::RX; j++)
  {
    rxBuffer[j] = 0;
  }
}

/**
 * @brief Encodes data (i.e. power measurements) into the 'txBuffer' before 
 * transmission to a LoRa receiver can occur.  
 * @param data:   data to be encoded (e.g. voltage, current).  
 * @param dataID: location of specific data within 'txBuffer' to be encoded.
*/
void MNI::EncodeData(uint16_t dataToEncode,TxDataId id)
{
  uint8_t dataID = (uint8_t)id;
  //split 16-bit data into 2 bytes and store in the 'txBuffer'
  txBuffer[dataID] = (dataToEncode & 0xFF00) >> 8; //high byte
  txBuffer[dataID + 1] = dataToEncode & 0xFF; //low byte
}

/**
 * @brief Sends data to a LoRa receiver (i.e. master device).  
 * @param pData: Pointer to data to be sent to LoRa receiver
 * @param deviceAddr: Address of LoRa receiver
*/
void MNI::TransmitData(void)
{
  Serial.print("Tx buffer: ");
  for(uint8_t i = 0; i < BufferSize::TX; i++)
  {
    LoRa.beginPacket();
    LoRa.write(txBuffer[i]);
    LoRa.endPacket(); 
    Serial.print(txBuffer[i]);
    Serial.print(' ');
    delay(150);
  }
  Serial.print("\n");
}

/**
 * @brief  Receives data from a LoRa transmitter. 
 * @return true:  if data reception is complete,
 *         false: if otherwise.
*/
bool MNI::ReceivedData(void)
{
  bool rxDone = false;
  packetSize = LoRa.parsePacket(); 
  if(packetSize != 0)
  {
    if(LoRa.available())
    {
      if(rxDataCounter < BufferSize::RX)
      {
        rxBuffer[rxDataCounter] = LoRa.read();
        Serial.print(rxBuffer[rxDataCounter]);
        Serial.print(" ");
        rxDataCounter++;
      }
    }
    if(rxDataCounter == BufferSize::RX)
    {
      Serial.print("\n");
      rxDataCounter = 0;
      rxDone = true;
    }
  }
  return rxDone;
}

/**
 * @brief Decodes data received from LoRa transmitter.  
 * @param dataID: location of specific data within 'rxBuffer' to be decoded.  
 * @return decoded data (or actual value of desired parameter e.g. voltage).
*/
uint16_t MNI::DecodeData(RxDataId id)
{
  uint8_t dataID = (uint8_t)id;
  //merge 2 bytes stored in 'rxBuffer' into 16-bit data
  return (rxBuffer[dataID] << 8) | rxBuffer[dataID + 1]; 
}

void MNI::ClearRxBuffer(void)
{
  for(uint8_t j = 0; j < BufferSize::RX; j++)
  {
    rxBuffer[j] = 0;
  }
}
