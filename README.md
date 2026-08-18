# TrenoLEGO
LEGO Hub for old 4.5V, Power Functions trains &amp; more

## 1. Introduction  
*TrenoLEGO* was developed initially with the aim to create a complete LEGO hub for vintage 4.5V trains, but can also used to build
*Power Functions* (PF) with advanced features. It is based on ESP32 and ESP8266 MCUs and it was successfully tested with ESP32 S3 SuperMini, ESP32 C3 SuperMini Plus and NodeMCU 1.0/ESP8266 (with some limitations due to small quantity of RAM).  
The main features are:

- Support for old *4.5V motors* (but you can increase the power up to 6-6.5V without burning the MCU).
- *PoweredUp* emulation: *TrenoLEGO* can be configured as a real PoweredUp hub and can be connected with LEGO controllers and
  *PoweredUp App*
- Possibility to control up to 4 *Power Functions* trains by optional IR transmitter. 
- Sound effects and music.
- Soft start/stop to simulate acceleration and breaking.
- Support for *Bluetooth* controllers (they cannot connected directly to *TrenoLEGO*, but through the phone/tablet/PC. 
- *Philips Hue emulation* for Alexa/Amazon Echo voice commands (e.g. "Alexa, Holiday Express 50%").
- *MQTT client*, which can be used to control trains by your smart home system (e.g. openHAB, Home Assistant, etc.) 
- Battery monitoring system (BMS) to prevent the overdischarge of the battery and to check the residual capacity.
- *Telnet server* for remote configuration/debug.
- *FTP server* for GUI file update without rebuild the firmware.
- *On-The-Air* (OTA) update via web and Arduino OTA.
- Automatic language localization (*english* and *italian* supported).
- Speech synthesis.

## 2. Hardware requirements
- An ESP32 S3 SuperMini or ESP32 C3 SuperMini Plus or NodeMCU 1.0.  
  ESP32 C3 SuperMini Plus is preferred because it's small, cheap, and makes stable WiFi and BLE connections. Moreover, the connector present on its board allows to connect anexternal antenna, extending the operation range. Although ESP32 S3 SuperMini is more powerful (2 cores and additional PSRAM) if compared to the C3 version, is affected by a bad board design that doesn't permit stable connection due to the ceramic antenna too close to the main chip. NodeMCU or more in general ESP8266 doesn't have BLE support and, for this reason, cannot emulate a *PowerUp* hub. Moreover, due to the limited quantity of RAM, it cannot run all te services at the same time (e.g. ftp, telnet, MQTT and Alexa/Hue emulation). For this reason, enable only the services that you need.  
