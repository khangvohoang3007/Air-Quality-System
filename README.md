# THE AIR QUALITY MONITORING SYSTEM USING ESP8266 AND BLYNK SERVER MONITORING

## OVERVIEW
Hello! This is a ***hands-on guide*** to help you build your own ***Air Quality Monitoring System using ESP8266 and Blynk Server***.

The system features 2 sensor nodes for data collection and transmission to the Blynk Server.
<p align="center">
  <img src="hardware/system_blockDiagram.png" width="500" alt="System Block Diagram">
  <br>
</p>

Each node is designed with the following core modules:
* **Processing & Communication:** ESP8266 NodeMCU (Wi-Fi enabled).
* **Sensing Unit:** MQ-135 (Gas/Air Quality) and DHT11 (Temperature & Humidity).
* **Display Unit:** 0.96" OLED Display for local real-time monitoring.
<p align="center">
  <img src="hardware/sensorNode_structure.png" width="500" alt="Sensor Node Structure">
  <br>
</p>

