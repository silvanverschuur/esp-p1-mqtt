// Required libraries:
// - PubSubClient
// - WifiManager

// GLOBAL
const char *SKETCH_NAME = "P1 Meter";
const char *VERSION = "v0.3.8";

// WIFI
const char *DEFAULT_HOSTNAME = "P1";
const char *DEFAULT_AP_SSID = "P1_ESP8266";
const char *DEFAULT_AP_PASSWORD = "my-password";

// Debugging, outputs telegram to serial port
const bool OUTPUT_TELEGRAM_ON_SERIAL = false;

// MQTT
const char *MQTT_SERVER = "192.168.1.2";
const char *MQTT_CLIENT_ID = "mqqq-client";
const char *MQTT_USER = "mqtt-user";
const char *MQTT_PASSWORD = "mqtt-password";
const int MQTT_RECONNECT_DELAY = 5000; // Try to reconnect every 5 seconds
const int MQTT_PUBLISH_INTERVAL = 30000; // Publish every X milliseconds

// MQTT topics
const char *MQTT_TOPIC_STATUS = "p1/status";
const char *MQTT_TOPIC_ELECTRICITY_CONSUMPTION_HIGH = "p1/electricity/consumption/high";
const char *MQTT_TOPIC_ELECTRICITY_CONSUMPTION_LOW = "p1/electricity/consumption/low";
const char *MQTT_TOPIC_ELECTRICITY_CONSUMPTION_CURRENT = "p1/electricity/consumption/current";
const char *MQTT_TOPIC_ELECTRICITY_PRODUCTION_HIGH = "p1/electricity/production/high";
const char *MQTT_TOPIC_ELECTRICITY_PRODUCTION_LOW = "p1/electricity/production/low";
const char *MQTT_TOPIC_ELECTRICITY_PRODUCTION_CURRENT = "p1/electricity/production/current";
const char *MQTT_TOPIC_ELECTRICITY_VOLTAGE = "p1/electricity/voltage";
const char *MQTT_TOPIC_ELECTRICITY_TARIFF = "p1/electricity/tariff";
const char *MQTT_TOPIC_GAS_CONSUMPTION = "p1/gas/consumption";
