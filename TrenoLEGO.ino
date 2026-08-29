

/**************************************
****        TrenoLEGO 0.8.1        ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/

/*
   LEGO hub for vintage 4.5V trains

   IDE settings for ESP32C3 Supermini:
   ===================================
   Board: ESP32C3
   CPU frequency: 160 MHz (WiFi)
   USB CDC on boot: enabled
   Partition scheme: Default 4MB with spiff (1.2MB APP / 1.5MB SPIFF)
 
   IDE settings for ESP32S3 Supermini (S3FH4R2):
   =============================================
   Board: ESP32S3
   CPU frequency: 240 MHz (WiFi)
   USB CDC on boot: enabled
   Partition scheme: Default 4MB with spiff (1.2MB APP / 1.5MB SPIFF)
   PSRAM: QSPI PSRAM (OPI PSRAM for R8 chips)

   IDE settings for ESP8266:
   =========================
   Board: NodeMCU 1.0 (ESP 12E Module)
   CPU frequency: 160 MHz

   Software dependencies:
   ======================
   ESP32 2.0.11 (only for ESP32S3)
   ESP8266 3.1.2 (only for ESP8266)
   ArduinoJSON 7.4.3
   Async TCP 2.0.0 (only for ESP32)
   CommandParser 1.1.1
   ESP_Async_TCP 3.4.10
   ESP_Async_WebServer 3.10.3
   ESP_Telnet 2.2.3
   FauxmoESP 3.4.1
   IRremoteESP8266 2.9.0
   Legoino 1.1.0 (only for ESP32)
   NimBLE Arduino 1.4.3 (only for ESP32)
   PubSubClient 2.8.0
   SimpleFTPServer 3.0.2 (change in FtpServerKey.h to #define DEFAULT_STORAGE_TYPE_ESP32 STORAGE_LITTLEFS)

   Connections:

   ESP32S3
   SuperMini   DRV8833   Power module    Battery   Switch   IR Module
   ==================================================================
   GPIO04 ------------------------------------------------- DAT
   GPIO05 ---- IN1
   GPIO06 ---- IN2
   GPIO07 ---- EEP
                     +-- 100K ---------- BAT+
   GPIO08 -----------+-- 100K ---------- BAT-
                     +-- 100 nF -------- BAT-
   GPIO15 ---------------------------------------- C
   GND ------- GND ----- GND ----------- BAT--------------- GND
   5V ---|<--- VCC ----- 5V-------------------------------- VCC
   3.3V ------------------------------------------ 1


   ESP32C3
   SuperMini   DRV8833   Power module    Battery   Switch   IR Module
   ==================================================================
   GPIO10 ------------------------------------------------- DAT
   GPIO00 ---- IN1
   GPIO01 ---- IN2
   GPIO03 ---- EEP
                     +-- 100K ---------- BAT+
   GPIO08 -----------+-- 100K ---------- BAT-
                     +-- 100 nF -------- BAT-
   GPIO02 ---------------------------------------- C
   GND ------- GND ----- GND ----------- BAT--------------- GND
   5V ---|<--- VCC ----- 5V-------------------------------- VCC
   GND ------------------------------------------- 1
  

   ESP8266
   NodeMCU     DRV8833   Power module    Battery   IR Module
   =========================================================
   D6 -------------------------------------------- DAT
   D1 -------- IN1
   D2 -------- IN2
   D5 -------- EEP
                     +-- 390K----------- BAT+
   A0 ---------------+-- 100K ---------- BAT-
                     +-- 100 nF -------- BAT-
   GND ------- GND ----- GND ----------- BAT- ---- GND
   5V --|<---- VCC ----- 5V ---------------------- VCC

   --|<-- = diode 1N4007
*/

/**** Build configuration ****/

#define  TR_HAS_BLE         /* The MCU has BLE                                */
#define  TR_BATT_BMS        /* Enable the software BMS to protect the battery */

/**** Includes ****/

#include <Arduino.h>
#include <string>
#include <map>
#include <ArduinoJson.h>
#include <ArduinoOTA.h>
#include <CommandParser.h>
#include <DNSServer.h>
#include <Print.h>
#include <stdarg.h>
#include <time.h>

