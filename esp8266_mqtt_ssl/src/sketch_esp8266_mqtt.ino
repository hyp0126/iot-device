/*
  Rui Santos
  Complete project details at https://RandomNerdTutorials.com/esp8266-nodemcu-mqtt-publish-dht11-dht22-arduino/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*/
// Originally from https://github.com/marvinroger/async-mqtt-client
/* 
 *  Yi Phyo Hong
 *  modified 2020-12-24: Add LED for checking live
 *                       Add Light Intensity Sensor 
 *  modified 2020-12-25: Add LED subscribe topic
 *  modified 2020-12-28: Use credentials
 *  modified 2020-12-30: For Using SSL, Arduino IDE -> PlatformIO IDE
 */

#include <Arduino.h>
#include "Adafruit_Sensor.h"
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <Ticker.h>
#include <AsyncMqttClient.h>
#include "HomeConfig.h"

#define WIFI_SSID HOME_SSID
#define WIFI_PASSWORD HOME_PASSWORD

// Raspberri Pi Mosquitto MQTT Broker
//#define MQTT_HOST IPAddress(192, 168, 0, 26)
// For a cloud MQTT broker, type the domain name
#define MQTT_HOST HOME_HOST

#if ASYNC_TCP_SSL_ENABLED
#define MQTT_SECURE true
#define MQTT_PORT 8883
#else
#define MQTT_PORT 1883
#endif

#define ROOM_NUMBER 1
// Temperature, Humidity, Light Intensity MQTT Topics
#define MQTT_PUB_TEMP "home/room%d/temperature"
#define MQTT_PUB_HUM "home/room%d/humidity"
#define MQTT_PUB_LIGHT "home/room%d/brightness"

// LED MQTT Topics
#define MQTT_PUB_LED "home/room%d/ledState"
#define MQTT_SUB_LED "home/room%d/led"

// Digital pin connected to the DHT sensor
#define DHTPIN 14  

// Digital pin: blinking LED pin (GPIO 2)
#define LEDPIN 2
#define LED_OFF 0
#define LED_ON  1

// Analog pin: ADC for CDS Sensor
#define CDSPIN  A0

// Uncomment whatever DHT sensor type you're using
#define DHTTYPE DHT11   // DHT 11
//#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)   

// Number of sensors
#define MAX_SENSOR_ORDER 4

// Initialize DHT sensor
DHT dht(DHTPIN, DHTTYPE);

// Variables to hold sensor readings
float temp;
float hum;
float bright;
bool isMqttConnected = false;

AsyncMqttClient mqttClient;
Ticker mqttReconnectTimer;

WiFiEventHandler wifiConnectHandler;
WiFiEventHandler wifiDisconnectHandler;
Ticker wifiReconnectTimer;

unsigned long previousMillis = 0;   // Stores last time temperature was published
const long interval = 2500;        // Interval at which to publish sensor readings

unsigned long previousMillisForLed = 0;   // Stores last time LED turn on/off
const long intervalForLed = 1000;        // Interval at which to turn on/off LED
unsigned char currentLedStatus = LED_OFF;
bool ledOnFlag = true;
// When using SSL, much data make some problem, so, need interval between sending data
unsigned char sensorOrder = 0;  

