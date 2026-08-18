# TrenoLEGO
LEGO Hub for old 4.5V, Power Functions trains &amp; more

## 1. Introduction  
*TrenoLEGO* was developed initially with the aim to create a complete LEGO hub for vintage 4.5V trains, but can also used to build
*Power Functions* (PF) with advanced features. It is based on ESP32 and ESP8266 MCUs and it was successfully tested with ESP32 S3 SuperMini, ESP32 C3 SuperMini Plus and NodeMCU 1.0/ESP8266 (with some limitations due to small quantity of RAM).  
The main features are:

- Support for old *4.5V motors* (but you can increase the power up to 6-6.5V without burning the MCU).
- *PoweredUp* emulation: *TrenoLEGO* can be configured as a real PoweredUp hub and can be connected with LEGO controllers and
  *PoweredUp App*
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
