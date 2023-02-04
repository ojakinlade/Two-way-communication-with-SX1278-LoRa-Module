#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>
#include "lora.h"

#define csPin       10
#define resetPin    9
#define irqPin      2
#define localAddr   0xBB
#define UtilityAddr 0xFF

uint32_t startTime;

//SoftwareSerial Pins
const int pzemTx = 2;
const int pzemRx = 3;

//Objects
SoftwareSerial pzemSerial(pzemTx, pzemRx);
PZEM004Tv30 pzem(pzemSerial);
LORA Node_1(csPin,resetPin,irqPin,FREQ_433MHZ,localAddr);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
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
    pzemSerial.listen();
    uint16_t volt = pzem.voltage() * 10;
    uint16_t freq = pzem.frequency() * 10;
    uint16_t pf = pzem.pf() * 100;
    uint16_t curr = pzem.current() * 100;
    uint16_t pwr = pzem.power() * 10;
    uint16_t kwh = pzem.energy() * 100;  
    Serial.print("voltage = ");
    Serial.println(volt);
    Serial.print("frequency = ");
    Serial.println(freq);  
    Serial.print("power factor = ");
    Serial.println(pf);
    Serial.print("current = ");
    Serial.println(curr); 
    Serial.print("power = ");
    Serial.println(pwr);
    Serial.print("energy = ");
    Serial.println(kwh);     

    Node_1.EncodeData(volt,VOLTAGE);
    Node_1.EncodeData(curr,CURRENT);
    Node_1.EncodeData(pf,PFACTOR);
    Node_1.EncodeData(freq,FREQ);
    Node_1.EncodeData(pwr,POWER);
    Node_1.EncodeData(kwh,ENERGY);
    Node_1.TransmitData(UtilityAddr);
    Serial.println("Data Transmitted");
    const uint32_t ackTimeout = 2000; //1000ms
    uint32_t timeout = millis(); 
    while(!Node_1.ReceivedAck())
    {
      if(millis() - timeout >= ackTimeout)
      {
        Serial.println("Data not acknowledged");
        break;
      }
    }
    Serial.println("Transmitted data was acknowledged"); 
  }
}