#ifdef ESP8266
#  include <ESP8266WiFi.h>
#  include <ESP8266mDNS.h>
#  include <LittleFS.h>
#  include <Updater.h>

#  define  U_PART         U_FS
#  undef   TR_HAS_BLE
#  define  TR_HEAP_WARN   6000
#endif

#ifdef ESP32
#  include <WiFi.h>
#  include <FS.h>
#  include <LittleFS.h>
#  include <Update.h>
#  include <ESPmDNS.h>
#  include <esp_system.h>
#  include <esp_chip_info.h>

#  define  U_PART         U_SPIFFS
#  define  TR_HEAP_WARN   8000

/**** Fix for ESP32S3 ****/

#  if defined(RGB_BUILTIN) && defined(CONFIG_IDF_TARGET_ESP32S3)
#    undef  RGB_BUILTIN
#    define  RGB_BUILTIN  48
#  endif
#endif

#include <EEPROM.h>
#include <ESPAsyncWebServer.h>
#include <ESPTelnet.h>
#include <fauxmoESP.h>
#include <PubSubClient.h>
#include <SimpleFTPServer.h>

#ifdef TR_HAS_BLE
#  include <Lpf2HubEmulation.h>
#  include <LegoinoCommon.h>
#endif

#include <IRremoteESP8266.h>
#include <IRsend.h>

/**** Default credentials ****/

#include "credentials.h"

/***** Constants ****/

#define  TR_VERSION                     "0.8.1"
#define  TR_PREFS_VER                   0x00080101

/**** Default names ****/

#define  TR_ALEXA_TRAIN_0               "4.5V train"
#define  TR_ALEXA_TRAIN_0_REVERSE       "4.5V train reverse"
#define  TR_ALEXA_TRAIN_PF              "PF train %u"
#define  TR_ALEXA_TRAIN_PF_REVERSE      "PF train %u reverse"

/**** Misc ****/

#define  TR_HOSTNAME                    "TrenoLego"
#define  TR_HTML_DIR                    "/Html"
#define  TR_LANG_DEFAULT                0             /* Default lang (en)      */
#define  TR_HTTP_MAX_CLIENTS            4             /* Max concurrent clients */
#define  TR_PWM_DEBAND                  10
#define  TR_PWM_MIN                     160
#define  TR_PWM_MAX                     255
#define  TR_RGB_BRIGHTNESS              80            /* 0...255                */
#define  TR_SLEEP_TIME                  300           /* 300 sec. = 5 min.      */

/**** MQTT ****/

#define  TR_MQTT_AVAIL_REFRESH          60000         /* ms. 60 sec.            */
#define  TR_MQTT_BASETOPIC              "trenolego"   /* MQTT topic             */
#define  TR_MQTT_RETRY                  10            /* Retry times            */

/**** OTA modes ****/

#define  TR_OTA_NONE                    0
#define  TR_OTA_ARDUINO                 1
#define  TR_OTA_HTTP                    2

/**** Pending command ****/

#define  TR_PENDCMD_NONE                0
#define  TR_PENDCMD_MOTOR               2
#define  TR_PENDCMD_REBOOT              3
#define  TR_PENDCMD_SUSPEND             4

#define  TR_PREFS_FILENAME              "/prefs"
#define  TR_SERIAL_BAUDRATE             115200

/**** Telnet mode ****/

#define  TR_TELNET_USER                 0
#define  TR_TELNET_PASS                 1
#define  TR_TELNET_COM                  2

#define  TR_TELNET_MAX_INPUT            128
#define  TR_TELNET_MAX_RETRY            3

/**** WiFi modes ****/

#define  TR_WIFI_CONN_NONE              0
#define  TR_WIFI_CONN_AP                1
#define  TR_WIFI_CONN_STA               2

#define  TR_WIFI_RETRY                  60
#define  TR_WIFI_CHK_INTERVAL           10000 /* ms. */

/**** Pins ****/

#define  TR_PIN_LED                     LED_BUILTIN       /* Builtin (13 for NodeMCU/ESP8266) */

