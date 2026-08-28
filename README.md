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
All the hardware components were purchased from *AliExpress*. Pictures of the various components are provided below to help you identify suitable alternatives if you prefer to buy them from other online stores.

- **ESP32 S3 SuperMini or ESP32 C3 SuperMini Plus or NodeMCU 1.0**  
  The ESP32 C3 SuperMini Plus is preferred because it is small, inexpensive, and provides stable Wi-Fi and BLE connections. Moreover, the onboard connector allows an external antenna to be connected, extending the operating range. Although the ESP32 S3 SuperMini is more powerful than the C3 version (2 cores and additional PSRAM), it is affected by a poor board design that may result in an unstable connection due to the ceramic antenna being too close to the main chip. The NodeMCU, and ESP8266-based boards in general, do not support BLE and, for this reason, cannot emulate a *PowerUp* hub. Moreover, due to the limited amount of RAM, they cannot run all services at the same time (e.g., FTP, Telnet, MQTT, and Alexa/Hue emulation). For this reason, enable only the services you need.
  <table align="center">
   <tr>
    <td align="center"><img src="images/hardware/esp32_c3_supermini_plus.jpg" alt="ESP32 C3 SuperMini Plus" width="200"></td>
    <td align="center"><img src="images/hardware/esp32_s3_supermini.jpg" alt="ESP32 S3 SuperMini" width="200"></td>
    <td align="center"><img src="images/hardware/node_mcu.jpg" alt="Node MCU ESP8266" width="200"></td>
   </tr>
   <tr>
    <td align="center">ESP32 C3 SuperMini Plus</td>
    <td align="center">ESP32 S3 SuperMini</td>
    <td align="center">Node MCU ESP8266</td>
   </tr>
  </table>

- **DRV8833 motor driver**
  This compact motor driver can handle a maximum current of 1.5 A and features two independent channels for controlling two motors.
  <table align="center">
   <tr>
    <td align="center"><img src="images/hardware/drv8833.jpg" alt="DRV8833" width="200"></td>
   </tr>
   <tr>
    <td align="center">DRV8233dual channel motor driver</td>
   </tr>
  </table>
  
- **Power bank circuit board**  
  This board must provide a 5 V output with a current of 1–2 A and include the battery charging circuitry. Some boards sold on *AliExpress* have a small trimmer for adjusting the output voltage. If you want to power *LEGO Power Functions* motors or compatible clones, you can increase the output voltage up to 6.5 V.
  <table align="center">
   <tr>
    <td align="center"><img src="images/hardware/pwr_mod_5v3a.jpg" alt="Fixed 5V 2A board" width="200"></td>
    <td align="center"><img src="images/hardware/pwr_mod_5-9v2a_2.jpg" alt="5 to 9V adjustable power & charger" width="200"></td>
    <td align="center"><img src="images/hardware/pwr_mod_5-9v2a_1.jpg" alt="Smaller version on the previous module" width="200"></td>
   </tr>
   <tr>
    <td align="center">Fixed 5V 2A board. Perfect to power old 4,5V motors</td>
    <td align="center">5 to 9V adjustable power & charger. This board is perfect to be inserted into LEGO 88000 battery box</td>
    <td align="center">Smaller version on the previous module. Perfect for super-small projects</td>
   </tr>
  </table>
    
  **WARNING:**  
  Do not set the voltage higher than 6.5 V, as this may damage the ESP32's voltage regulator circuit. The NodeMCU may tolerate higher voltages, but this depends on the specific board design and the type of voltage regulator used.

- **Battery holder/box** (optional)  
  A battery holder is required if you intend to use batteries without solder tabs. One interesting solution is to buy an inexpensive battery box compatible with the *LEGO 88000*. It includes an electronic switch that can be used to turn the MCU on and off, as well as enough space to accommodate one or two 14500 cells. This is probably the best choice if you want a full-size, LEGO-compatible battery box without the need for 3D printing.  
  Another interesting option is to use three AA-to-C battery adapters, which fit inside the old *LEGO 161 battery wagon* in place of the original C batteries. The three adapters can be used to house the 14500 battery, the ESP32 C3 SuperMini, and the power board, respectively. This makes it possible to convert the old *LEGO 161 battery wagon* into a modern, self-contained hub without making any permanent modifications to the original LEGO part.
  
  <table align="center">
   <tr>
    <td align="center"><img src="images/hardware/battery_holder_18650.jpg" alt="Battery holder for 18650 cell" width="200"></td>
    <td align="center"><img src="images/hardware/battery_holder_88000.jpg" alt="Battery box compatible with LEGO 88000" width="200"></td>
    <td align="center"><img src="images/hardware/aa_to_c_adapter.jpg" alt="AA to C battery adapter" width="200"></td>
   </tr>
   <tr>
    <td align="center">Battery holder for 18650 cell</td>
    <td align="center">Battery box compatible with LEGO 88000</td>
    <td align="center">AA to C battery adapter. Three of these adapters can fit inside the old LEGO 161 battery vagon</td>
   </tr>
  </table>
  
- **1N4007 diode or equivalent**  
  This diode is required to protect the MCU and the power bank board when the MCU's USB port is connected to an external power source.
  <table align="center">
   <tr>
    <td align="center"><img src="images/hardware/1n4007.jpg" alt="1N4007 diode" width="200"></td>
   </tr>
   <tr>
    <td align="center">1N4007 diode</td>
   </tr>
  </table>
  
- **18650 or 14500 3.7 V Li-ion battery**  
  Choose the most suitable battery size according to the available space in the housing. The 18650 is inexpensive, widely available, and offers a high capacity (typically from 1500 to 3500 mAh), but it is relatively large. The 14500 battery is approximately the same size as a standard AA cell, making it suitable for smaller housings (e.g., the *LEGO 88000* 9 V battery box), but it has a lower capacity (typically from 500 to 1500 mAh).
  <table align="center">
   <tr>
    <td align="center"><img src="images/hardware/battery_18650.jpg" alt="18650" width="200"></td>
    <td align="center"><img src="images/hardware/battery_14500.jpg" alt="14500" width="200"></td>
   </tr>
   <tr>
    <td align="center">18650 battery</td>
    <td align="center">14500 battery</td>
   </tr>
  </table>
  
- **IR module** (optional)  
  This module is required if you want to control other *Power Functions* trains from the same web app provided by the MCU that directly controls the motor through the motor driver.
  <table align="center">
   <tr>
    <td align="center"><img src="images/hardware/ir_pcb0100.jpg" alt="IR transmitter board PCB0100" width="200"></td>
   </tr>
   <tr>
    <td align="center">IR transmitter board PCB0100</td>
   </tr>
  </table>
