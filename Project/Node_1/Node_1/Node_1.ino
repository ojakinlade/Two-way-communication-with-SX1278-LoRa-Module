#include "lora.h"

#define csPin       10
#define resetPin    9
#define irqPin      2
#define localAddr   0xBB
#define UtilityAddr 0xFF

uint32_t startTime;

LORA Node_1(csPin,resetPin,irqPin,FREQ_433MHZ,localAddr);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  LoRa.begin(433E6);
  Serial.println("Energy Monitor");
  startTime = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Node_1.ReceivedQuery())
  {
    Serial.println("Query Received");
    delay(100);
    Node_1.TransmitData("Hello", UtilityAddr);
    Serial.println("Data Transmitted");
    const uint32_t ackTimeout = 1000; //1000ms
    uint32_t timeout = millis(); 
    while(!Node_1.ReceivedAck())
    {
      if(millis() - timeout >= ackTimeout)
      {
        Serial.println("Data not acknowledged");
      }
    }
    Serial.println("Transmitted data was acknowledged"); 
  }
}