#ifdef ESP32
#  ifdef CONFIG_IDF_TARGET_ESP32C3
#    define  TR_PIN_BATTERY               9               /* Battery        */
#    define  TR_PIN_IR                   10               /* IR LED         */
#    define  TR_PIN_MOTOR_A_IN1           0               /* PWM            */
#    define  TR_PIN_MOTOR_A_IN2           1               /* Direction      */
#    define  TR_PIN_MOTOR_EEP             3               /* Enable         */
#    define  TR_PIN_SWITCH                2               /* Mode switch    */

#    define  TR_SWITCH_PULL               INPUT_PULLUP    /* Default state  */
#    define  TR_SWITCH_STATE              LOW             /* Switch state   */
#  else
#    define  TR_PIN_BATTERY               8               /* Battery        */
#    define  TR_PIN_IR                    4               /* IR LED (or 16) */
#    define  TR_PIN_MOTOR_A_IN1           5               /* PWM            */
#    define  TR_PIN_MOTOR_A_IN2           6               /* Direction      */
#    define  TR_PIN_MOTOR_EEP             7               /* Enable         */
#    define  TR_PIN_SWITCH                15              /* Mode switch    */

#    define  TR_SWITCH_PULL               INPUT_PULLDOWN  /* Default state  */
#    define  TR_SWITCH_STATE              HIGH            /* Switch state   */
#  endif

#  define  TR_MOTOR_PWM_CH1               0               /* PWM channel 1  */
#  define  TR_MOTOR_PWM_CH2               1               /* PWM channel 2  */
#  define  TR_MOTOR_PWM_FREQ              20000           /* 20 KHz         */
#  define  TR_MOTOR_PWM_RES               8               /* 0-255          */

#else
#  define  TR_PIN_BATTERY                 A0              /* Battery        */
#  define  TR_PIN_IR                      D6              /* IR LED         */
#  define  TR_PIN_MOTOR_A_IN1             D1              /* PWM            */
#  define  TR_PIN_MOTOR_A_IN2             D2              /* Direction      */
#  define  TR_PIN_MOTOR_EEP               D5              /* Enable         */

#  define  TR_MOTOR_PWM_FREQ              20000           /* 20 KHz         */
#endif

/**** Battery ****/

#ifdef ESP32
#  define  TR_ADC_MAX                     4095.0f
#  define  TR_BATT_VREF                   3100.0f
#else
#  define  TR_ADC_MAX                     1023.0f
#  define  TR_BATT_VREF                   1000.0f
#endif

/**** Battery management ****/

#define  TR_BATT_CALIB_FACTOR             1.13804f      /* Calibration factor to read mV      */
#define  TR_BATT_DIV_FACTOR               2.0f
#define  TR_BATT_LOW                      20            /* Battery low level at 20%           */
#define  TR_BATT_NUM_SAMPLES              10            /* Read samples to avoid fluctuations */
#define  TR_BATT_READ_DELAY               200           /* Read delay of 200 ms               */
#define  TR_BATT_VERYLOW                  10            /* BMS protection at 10% of charge    */

/**** Types ****/

typedef CommandParser<12, 3, 10, 32, 64> MyCommandParser; /* MaxCmd, MaxArg, MaxArgNameLen, MaxArgValLen, MaxCmdLineLen */

/**** Mime type ****/

typedef struct {
  const char *    FileExt;          /* File Extension              */
  const char *    Type;             /* Mime type                   */
} TR_MIME;

/**** Global variables ****/

AsyncWebServer            HttpServer(80);
char                      MqttClientID[32];
int                       TelnetLoginRetry, TelnetMode;
MyCommandParser           CmdParser;
String                    TelnetLoginUser;
unsigned long             LastCmdTime, MqttAvailTime, PendingCmdAt;

