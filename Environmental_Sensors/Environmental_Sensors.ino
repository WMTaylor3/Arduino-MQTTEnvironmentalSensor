#include <DHT.h>
#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include "credentials.h"

#define REBOOT_INTERVAL 3600100 // 100 milliseconds over every hour (Not to interfere with reading every minute).

#define DHT_PIN 2
#define DHT_MODEL DHT22
#define DHT_MEASUREMENT_INTERVAL 60000 // 60 seconds
unsigned long lastMeasurementTime = 0;

#define MQTT_TEMPERATURE_TOPIC MQTT_TOPIC "/temperature"
#define MQTT_HUMIDITY_TOPIC MQTT_TOPIC "/humidity"

DHT dht(DHT_PIN, DHT_MODEL);
WiFiClient wifiClient;
PubSubClient mqtt;

void setup() {
  Serial.begin(9600);
  
  dht.begin();

  setupWifi();
  
  mqtt.setClient(wifiClient);
  mqtt.setServer(MQTT_SERVER, MQTT_PORT);
  mqtt.setCallback(callback);
}

void setupWifi() {
  Serial.print(F("Connecting to WiFi"));
  Serial.println(WIFI_NAME);

  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    WiFi.disconnect();
    for(uint8_t i = 0; i < 30; i++) {
      if(WiFi.waitForConnectResult() == WL_CONNECTED) {
        break;
      }
      WiFi.mode(WIFI_STA);
      WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
      Serial.print("Wifi Status: ");
      Serial.println(WiFi.waitForConnectResult());
      delay(500); 
    }
  }

  Serial.println(F(""));
  Serial.println(F("WiFi connected"));
  Serial.println(F("IP address: "));
  Serial.println(WiFi.localIP());
}

void reconnect() {
  while (!mqtt.connected()) {
    Serial.print(F("Attempting MQTT connection..."));
    if (mqtt.connect(MQTT_ID, MQTT_USER, MQTT_PASSWORD)) {
      Serial.println(F("connected"));
    } else {
      Serial.print(F("Failed, rc="));
      Serial.print(mqtt.state());
      Serial.println(F(" try again in 5 seconds"));
      delay(5000);
    }
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print(topic);
  Serial.print(F("] "));
  for (int i = 0; i < length; i++) {
    char receivedChar = (char)payload[i];
    Serial.print(receivedChar);
  }
  Serial.println();
}

void loop() {
//  if(millis() > REBOOT_INTERVAL) {
//    ESP.restart();
//  }
  if(WiFi.waitForConnectResult() != WL_CONNECTED) {
    setupWifi();
  }
    
  if (!mqtt.connected()) {
    reconnect();
  }

  mqtt.loop();

  if(millis() - lastMeasurementTime > DHT_MEASUREMENT_INTERVAL) {
//      Serial.print(F("Heap: "));
//      Serial.println(ESP.getFreeHeap());
//      Serial.print(F("Stack: "));
//      Serial.println(ESP.getFreeContStack());
//      Serial.print(F("Sketch Space: "));
//      Serial.println(ESP.getFreeSketchSpace());
  
    float humidity = dht.readHumidity();
    float temperature = dht.readTemperature();

    if (!isnan(humidity) && !isnan(temperature)) {
      Serial.print(F("Humidity: "));
      Serial.print(humidity);
      Serial.print(F("%  Temperature: "));
      Serial.print(temperature);
      Serial.println(F("°C "));

      bool successTemperature = mqtt.publish(MQTT_TEMPERATURE_TOPIC, String(temperature, 1).c_str());
      bool successHumidity = mqtt.publish(MQTT_HUMIDITY_TOPIC, String(humidity, 1).c_str());

      if(successTemperature){
        Serial.print(F("Successfully posted to MQTT topics: "));
        Serial.println(MQTT_TEMPERATURE_TOPIC);
      }

      if(successHumidity){
        Serial.print(F("Successfully posted to MQTT topics: "));
        Serial.println(MQTT_HUMIDITY_TOPIC);
      }

      lastMeasurementTime = millis();

    } else {
      Serial.println(F("Failed to read from DHT sensor!"));
    }
  }
}
