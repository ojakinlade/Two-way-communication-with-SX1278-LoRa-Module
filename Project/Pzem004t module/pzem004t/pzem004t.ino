#include <PZEM004Tv30.h>
#include <SoftwareSerial.h>

const int pzemTx = 2;
const int pzemRx = 3;

SoftwareSerial pzemSerial(pzemTx, pzemRx);
PZEM004Tv30 pzem(pzemSerial);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Read the data from the sensor
  pzemSerial.listen();
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float energy = pzem.energy();
  float frequency = pzem.frequency();
  float pf = pzem.pf();

  // Print the values to the Serial console
  Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
  Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
  Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
  Serial.print("Energy: ");       Serial.print(energy,3);     Serial.println("kWh");
  Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
  Serial.print("PF: ");           Serial.println(pf);

  Serial.println();
  delay(2000);  
}