bool                      BattLow                 = true;
bool                      FtpEnabled              = false;
bool                      HasPSRAM                = false;
bool                      HueEnabled              = false;
bool                      MqttEnabled             = false;
bool                      TelnetEnabled           = false;
static const char         PrefsFileBak[]  PROGMEM = TR_PREFS_FILENAME ".bak";
static const char         PrefsFileName[] PROGMEM = TR_PREFS_FILENAME ".json";
static const char         PrefsFileTmp[]  PROGMEM = TR_PREFS_FILENAME ".tmp";
static const char *       PfPortStr[]             = { "RED", "BLUE", "UNK" };
DNSServer *               DnsServer               = NULL;
ESPTelnet *               Telnet                  = NULL;
fauxmoESP *               Fauxmo                  = NULL;
FtpServer *               FtpSrv                  = NULL;
int                       BattLevel               = 0;
int                       BattSampleCount         = 0;
int                       BattMilliVolts          = 0;
int                       HttpClients             = 0;
int                       OtaMode                 = TR_OTA_NONE;
int                       PendingCmd              = TR_PENDCMD_NONE;
int                       PendingMotorChannel     = 0;
int                       WiFiConnected           = TR_WIFI_CONN_NONE;
int                       PendingMotorSpeed       = 0;
PubSubClient *            MqttClient              = NULL;
unsigned long             BattSampleSum           = 0;
unsigned long             WiFiLastCheck           = 0;
WiFiClient *              EspClient               = NULL;

/**** TR_PIN_BATTERY discharge curve ****/

static const uint16_t     BattChargeCurve[] PROGMEM  = { 2850, 3165, 3388, 3500, 3550,
                                                         3575, 3620, 3612, 3625, 3633,
                                                         3651, 3663, 3686, 3718, 3780,
                                                         3800, 3850, 3900, 3975, 4100,
                                                         4200
                                                       };
/**** Preference keywords ****/

static const char         StrKeyAdminPass[]           PROGMEM = "AdminPass";
static const char         StrKeyAdminUser[]           PROGMEM = "AdminUser";
static const char         StrKeyDebug[]               PROGMEM = "Debug";
static const char         StrKeyFtpEnabled[]          PROGMEM = "FtpEnabled";
static const char         StrKeyHostName[]            PROGMEM = "HostName";
static const char         StrKeyHueEnabled[]          PROGMEM = "HueEnabled";
static const char         StrKeyLang[]                PROGMEM = "Lang";
static const char         StrKeyLegoHubAvailable[]    PROGMEM = "LegoHubAvail";
static const char         StrKeyMqttAvailRefresh[]    PROGMEM = "MqttAvailRefresh";
static const char         StrKeyMqttEnabled[]         PROGMEM = "MqttEnabled";
static const char         StrKeyMqttPass[]            PROGMEM = "MqttPass";
static const char         StrKeyMqttPort[]            PROGMEM = "MqttPort";
static const char         StrKeyMqttServer[]          PROGMEM = "MqttServer";
static const char         StrKeyMqttUser[]            PROGMEM = "MqttUser";
static const char         StrKeyNtpServer[]           PROGMEM = "NtpServer";
static const char         StrKeyPwmDeband[]           PROGMEM = "PwmDeband";
static const char         StrKeyPwmMax[]              PROGMEM = "PwmMax";
static const char         StrKeyPwmMin[]              PROGMEM = "PwmMin";
static const char         StrKeySleepEnabled[]        PROGMEM = "SleepEnabled";
static const char         StrKeySleepTime[]           PROGMEM = "SleepTime";
static const char         StrKeySpeechEnabled[]       PROGMEM = "SpeechEnabled";
static const char         StrKeyTelnetEnabled[]       PROGMEM = "TelnetEnabled";
static const char         StrKeyTimeZone[]            PROGMEM = "TimeZone";
static const char         StrKeyTrainAccelAmount[]    PROGMEM = "Train%uAccelAmount";
static const char         StrKeyTrainAccelStepTime[]  PROGMEM = "Train%uAccelStepTime";
static const char         StrKeyTrainEnabled[]        PROGMEM = "Train%uEnabled";
static const char         StrKeyTrainInvert[]         PROGMEM = "Train%uInvert";
static const char         StrKeyTrainName[]           PROGMEM = "Train%uName";
static const char         StrKeyTrainNameRev[]        PROGMEM = "Train%uNameRev";
static const char         StrKeyTrainPort[]           PROGMEM = "Train%uPort";
static const char         StrKeyVersion[]             PROGMEM = "Version";
static const char         StrKeyVibrationEnabled[]    PROGMEM = "VibrationEnabled";
static const char         StrKeyWiFiPass[]            PROGMEM = "WiFiPass";
static const char         StrKeyWiFiSSID[]            PROGMEM = "WiFiSSID";

