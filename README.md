# Two-way communication with SX1278 LoRa Module
This project demonstrates how send and receive data between two or more SX1278 LoRa modules. In this project, there is a master and 2 nodes. The master periodically requests data from the two nodes and displays the received data on an LCD. 
The master uses an ESP32 microntroller while the nodes use an Arduino microcontroller.

# Precautions taken
1. The SX1278 LoRa module is a 3.3V logic level module so voltage has to be stepped down for compatibility with an Arduino Nano which has a 5V logic level.
2. The SX1278 LoRa module requires a power input of 3.3V. The circuit diagram used for the supply input is in the project folder.
