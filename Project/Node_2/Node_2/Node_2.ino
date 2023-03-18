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

uint32_t nodeTime = millis();
//Objects
SoftwareSerial pzemSerial(pzemTx, pzemRx);
PZEM004Tv30 pzem(pzemSerial);
MNI Node_2(Pin::csPin,Pin::resetPin,Pin::irqPin,FREQ_433MHZ,MNI::localAddr);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  LoRa.begin(433E6);
  Serial.println("Energy Monitor");
  nodeTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Node_2.ReceivedData())
  {
    if(Node_2.DecodeData(MNI::RxDataId::NODE_ADDR) == MNI::localAddr &&
       Node_2.DecodeData(MNI::RxDataId::DATA_QUERY) == MNI::QUERY)
    {
      Serial.println("Query Received");
//      uint16_t pwr = pzem.power() * 10;      
      uint16_t pwr = 100;   
      //Debug  
      Serial.print("power = ");
      Serial.println(pwr);   

      Node_2.EncodeData(MNI::ACK,MNI::TxDataId::DATA_ACK);
      Node_2.EncodeData(MNI::localAddr,MNI::TxDataId::TX_NODE_ADDR);
      Node_2.EncodeData(pwr,MNI::TxDataId::POWER);
      Node_2.TransmitData();
      Serial.println("Data Transmitted");
    } 
  }
}