void connectToWifi() {
  Serial.println("Connecting to Wi-Fi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void onWifiConnect(const WiFiEventStationModeGotIP& event) {
  Serial.println("Connected to Wi-Fi.");
  connectToMqtt();
}

void onWifiDisconnect(const WiFiEventStationModeDisconnected& event) {
  Serial.println("Disconnected from Wi-Fi.");
  mqttReconnectTimer.detach(); // ensure we don't reconnect to MQTT while reconnecting to Wi-Fi
  wifiReconnectTimer.once(2, connectToWifi);
}

void connectToMqtt() {
  Serial.println("Connecting to MQTT...");
  mqttClient.connect();
}

void onMqttConnect(bool sessionPresent) {
  char topic[100];

  Serial.println("Connected to MQTT.");
  Serial.print("Session present: ");
  Serial.println(sessionPresent);

  // Subscribe LED command
  sprintf(topic, MQTT_SUB_LED, ROOM_NUMBER);
  uint16_t packetIdSub1 = mqttClient.subscribe(topic, 1);                            
  Serial.printf("Subscribing on topic %s at QoS 1, packetId %i: ", topic, packetIdSub1);

  isMqttConnected = true;
}

void onMqttDisconnect(AsyncMqttClientDisconnectReason reason) {
  isMqttConnected = false;

  Serial.println("Disconnected from MQTT.");

  if (WiFi.isConnected()) {
    mqttReconnectTimer.once(2, connectToMqtt);
  }
}

void onMqttSubscribe(uint16_t packetId, uint8_t qos) {
  Serial.println("Subscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
  Serial.print("  qos: ");
  Serial.println(qos);
}

void onMqttUnsubscribe(uint16_t packetId) {
  Serial.println("Unsubscribe acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void onMqttMessage(char* topic, char* payload, AsyncMqttClientMessageProperties properties, size_t len, size_t index, size_t total) {
  Serial.println("Publish received.");
  Serial.print("  topic: ");
  Serial.println(topic);
  Serial.print("  qos: ");
  Serial.println(properties.qos);
  Serial.print("  dup: ");
  Serial.println(properties.dup);
  Serial.print("  retain: ");
  Serial.println(properties.retain);
  Serial.print("  len: ");
  Serial.println(len);
  Serial.print("  index: ");
  Serial.println(index);
  Serial.print("  total: ");
  Serial.println(total);
  Serial.print("  payload: ");
  Serial.println(payload[0]);

  // Set led on/off flag
  if (payload[0] == '1')
  {
    ledOnFlag = true;
  }
  else
  {
    ledOnFlag = false;    
  }
}

void onMqttPublish(uint16_t packetId) {
  Serial.print("Publish acknowledged.");
  Serial.print("  packetId: ");
  Serial.println(packetId);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // LED pin: output
  pinMode(LEDPIN, OUTPUT); 
  
  dht.begin();
  
  wifiConnectHandler = WiFi.onStationModeGotIP(onWifiConnect);
  wifiDisconnectHandler = WiFi.onStationModeDisconnected(onWifiDisconnect);

  mqttClient.onConnect(onMqttConnect);
  mqttClient.onDisconnect(onMqttDisconnect);
  mqttClient.onSubscribe(onMqttSubscribe);
  mqttClient.onUnsubscribe(onMqttUnsubscribe);
  mqttClient.onMessage(onMqttMessage);
  mqttClient.onPublish(onMqttPublish);
  mqttClient.setServer(MQTT_HOST, MQTT_PORT);
  // If your broker requires authentication (username and password), set them below
  mqttClient.setCredentials(HOME_USER_ID, HOME_USER_PASSWORD);

#if ASYNC_TCP_SSL_ENABLED
  mqttClient.setSecure(MQTT_SECURE);
  if (MQTT_SECURE) {
    mqttClient.addServerFingerprint((const uint8_t[])MQTT_SERVER_FINGERPRINT);
  }
#endif

  connectToWifi();
}

void loop() {
  uint16_t packetIdPub;
  char topic[100];
  unsigned long currentMillis = millis();
  // Every X number of seconds (interval = 10 seconds) 
  // it publishes a new MQTT message
  if (currentMillis - previousMillis >= interval) {
    // Save the last time a new reading was published
    previousMillis = currentMillis;

    if (isMqttConnected)
    {
      switch (sensorOrder)
      {
        case 0:
          // New DHT sensor readings
          hum = dht.readHumidity();
          // Publish an MQTT message on topic humidity
          sprintf(topic, MQTT_PUB_HUM, ROOM_NUMBER);
          packetIdPub = mqttClient.publish(topic, 1, true, String(hum).c_str());                            
          Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", topic, packetIdPub);
          Serial.printf("Message: %.2f \n", hum);
          break;

        case 1:
          // Read temperature as Celsius (the default)
          temp = dht.readTemperature();
          // Read temperature as Fahrenheit (isFahrenheit = true)
          //temp = dht.readTemperature(true);
          // Publish an MQTT message on topic temperature
          sprintf(topic, MQTT_PUB_TEMP, ROOM_NUMBER);
          packetIdPub = mqttClient.publish(topic, 1, true, String(temp).c_str());                            
          Serial.printf("Publishing on topic %s at QoS 1, packetId: %i ", topic, packetIdPub);
          Serial.printf("Message: %.2f \n", temp);
          break;

        case 2:
          // Read CDS sensor
          bright = analogRead(CDSPIN);
          // Publish an MQTT message on topic brightness
          sprintf(topic, MQTT_PUB_LIGHT, ROOM_NUMBER);
          packetIdPub = mqttClient.publish(topic, 1, true, String(bright).c_str());                            
          Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", topic, packetIdPub);
          Serial.printf("Message: %.2f \n", bright);  
          break;

        case 3:
          // Publish an MQTT message on topic led
          char ledOnFlagStr[2];
          if (ledOnFlag == true)
          {
            strcpy(ledOnFlagStr, "1");
          }
          else
          {
            strcpy(ledOnFlagStr, "0");
          }
          sprintf(topic, MQTT_PUB_LED, ROOM_NUMBER);
          packetIdPub = mqttClient.publish(topic, 1, true, ledOnFlagStr);                            
          Serial.printf("Publishing on topic %s at QoS 1, packetId %i: ", topic, packetIdPub);
          Serial.printf("Message: %s \n", ledOnFlagStr);
      }
      sensorOrder++;
      if (sensorOrder >= MAX_SENSOR_ORDER)
      {
        sensorOrder = 0;
      }
    }
  }

  if (currentMillis - previousMillisForLed >= intervalForLed) {
    previousMillisForLed = currentMillis;
    if (currentLedStatus == LED_OFF)
    {
      if (ledOnFlag == true)
      {
        digitalWrite(LEDPIN, LOW); // Turn the LED on (Note that LOW is the voltage level) 
      }
      currentLedStatus = LED_ON;
    }
    else
    {
      digitalWrite(LEDPIN, HIGH); // Turn the LED off by making the voltage HIGH    
      currentLedStatus = LED_OFF;    
    }
  }
}
