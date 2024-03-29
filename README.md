# Home IoT Project (Iot Devices)

## Introduction
The IoT devices send room temperature and humidity to Home IoT Server, and receive LED control commands.

## IoT Devices
### NodeMCU(ESP8266)
<p align="center">
<img src="https://github.com/hyp0126/iot-device/blob/main/ESP8266_DHT11_CDS.jpg?raw=true" width="500" />
</p>

### Arduino Uno
<p align="center">
<img src="https://github.com/hyp0126/iot-device/blob/main/UNO_HC-06_THERMISTOR.jpg?raw=true" width="500" />
</p>

## Technologies
* WiFi, Bluetooth, USART<br/>
* MQTT + TLS/SSL

## IoT Gateway
* IoT Gateway Project: https://github.com/hyp0126/iot-gateway-rpi

## Web Server
* Backend(AWS EC2): mosquitto, Express (https://github.com/hyp0126/react-home-iot)<br/>
* Frontend(GCP VM): React, NGINX (https://github.com/hyp0126/react-home-iot)<br/>

## Test (node-RED)
<p align="center">
<img src="https://github.com/hyp0126/iot-device/blob/main/node-RED/node-RED.png?raw=true" width="700" />
</p>

