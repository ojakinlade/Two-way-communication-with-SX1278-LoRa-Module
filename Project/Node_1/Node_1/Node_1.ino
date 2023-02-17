#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
#include "MNI.h"

namespace Pin
{
  const uint8_t csPin = 10;
  const uint8_t resetPin = 9;
  const uint8_t irqPin = 2;
};

//SoftwareSerial Pins
const int pzemTx = 2;
const int pzemRx = 3;

//Objects
SoftwareSerial pzemSerial(pzemTx, pzemRx);
PZEM004Tv30 pzem(pzemSerial);
MNI Node_1(Pin::csPin,Pin::resetPin,Pin::irqPin,FREQ_433MHZ,MNI::localAddr);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  LoRa.begin(433E6);
  Serial.println("Energy Monitor");
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Node_1.ReceivedData())
  {
    if(Node_1.DecodeData(MNI::RxDataId::NODE_ADDR) == MNI::localAddr &&
       Node_1.DecodeData(MNI::RxDataId::DATA_QUERY) == MNI::QUERY)
    {
      Serial.println("Query Received");
      //uint16_t pwr = pzem.power() * 100;
      uint16_t pwr = 50;   
      //Debug  
      Serial.print("power = ");
      Serial.println(pwr);   

      Node_1.EncodeData(MNI::ACK,MNI::TxDataId::DATA_ACK);
      Node_1.EncodeData(MNI::localAddr,MNI::TxDataId::TX_NODE_ADDR);
      Node_1.EncodeData(pwr,MNI::TxDataId::POWER);
      Node_1.TransmitData();
      Serial.println("Data Transmitted");
    } 
  }
}
