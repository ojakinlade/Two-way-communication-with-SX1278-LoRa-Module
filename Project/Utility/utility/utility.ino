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
  xTaskCreatePinnedToCore(ApplicationTask,"",25000,NULL,1,&appTaskHandle,1);  
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
  lcd.print("INITIALIZING...");
  vTaskDelay(pdMS_TO_TICKS(1500));
  lcd.clear();
  vTaskResume(nodeTaskHandle);

  while(1)
  {
    //Receive sensor Data from Node-Application Queue.
    if(xQueueReceive(nodeToAppQueue,&pwr,0) == pdPASS)
    {
      Serial.println("--Application task received data from Node task\n");
    }
    uint16_t totalPwr = pwr.node1Pwr + pwr.node2Pwr;
    lcd.setCursor(0,0);
    lcd.print("Node 1: ");
    lcd.print(pwr.node1Pwr);
    lcd.print(" W");
    lcd.setCursor(0,1);
    lcd.print("Node 2: ");
    lcd.print(pwr.node2Pwr);
    lcd.print(" W");
    lcd.setCursor(0,2);
    lcd.print("Total PWR: ");
    lcd.print(totalPwr);
    lcd.print(" W");
    lcd.setCursor(0,3);
    lcd.print("Source used: ");
    lcd.print("xxxx");
  }
}

/**
 * @brief Handles communication between the master and the node via a 
 * serial interface (i.e. MNI).
 * MNI is an acronym for 'Master-Node-Interface'.
*/
void NodeTask(void* pvParameters)
{
  vTaskSuspend(NULL);
  MNI Utility(Pin::csPin,Pin::resetPin,Pin::irqPin,FREQ_433MHZ,MNI::localAddr);
  static pwr_t pwr; 
  uint8_t node = MNI::node1Addr;
  uint32_t nodeTime = millis();
  bool utilityDoneReceiving = false;

  while(1)
  {
    if(millis() - nodeTime >= 1000)
    {
      Utility.EncodeData(node,MNI::TxDataId::NODE_ADDR);
      Utility.EncodeData(MNI::QUERY,MNI::TxDataId::DATA_QUERY);
      Utility.TransmitData();
      Serial.print("--Query sent to ");
      Serial.println(node);
      node = (node == MNI::node1Addr) ? MNI::node2Addr : MNI::node1Addr;
      nodeTime = millis();
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
          utilityDoneReceiving = true;
          Serial.println(pwr.node2Pwr);
        }
      }   
    }
    if(utilityDoneReceiving)
    {
      utilityDoneReceiving = false;
      if(xQueueSend(nodeToAppQueue,&pwr,0) == pdPASS)
      {
        Serial.println("--Data Sucessfully sent to Application Task\n");
      }
      else
      {
        Serial.println("--Failed to send to Application Task\n");
      }
    }
  }
}
