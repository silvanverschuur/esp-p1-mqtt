# esp-p1-mqtt
Software for the ESP8266 that sends P1 smart meter data to a MQTT broker.

### Features
- Reads P1 telegram from smart meter
- Publish data to MQTT broker
- Uses WifiManager to create an AP if WiFi connection cannot be established

### Installation
- Clone this repository
- Download the [Arduino IDE](https://www.arduino.cc/)
- Install [Arduino core for ESP8266 WiFi chip](https://github.com/esp8266/Arduino)
- Install the [WifiManager library](https://github.com/tzapu/WiFiManager)
- Install the [PubSub MQTT client library](https://github.com/knolleary/pubsubclient)
- Edit pubsubclient.h to set the MQTT MAX MESSAGE SIZE to 512
- Adjust WiFi, MQTT and debug settings in `P1_settings.h`
- Compile and flash the ESP8266 module as usual

### Connecting the ESP8266 to the smart meter
The ESP8266 can be connected to the smart meter using a RJ11 connector. The ESP module can be powered from the smart meter. A couple of hardware components are added to the ESP8266 module to invert the serial signal from the smart meter. If you do not want to use additional hardware you can try to use the `SoftwareSerial` library to invert the signal in software.

I followed this schema to modify my ESP8266:

![](https://wiki.pieper.eu/wp-content/uploads/2016/03/P1port.png)

Use this schema to connect the RJ11 connector:

![RJ11 P1 connetor](http://gejanssen.com/howto/Slimme-meter-uitlezen/RJ11-pinout.png)