/**** POwered Up variables ****/

#ifdef TR_HAS_BLE
Lpf2HubEmulation          EmulatedHub("TrainHub", HubType::POWERED_UP_HUB);

static const char *       PuPortStr[]         = { "A", "B", "UNK" };

/**** Preference keywords ****/

static const char         StrKeyLegoHubEnabled[]     PROGMEM = "LegoHubEnabled";
static const char         StrKeyLegoHubPort[]        PROGMEM = "LegoHubPort";
static const char         StrKeyLegoHubConnChannel[] PROGMEM = "LegoHubConnChannel";
static const char         StrKeyLegoHubConnPort[]    PROGMEM = "LegoHubConnPort";
#endif

/**** Prototypes ****/

void                      AddPendingCmd(int Cmd, unsigned long Delay);
void                      BattCalcLvl(unsigned int Sum, unsigned int Samples);
const char *              Bool2Str(bool Val);
const char *              Bool2Tf(bool Val);
int                       ChkTrue(const char *Str);
void                      CmdFormat(MyCommandParser::Argument *Args, char *Response);
void                      CmdHelp(MyCommandParser::Argument *Args, char *Response);
void                      CmdLoad(MyCommandParser::Argument *Args, char *Response);
void                      CmdMotor(MyCommandParser::Argument *Args, char *Response);
void                      CmdReboot(MyCommandParser::Argument *Args, char *Response);
void                      CmdSet(MyCommandParser::Argument *Args, char *Response);
void                      CmdSave(MyCommandParser::Argument *Args, char *Response);
void                      CmdShowPrefs(MyCommandParser::Argument *Args, char *Response);
void                      CmdSleep(MyCommandParser::Argument *Args, char *Response);
void                      CmdStatus(MyCommandParser::Argument *Args, char *Response);
void                      CmdStop(MyCommandParser::Argument *Args, char *Response);
void                      CmdTset(MyCommandParser::Argument *Args, char *Response);
void                      FtpCallback(FtpOperation FtpOperation, uint32_t FreeSpace, uint32_t TotalSpace);
void                      FtpTransferCallback(FtpTransferOperation FtpOperation, const char *Name, uint32_t TransferredSize);
void                      FtpSetup(void);
void                      FauxmoSetup(void);
const char *              GetPfPort(uint8_t PfPort);
const char *              HttpServerFindMime(String &FileName);
bool                      HttpServerHeapCheck(AsyncWebServerRequest *Request);
void                      HttpServerDoUpdate(AsyncWebServerRequest *Request, const String &FileName, size_t Index, uint8_t *Data, size_t Len, bool Final);
void                      HttpServerExecCgiCallBack(AsyncWebServerRequest *Request);
void                      HttpServerGetConfigCallBack(AsyncWebServerRequest *Request);
void                      HttpServerNotFoundCallBack(AsyncWebServerRequest *Request);
void                      HttpServerSendGzFile(AsyncWebServerRequest *Request, const __FlashStringHelper *FileName);
void                      HttpServerSetup(void);
void                      HttpServerUpdateCallBack(AsyncWebServerRequest *Request);
#ifdef RGB_BUILTIN
void                      LedColorWheel(uint8_t Pos, uint8_t Brightness);
#endif
bool                      IsDomainName(const char *Name, unsigned int MaxLen);
bool                      IsHostName(const char *Name, unsigned int MaxLen);
bool                      IsIP(const char *Ip);
void                      LedOff(void);
void *                    Malloc(size_t Size);
void                      MqttCallback(char *Topic, byte *Payload, unsigned int Length);
void                      MqttReconnect(void);
void                      MqttPubAvail(bool Online);
void                      MqttPubTrainState(void);
void                      MqttSetup(void);
void                      OtaSetup(void);
void                      Print(const char *Str);
void                      Print(const __FlashStringHelper *Str);
void                      Printf(const char *Fmt, ...);
void                      Printf(const __FlashStringHelper *Fmt, ...);
void                      PrintUpdateProgress(size_t Prg, size_t Sz);
void                      Reboot(void);
bool                      SetPfPort(uint8_t *PfPort, const char *Port);
size_t                    strlcpy_P(char *Dest, const char *Src, size_t Size);
void                      Suspend(void);
void                      TelnetOnConnect(String Ip);
void                      TelnetOnConnectionAttempt(String Ip);
void                      TelnetOnDisconnect(String Ip);
void                      TelnetOnInput(String Str);
void                      TelnetOnReconnect(String Ip);
void                      TelnetSetup(void);
void                      WiFiConnect(void);
void                      WiFiSetup(void);

