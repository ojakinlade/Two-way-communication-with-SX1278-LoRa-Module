#include <LiquidCrystal_I2C.h>
#include "MNI.h"

namespace Pin
{
  const uint8_t csPin = 5;
  const uint8_t resetPin = 34;
  const uint8_t irqPin = 2;
};

typedef  struct
{
  uint16_t node1Pwr;
  uint16_t node2Pwr;
}pwr_t;

//RTOS Handle(s)
TaskHandle_t appTaskHandle;
TaskHandle_t nodeTaskHandle;
QueueHandle_t nodeToAppQueue;
//QueueHandle_t nodeToMqttQueue;

void setup() {
  // put your setup code here, to run once:
  setCpuFrequencyMhz(80);
  Serial.begin(115200);
  Serial.println("Energy monitor");
  nodeToAppQueue = xQueueCreate(1,sizeof(pwr_t));
  if(nodeToAppQueue != NULL)
  {
    Serial.println("Node-Application Queue successfully creeated");
  }
  else
  {
    Serial.println("Node-Application Queue creation failed");
  }
  //Tasks
  xTaskCreatePinnedToCore(NodeTask,"",30000,NULL,1,&nodeTaskHandle,1);
//  xTaskCreatePinnedToCore(ApplicationTask,"",30000,NULL,1,&appTaskHandle,1);  
}

void loop() {
  // put your main code here, to run repeatedly:

}

void ApplicationTask(void* pvParameters)
{
  LiquidCrystal_I2C lcd(0x27,20,4);
  static pwr_t pwr;

  lcd.init();
  lcd.backlight();
  lcd.print(" ENERGY MONITOR");
  vTaskDelay(pdMS_TO_TICKS(1500));
  lcd.clear();
  lcd.print("STATUS: ");
  lcd.setCursor(0,1);
  lcd.print("LOADING...");
  vTaskDelay(pdMS_TO_TICKS(1500));
  lcd.clear();

  while(1)
  {
    
  }
}

/**
 * @brief Handles communication between the master and the node via a 
 * serial interface (i.e. MNI).
 * MNI is an acronym for 'Master-Node-Interface'.
*/
void NodeTask(void* pvParameters)
{
  MNI Utility(Pin::csPin,Pin::resetPin,Pin::irqPin,FREQ_433MHZ,MNI::localAddr);
  static pwr_t pwr; 
  uint32_t node1Time = millis();
  uint32_t node2Time = millis();

  while(1)
  {
    if(millis() - node1Time >= 3000)
    {
      Utility.EncodeData(MNI::node1Addr,MNI::TxDataId::NODE_ADDR);
      Utility.EncodeData(MNI::QUERY,MNI::TxDataId::DATA_QUERY);
      Utility.TransmitData();
      node1Time = millis();
      Serial.println("--Query sent to Node 1");
    }
    if(millis() - node2Time >= 3000)
    {
      node1Time = millis();
      Utility.EncodeData(MNI::node2Addr,MNI::TxDataId::NODE_ADDR);
      Utility.EncodeData(MNI::QUERY,MNI::TxDataId::DATA_QUERY);
      Utility.TransmitData();
      node1Time = millis();
      node2Time = millis();
      Serial.println("--Query sent to Node 2");
    }
    
    if(Utility.ReceivedData())
    {
      if(Utility.DecodeData(MNI::RxDataId::DATA_ACK) == MNI::ACK)
      {
        if(Utility.DecodeData(MNI::RxDataId::RX_NODE_ADDR) == MNI::node1Addr)
        {
          Serial.println("Data Received from node 1");
          vTaskDelay(pdMS_TO_TICKS(200));
          pwr.node1Pwr = Utility.DecodeData(MNI::RxDataId::POWER);
          Serial.println(pwr.node1Pwr);
        }
        else if(Utility.DecodeData(MNI::RxDataId::RX_NODE_ADDR) == MNI::node2Addr)
        {
          Serial.println("Data Received from node 2");
          vTaskDelay(pdMS_TO_TICKS(200));
          pwr.node2Pwr = Utility.DecodeData(MNI::RxDataId::POWER);
          Serial.println(pwr.node2Pwr);
        }
      }   
    }
  }
}
