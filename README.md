# iot-device
### NodeMCU(ESP8266)
<p align="center">
<img src="https://github.com/hyp0126/iot-device/blob/main/ESP8266_DHT11_CDS.jpg?raw=true" width="500" />
</p>

1. <b>ESP8266 MQTT (Temperature, Humidity, Light intensity, LED)</b> -> rpi (mosquitto, Node-RED) 

2. <b>ESP8266 MQTT (Temperature, Humidity, Light intensity, LED)</b> -> rpi (mosquitto) -> PC (WSL2: Ubuntu: Node-RED) 

3. <b>ESP8266 MQTT (Temperature, Humidity, Light intensity, LED)</b> -> GCP (Ubuntu: mosquitto, Node-RED) 

4. <b>ESP8266 MQTT</b> -> TLS/SSL -> GCP (Compute Engine: Mosquitto)

### Arduino Uno
<p align="center">
<img src="https://github.com/hyp0126/iot-device/blob/main/UNO_HC-06_THERMISTOR.jpg?raw=true" width="500" />
</p>

1. <b>ARDUINO UNO + Temperature sensor + HC-06(Bluetooth)</b> -> rpi (BT to MQTT) -> rpi (Mosquitto, Node-RED)

2. <b>ARDUINO UNO + Temperature sensor + HC-06(Bluetooth)</b> -> rpi (BT to MQTT) -> GCP (Ubuntu: Mosquitto, Node-RED)

3. <b>UNO BT</b> -> rpi (BT to MQTT) -> (TLS/SSL) -> GCP (Compute Engine: Mosquitto) -> GCP (Node-RED / https)

### node-RED
<p align="center">
<img src="https://github.com/hyp0126/iot-device/blob/main/node-RED/node-RED.png?raw=true" width="700" />
</p>
