#define MAXLINELENGTH 64
char telegram[MAXLINELENGTH];

unsigned int currentCRC=0;

bool isNumber(char *res, int len) {
  for (int i = 0; i < len; i++) {
    if (((res[i] < '0') || (res[i] > '9')) && (res[i] != '.' && res[i] != 0)) {
      return false;
    }
  }
  return true;
}

int FindCharInArrayRev(char array[], char c, int len) {
  for (int i = len - 1; i >= 0; i--) {
    if (array[i] == c) {
      return i;
    }
  }
  return -1;
}

float getIntValue(char *buffer, int maxlen) {
  int s = FindCharInArrayRev(buffer, '(', maxlen - 2);
  if (s < 8)
    return 0;

  int l = FindCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
  if (l < 0)
    l = FindCharInArrayRev(buffer, ')', maxlen - 2) - s - 1;
  
  if (l < 4)
    return 0;

  char res[16];
  memset(res, 0, sizeof(res));
  if (strncpy(res, buffer + s + 1, l)) {
    if (isNumber(res, l)) {
      return atoi(res);
    }
  }
  return 0;
}

float getFloatValue(char *buffer, int maxlen) {
  int s = FindCharInArrayRev(buffer, '(', maxlen - 2);
  if (s < 8)
    return 0;

  int l = FindCharInArrayRev(buffer, '*', maxlen - 2) - s - 1;
  if (l < 4)
    return 0;

  char res[16];
  memset(res, 0, sizeof(res));
  if (strncpy(res, buffer + s + 1, l)) {
    if (isNumber(res, l)) {
      return atof(res);
    }
  }
  return 0;
}

bool decodeTelegram(int len) {
  //need to check for start
  int startChar = FindCharInArrayRev(telegram, '/', len);
  int endChar = FindCharInArrayRev(telegram, '!', len);
  bool validCRCFound = false;
  
  if (startChar >= 0) {
    //start found. Reset CRC calculation
    currentCRC = CRC16(0x0000, (unsigned char*)telegram + startChar, len - startChar);
    
    if (OUTPUT_TELEGRAM_ON_SERIAL) {
      for (int cnt = startChar; cnt < len - startChar; cnt++)
        Serial.print(telegram[cnt]);
    }
  } else if (endChar >= 0) {
    // Add to CRC calc 
    currentCRC = CRC16(currentCRC, (unsigned char*)telegram + endChar, 1);
    char messageCRC[5];
    strncpy(messageCRC, telegram + endChar + 1, 4);
    messageCRC[4]=0;
    
    if (OUTPUT_TELEGRAM_ON_SERIAL) {
      for (int cnt = 0; cnt < len; cnt++)
        Serial.print(telegram[cnt]);
    }

    validCRCFound = (strtol(messageCRC, NULL, 16) == currentCRC);
    if(!validCRCFound) {
      mqttClient.publish(MQTT_TOPIC_STATUS, "Error decoding telegram: invalid CRC");
      if (OUTPUT_TELEGRAM_ON_SERIAL) {
        Serial.println("\n===INVALID CRC FOUND!===");
      }
    }
      
    currentCRC = 0;
  } else {
    currentCRC=CRC16(currentCRC, (unsigned char*)telegram, len);
    
    if (OUTPUT_TELEGRAM_ON_SERIAL) {
      for (int cnt = 0; cnt < len; cnt++)
        Serial.print(telegram[cnt]);
    }
  }

  // 1-0:1.8.1(000992.992*kWh)
  // 1-0:1.8.1 = Elektra verbruik laag tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.8.1", strlen("1-0:1.8.1")) == 0)
    electricityConsumptionLow = getFloatValue(telegram, len);

  // 1-0:1.8.2(000560.157*kWh)
  // 1-0:1.8.2 = Elektra verbruik hoog tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.8.2", strlen("1-0:1.8.2")) == 0)
    electricityConsumptionHigh = getFloatValue(telegram, len);
  
  // 1-0:2.8.1(000348.890*kWh)
  // 1-0:2.8.1 = Elektra opbrengst laag tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:2.8.1", strlen("1-0:2.8.1")) == 0)
    electricityProductionLow = getFloatValue(telegram, len);

  // 1-0:2.8.2(000859.885*kWh)
  // 1-0:2.8.2 = Elektra opbrengst hoog tarief (DSMR v4.0)
  if (strncmp(telegram, "1-0:2.8.2", strlen("1-0:2.8.2")) == 0)
    electricityProductionHigh = getFloatValue(telegram, len);

  // 1-0:1.7.0(00.424*kW) Actueel verbruik
  // 1-0:2.7.0(00.000*kW) Actuele teruglevering
  // 1-0:1.7.x = Electricity consumption actual usage (DSMR v4.0)
  if (strncmp(telegram, "1-0:1.7.0", strlen("1-0:1.7.0")) == 0)
    electricityConsumptionCurrent = getFloatValue(telegram, len);

  if (strncmp(telegram, "1-0:2.7.0", strlen("1-0:2.7.0")) == 0)
    electricityProductionCurrent = getFloatValue(telegram, len);

  // 0-1:24.2.1(150531200000S)(00811.923*m3)
  // 0-1:24.2.1 = Gas (DSMR v4.0) on Kaifa MA105 meter
  if (strncmp(telegram, "0-1:24.2.1", strlen("0-1:24.2.1")) == 0)
    gasConsumption = getFloatValue(telegram, len);

  // 1-0:32.7.0(229.0*V)
  if (strncmp(telegram, "1-0:32.7.0", strlen("1-0:32.7.0")) == 0)
    electricityVoltage = getFloatValue(telegram, len);

  // 0-0:96.14.0(0002)
  if (strncmp(telegram, "0-0:96.14.0", strlen("0-0:96.14.0")) == 0)
    electricityTariff = getIntValue(telegram, len);
  
  return validCRCFound;
}

boolean readTelegram() {
  if (Serial.available()) {
    if (OUTPUT_TELEGRAM_ON_SERIAL) {
      Serial.println("\nTrying to read");
    }
    
    memset(telegram, 0, sizeof(telegram));
    while (Serial.available()) {
      int len = Serial.readBytesUntil('\n', telegram, MAXLINELENGTH);
      telegram[len] = '\n';
      telegram[len + 1] = 0;
      yield();

      return (decodeTelegram(len + 1));
    }
  }

  return false;
}
