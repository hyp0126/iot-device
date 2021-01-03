# iot-device
### NodeMCU(ESP8266)
<p align="center">
<img src="https://github.com/hyp0126/iot-device/blob/main/ESP8266_DHT11_CDS.jpg?raw=true" width="500" />
</p>

### Arduino Uno
<p align="center">
<img src="https://github.com/hyp0126/iot-device/blob/main/UNO_HC-06_THERMISTOR.jpg?raw=true" width="500" />
</p>

### Scenario
1. ESP8266 MQTT (Temperature, Humidity, Light intensity, LED) -> rpi (mosquitto, Node-RED) 

2. ESP8266 MQTT (Temperature, Humidity, Light intensity, LED) -> rpi (mosquitto) -> PC (WSL2: Ubuntu: Node-RED) 

3. ESP8266 MQTT (Temperature, Humidity, Light intensity, LED) -> GCP (Ubuntu: mosquitto, Node-RED) 

4. ESP8266 MQTT -> TLS/SSL -> GCP (Compute Engine: Mosquitto)
