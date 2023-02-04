#include "lora.h"

#define csPin       5
#define resetPin    34
#define irqPin      2
#define localAddr   0xFF
#define Node1Addr   0xBB

uint32_t startTime;

LORA Utility(csPin,resetPin,irqPin,FREQ_433MHZ,localAddr);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.println("Energy monitor");
  startTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(millis() - startTime >= 5000)
  {
    Utility.TransmitQuery(Node1Addr);
    startTime = millis();
    Serial.println("Query sent");
  }
  if(Utility.ReceivedData())
  {
    Serial.println("Data Received");
    delay(100);
    Utility.TransmitAck(Node1Addr);
    Serial.println(Utility.DecodeData(VOLTAGE));
    Serial.println(Utility.DecodeData(CURRENT));
    Serial.println(Utility.DecodeData(PFACTOR));
    Serial.println(Utility.DecodeData(FREQ));
    Serial.println(Utility.DecodeData(POWER));
    Serial.println(Utility.DecodeData(ENERGY));
  }
}
