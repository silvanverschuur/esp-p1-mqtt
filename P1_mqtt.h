// MQTT
long lastMqttConnectionAttempt = 0;

// Variables to store meter readings
float electricityConsumptionLow = -1;
float electricityConsumptionHigh = -1;
float electricityConsumptionCurrent = -1;
float electricityProductionCurrent = -1;
float electricityProductionLow = -1;
float electricityProductionHigh = -1;
float electricityVoltage = -1;
float gasConsumption = -1;
int electricityTariff = -1;

const int VALUE_LENGTH = 16;

WiFiClient espClient;
PubSubClient mqttClient(espClient);

boolean connectMqtt() {
  if (mqttClient.connected()) {
    return true;
  }
  
  long now = millis();
  if (now - lastMqttConnectionAttempt < MQTT_RECONNECT_DELAY) {
    return false;
  }

  lastMqttConnectionAttempt = now;

  if (mqttClient.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASSWORD)) {
    Serial.println("Connected\n");
    mqttClient.publish(MQTT_TOPIC_STATUS, "P1 connected to MQTT");
  } else {
    Serial.print("Cannot connect to MQTT server, result ");
    Serial.print(mqttClient.state());
    Serial.println();
    delay(50);
  }
  
  return mqttClient.connected();
}

void configureMqttClient() {
  mqttClient.setServer(MQTT_SERVER, 1883);
}

char* ftoa(char *output, float value, int length, int precision) {
  dtostrf(value, length, precision, output);
  String tmp(output);
  tmp.trim();
  tmp.toCharArray(output, VALUE_LENGTH);
  return output;
}

boolean publishMqtt(const char *topic, float value, int precision) {
  char buffer[VALUE_LENGTH];
  return mqttClient.publish(topic, ftoa(buffer, value, VALUE_LENGTH, precision), true);
}

boolean publishMqtt(const char *topic, int value) {
  char buffer[VALUE_LENGTH];
  return mqttClient.publish(topic, itoa(value, buffer, VALUE_LENGTH), true);
}

void publishDataToMqtt() {
  publishMqtt(MQTT_TOPIC_ELECTRICITY_CONSUMPTION_LOW, electricityConsumptionLow, 3);
  publishMqtt(MQTT_TOPIC_ELECTRICITY_CONSUMPTION_HIGH, electricityConsumptionHigh, 3);
  publishMqtt(MQTT_TOPIC_ELECTRICITY_CONSUMPTION_CURRENT, electricityConsumptionCurrent, 3);
  publishMqtt(MQTT_TOPIC_ELECTRICITY_PRODUCTION_CURRENT, electricityProductionCurrent, 3);
  publishMqtt(MQTT_TOPIC_ELECTRICITY_PRODUCTION_LOW, electricityProductionLow, 3);
  publishMqtt(MQTT_TOPIC_ELECTRICITY_PRODUCTION_HIGH, electricityProductionHigh, 3);
  publishMqtt(MQTT_TOPIC_ELECTRICITY_VOLTAGE, electricityVoltage, 1);
  publishMqtt(MQTT_TOPIC_ELECTRICITY_TARIFF, electricityTariff);
  publishMqtt(MQTT_TOPIC_GAS_CONSUMPTION, gasConsumption, 3);
}
