# Final-year-project-codes
This is the first iteration of a school project on an intelligent power source selector(IPSS) that selects the most appropraite source to supply a microgrid based on factors such as the cost of electricity, environmental impact and the availability of the source.

The IPSS receives data on the power consumption from various households in the microgrid via RF communication. The RF module used here is the SX1278 LoRa module.

#Precautions taken
1. The SX1278 LoRa module is a 3.3V logic level module so voltage has to be stepped down for compatibility with 5V logic level MCUs.
2. The SX1278 LoRa module requires a power input of 3.3V. The circuit diagram used for the supply input is in the project folder.