/**** Powered Up functions ****/

#ifdef TR_HAS_BLE
const char *              GetPuPort(PoweredUpHubPort PuPort);
void                      LegoHubHandle(void);
void                      LegoHubCallback(byte Port, byte Value);
bool                      SetPuPort(PoweredUpHubPort *PuPort, const char *Port);
#endif

/**** Preferences & trains ****/

#include "trclasses.h"

TrPfTrains                PfTrains;
TrPrefs                   Prefs;


/**** Setup ****/

void setup()
{

  /**** Initialize the serial port ****/

  Serial.begin(TR_SERIAL_BAUDRATE);
  unsigned long SerialTime = millis();
  while ((!Serial) && ((millis() - SerialTime) < 1500))
    delay(10);

  Serial.print(F("\n\n**** TrenoLEGOS V" TR_VERSION " ****\n"));

  /**** Check if the PSRAM is available ****/

#if defined(BOARD_HAS_PSRAM) || defined(CONFIG_SPIRAM_SUPPORT)
  HasPSRAM = psramFound();
#endif

  /**** Set the pins ****/

#ifndef RGB_BUILTIN
  pinMode(TR_PIN_LED, OUTPUT);
#endif

  pinMode(TR_PIN_MOTOR_A_IN1, OUTPUT);
  pinMode(TR_PIN_MOTOR_A_IN2, OUTPUT);
  pinMode(TR_PIN_MOTOR_EEP  , OUTPUT);
  digitalWrite(TR_PIN_MOTOR_A_IN1, LOW );
  digitalWrite(TR_PIN_MOTOR_A_IN2, LOW );
  digitalWrite(TR_PIN_MOTOR_EEP  , HIGH);

#ifdef ESP32
  pinMode(TR_PIN_SWITCH, TR_SWITCH_PULL);

  analogReadResolution(12);
  analogSetPinAttenuation(TR_PIN_BATTERY, ADC_11db);
  delay(10);
  ledcSetup(TR_MOTOR_PWM_CH1, TR_MOTOR_PWM_FREQ, TR_MOTOR_PWM_RES);
  ledcSetup(TR_MOTOR_PWM_CH2, TR_MOTOR_PWM_FREQ, TR_MOTOR_PWM_RES);
  ledcAttachPin(TR_PIN_MOTOR_A_IN1, TR_MOTOR_PWM_CH1);
  ledcAttachPin(TR_PIN_MOTOR_A_IN2, TR_MOTOR_PWM_CH2);
#else
  analogWriteRange(255);
  analogWriteFreq(TR_MOTOR_PWM_FREQ);
#endif

  /**** Default pin states ****/

  PfTrains.MotorSpeed(TR_PF_CHANNEL_0, 0);

#ifndef RGB_BUILTIN
  digitalWrite(TR_PIN_LED, HIGH);
#endif

  /**** Start LittleFS ****/

#ifdef ESP32
  if (!LittleFS.begin(true)) {
#elif defined(ESP8266)
  if (!LittleFS.begin()) {
#endif
    if (Prefs.Debug)
      Serial.println(F("ERROR: Can't initialize LittleFS"));
    FtpEnabled = false;
#if defined(ESP32) || defined(ESP8266)
  }
#endif

  /**** Load the preferences ****/

  Prefs.Load(false);

  /**** Check the mode switch ****/

#ifdef TR_HAS_BLE
  delay(10);
  if (digitalRead(TR_PIN_SWITCH) == TR_SWITCH_STATE)
    Prefs.LegoHubEnabled = !Prefs.LegoHubEnabled;

  if (Prefs.LegoHubEnabled) {
    EmulatedHub.setWritePortCallback(&LegoHubCallback);
    EmulatedHub.start();
  }
#endif

  /**** NTP server ****/

#ifdef ESP32
#  ifdef TR_HAS_BLE
  if (!Prefs.LegoHubEnabled)
#  endif
    configTzTime(Prefs.TimeZone, Prefs.NtpServer, Prefs.NtpServer, Prefs.NtpServer);
#else
  configTime(Prefs.TimeZone, Prefs.NtpServer);
#endif

  /**** Setups ****/

#ifdef TR_HAS_BLE
  if (!Prefs.LegoHubEnabled)
#endif
    WiFiSetup();

  if (!WiFiConnected) {
    FtpEnabled    = false;
    HueEnabled    = false;
    MqttEnabled   = false;
    TelnetEnabled = false;
  } else {
    FtpEnabled    = Prefs.FtpEnabled;
    HueEnabled    = Prefs.HueEnabled  && (WiFiConnected == TR_WIFI_CONN_STA);
    MqttEnabled   = Prefs.MqttEnabled && (WiFiConnected == TR_WIFI_CONN_STA);
    TelnetEnabled = Prefs.TelnetEnabled;

    HttpServerSetup();
    OtaSetup();
  }

  if (FtpEnabled   ) FtpSetup();
  if (HueEnabled   ) FauxmoSetup();
  if (MqttEnabled  ) MqttSetup();
  if (TelnetEnabled) TelnetSetup();

  /**** Register the CLI commands ****/

  CmdParser.registerCommand("format"   , "s"  , CmdFormat    );
  CmdParser.registerCommand("help"     , ""   , &CmdHelp     );
  CmdParser.registerCommand("load"     , "s"  , &CmdLoad     );
  CmdParser.registerCommand("motor"    , "us" , &CmdMotor    );
  CmdParser.registerCommand("reboot"   , ""   , &CmdReboot   );
  CmdParser.registerCommand("save"     , ""   , &CmdSave     );
  CmdParser.registerCommand("set"      , "ss" , &CmdSet      );
  CmdParser.registerCommand("showprefs", ""   , &CmdShowPrefs);
  CmdParser.registerCommand("sleep"    , ""   , &CmdSleep    );
  CmdParser.registerCommand("status"   , ""   , &CmdStatus   );
  CmdParser.registerCommand("tset"     , "uss", &CmdTset     );

  /**** Random seed ****/

  randomSeed(micros());

  /**** Finish ****/

  if (WiFiConnected) {
#ifdef RGB_BUILTIN
    if (WiFiConnected == TR_WIFI_CONN_STA)
      neopixelWrite(RGB_BUILTIN, 0, TR_RGB_BRIGHTNESS, 0);
    else
      neopixelWrite(RGB_BUILTIN, 0, 0, TR_RGB_BRIGHTNESS);
#else
#  ifdef ESP8266
    digitalWrite(TR_PIN_LED, LOW);
#  else
    digitalWrite(TR_PIN_LED, HIGH);
#  endif
#endif
    delay(500);
  } else {
#ifdef RGB_BUILTIN
#  ifdef TR_HAS_BLE
    if (Prefs.LegoHubEnabled)
      neopixelWrite(RGB_BUILTIN, 0, TR_RGB_BRIGHTNESS, TR_RGB_BRIGHTNESS);
    else
#  endif
      neopixelWrite(RGB_BUILTIN, TR_RGB_BRIGHTNESS, 0, 0);
#endif
  }

  /**** Start battery level ****/

  BattCalcLvl(analogRead(TR_PIN_BATTERY), 1);

  /**** Finish ****/

  Serial.print(F("\nType help for the command list\n"
                 "Ready\n"));

  LastCmdTime = millis();
}


/**** Main loop ****/

void loop()
{
  bool    MqttPub = false;

  /**** OTA ****/

#ifdef TR_HAS_BLE
  if (!Prefs.LegoHubEnabled) {
#endif
    if (WiFi.status() == WL_CONNECTED) {
      ArduinoOTA.handle();
      if (OtaMode == TR_OTA_ARDUINO)
        return;
    }
#ifdef TR_HAS_BLE
  }
#endif

  /**** Pending command ****/

  switch (PendingCmd) {
    case TR_PENDCMD_MOTOR:
      PfTrains.MotorSpeed(PendingMotorChannel, PendingMotorSpeed);
      PfTrains.SetCurrentSpeed(PendingMotorChannel, PendingMotorSpeed);
      MqttPub    = true;
      PendingCmd = TR_PENDCMD_NONE;
      break;

    case TR_PENDCMD_REBOOT:
      if ((long)(millis() - PendingCmdAt) > 0)
        Reboot();
      break;

    case TR_PENDCMD_SUSPEND:
      if ((long)(millis() - PendingCmdAt) > 0)
        Suspend();
      break;
  } /* End of switch */

  /**** Motor loop ****/

  if (PfTrains.MotorSpeedLoop()) MqttPub = true;

  /**** Mange the CLI ****/

  if (Serial.available()) {
    char    Line[128];
    char    Response[MyCommandParser::MAX_RESPONSE_SIZE];

    size_t  LineLength = Serial.readBytesUntil('\n', Line, sizeof(Line) - 1);

    if (LineLength) {
      Line[LineLength] = 0;

      if (Line[--LineLength] == '\r')
        Line[LineLength] = 0;

      if (LineLength) {
        *Response = 0;
        CmdParser.processCommand(Line, Response);
        if (*Response) Serial.println(Response);
        LastCmdTime = millis();
      }
    } else Serial.println();
  }

  /**** Auto sleep mode ****/

  if ((Prefs.SleepEnabled) &&
      (millis() - LastCmdTime >= Prefs.SleepTime * 1000UL))
    Suspend();

  /**** LEGO hub enabled ****/

#ifdef TR_HAS_BLE
  if (Prefs.LegoHubEnabled)
    LegoHubHandle();
#endif

  if (WiFiConnected) {

    /**** Check the WiFi connection ****/

    if ((WiFiConnected == TR_WIFI_CONN_STA) &&
        (millis() > (WiFiLastCheck + TR_WIFI_CHK_INTERVAL))) {
      WiFiLastCheck = millis();

      if (WiFi.status() != WL_CONNECTED) {
        if (Prefs.Debug)
          Serial.print(F("\n[DEBUG] ERROR: WiFi lost. Retrying\n"));
        delay(1000);
        WiFiConnect();
      }
    }

    /**** Manage Hue ****/

    if ((HueEnabled) && (Fauxmo)) Fauxmo -> handle();

    /**** DNS ****/

    if (DnsServer) DnsServer -> processNextRequest();

    /**** FTP ****/

    if ((FtpEnabled) && (FtpSrv)) FtpSrv -> handleFTP();

    /**** MQTT ****/

    if ((MqttEnabled) && (MqttClient)) {
      if (MqttClient -> connected()) {
        MqttClient -> loop();
        if (millis() - MqttAvailTime >= Prefs.MqttAvailRefresh) {
          MqttPubAvail(true);
          MqttPub = true;
        }
        if (MqttPub) MqttPubTrainState();
      } else {
        MqttReconnect();
      }
    }

    /**** Telnet ****/

    if ((TelnetEnabled) && (Telnet)) Telnet -> loop();
  }

  /**** Battery check ****/

  static unsigned long LastBatt = 0;

  if (millis() - LastBatt >= TR_BATT_READ_DELAY) {
    LastBatt = millis();

    BattSampleSum += (unsigned int)analogRead(TR_PIN_BATTERY);

    if ((++BattSampleCount) >= TR_BATT_NUM_SAMPLES) {
      BattCalcLvl(BattSampleSum, BattSampleCount);
      BattSampleSum   = 0;
      BattSampleCount = 0;

      /**** Suspend the system when the battery is too low ****/

#ifdef TR_BATT_BMS
      if ((BattLevel > 1) && (BattLevel <= TR_BATT_VERYLOW))
        Suspend();
#endif
    }
  }

#ifdef ESP32
  delay(10);
#endif
}
