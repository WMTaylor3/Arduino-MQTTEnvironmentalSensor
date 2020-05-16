# ArduinoHomeAssistantEnvironmentalMonitor
DHT22/ESP8266 temp and humidity sensor reporting via MQTT

## Requires an additional `credentials.h` file to store the following configuration values:
```
#define WIFI_NAME "WifiNameHere"
#define WIFI_PASSWORD "WifiPasswordHere"

#define MQTT_SERVER "xxx.xxx.xxx.xxx"
#define MQTT_PORT xxxx
#define MQTT_USER "deviceNameHere"
#define MQTT_PASSWORD "devicePasswordHere"
#define MQTT_ID "deviceIdHere"
```
