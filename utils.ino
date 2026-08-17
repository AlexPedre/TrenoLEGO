
/**************************************
****     TrenoLEGO - Utilities     ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Local variables ****/

static const char   MsgErrOutOfMem[] PROGMEM = "[DEBUG] Printf() out of memory";


/**** Add a pending command ****/

void AddPendingCmd(int Cmd, unsigned long Delay)
{
  PendingCmd   = Cmd;
  PendingCmdAt = millis() + Delay;
}


/**** Calculate the battery level ****/

void BattCalcLvl(unsigned int Sum, unsigned int Samples)
{
  BattMilliVolts = (int)(((float)Sum / (float)Samples / TR_ADC_MAX) * 
                   TR_BATT_VREF * TR_BATT_DIV_FACTOR * TR_BATT_CALIB_FACTOR);

       if (BattMilliVolts < 1000) BattLevel = 0;
  else if (BattMilliVolts > 4200) BattLevel = 100;
  else {
    BattLevel = 0;
    for(int i = 0; i < 20; i++) {
      uint16_t    p1 = pgm_read_word(BattChargeCurve + i);
      uint16_t    p2 = pgm_read_word(BattChargeCurve + i + 1);
      
      if ((BattMilliVolts >= p1) && (BattMilliVolts <= p2)) {
        BattLevel = (i * 5) + map(BattMilliVolts, p1, p2, 1, 5);
        break;
      }
    } /* End of for (i) */
  }

  BattLow = (BattLevel <= TR_BATT_LOW);
}


/**** Convert boolean value to string ****/

const char * Bool2Str(bool Val)
{
  return Val ? "yes" : "no";
}


/**** Convert boolean value to string ****/

const char * Bool2Tf(bool Val)
{
  return Val ? "true" : "false";
}


/**** Check if true/false ****/

int ChkTrue(const char *Str)
{
  if (Str) {
    if ((!strcasecmp_P(Str, PSTR("true"))) || (!strcasecmp_P(Str, PSTR("on"))) ||
        (!strcasecmp_P(Str, PSTR("yes" ))) || (!strcasecmp_P(Str, PSTR("1" ))))
      return 1;  

    if ((!strcasecmp_P(Str, PSTR("false"))) || (!strcasecmp_P(Str, PSTR("off"))) ||
        (!strcasecmp_P(Str, PSTR("no"   ))) || (!strcasecmp_P(Str, PSTR("0"  ))))
      return 0;
  }

  return -1;
}


/**** Fauxmo setup ****/

void FauxmoSetup(void)
{
  Fauxmo = new fauxmoESP();
  Fauxmo -> createServer(false);
  Fauxmo -> setPort(80);
  Fauxmo -> enable(true);

  /**** Hue virtual devices ****/
  
  int AlexaID = 0;
  for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel) {
    if (PfTrains.GetEnabled(Channel)) {
      Fauxmo -> addDevice(PfTrains.GetName(Channel));
      Fauxmo -> addDevice(PfTrains.GetNameReverse(Channel));
      PfTrains.SetAlexaID(Channel, AlexaID);
      AlexaID += 2;
    }
  } /* End of for (k) */

  /**** Hue callback ****/

  Fauxmo -> onSetState([](unsigned char DeviceID, const char * DeviceName, bool State, unsigned char Value) {
    if (Prefs.Debug) {
      Printf(F("[DEBUG] Fauxmo device #%d (%s) state: %S value: %d\r\n"), 
             DeviceID, DeviceName, State ? PSTR("ON") : PSTR("OFF"), Value);
    }

    uint8_t Channel = PfTrains.FindChannel(DeviceID);

    if (Channel != TR_PF_CHANNEL_UNK) {
      int MotorValue = (DeviceID & 1) ? -Value : Value;

      if (Prefs.Debug) {
        Printf(F("[DEBUG] Fauxmo channel #%d motor value: %d\r\n"), 
               Channel, MotorValue);
      }
 
      PfTrains.MotorSpeedSoft(Channel, State ? MotorValue : 0);
      LastCmdTime = millis();
    }
  });
}


/**** Get the Power Function port string ****/

const char * GetPfPort(uint8_t PfPort)
{
  if (PfPort > 2) PfPort = 2;

  return PfPortStr[PfPort];
}


/**** Get the Powered Up port string ****/

#ifdef TR_HAS_BLE
const char * GetPuPort(PoweredUpHubPort PuPort)
{
  if ((int)PuPort > 3) PuPort = (PoweredUpHubPort)3;

  return PuPortStr[(int)PuPort];
}
#endif


/**** Check if it's a valid domain name ****/

bool IsDomainName(const char *Name, unsigned int MaxLen)
{
  if ((!Name) || (!*Name) || (MaxLen < 1))
    return false;

  bool          LastDot  = true;
  const char *  Ptr      = Name;
  unsigned int  LabelLen = 0;
  unsigned int  Len      = 0;

  while(*Ptr) {
    unsigned char c = (unsigned char)*Ptr;

    if (Len >= MaxLen) return false;

    if (c == '.') {
      if ((LastDot) || (LabelLen == 0) || (*(Ptr - 1) == '-'))
        return false;

      LabelLen = 0;
      LastDot  = true;
    } else {
      if (!((c >= 'A' && c <= 'Z') ||
            (c >= 'a' && c <= 'z') ||
            (c >= '0' && c <= '9') ||
            (c == '-')))
        return false;

      if ((LabelLen == 0) && (c == '-'))
        return false;

      LabelLen++;
      if (LabelLen > 63) return false;

      LastDot = false;
    }

    Len++;
    Ptr++;
  } /* End of while */

  if ((LastDot) || (*(Ptr - 1) == '-')) 
    return false;

  return true;
}


/**** Check if it's a valid IP ****/

bool IsIP(const char *Ip)
{
  if ((!Ip) || (!*Ip)) return false;

  int Octets = 0;

  while (*Ip) {
    if (!isdigit((unsigned char)*Ip))
      return false;

    int           Value  = 0;
    
    int           Digits = 0;
    const char *  Start  = Ip;

    while((*Ip) && (isdigit((unsigned char)*Ip))) {
      Value = Value * 10 + (*Ip - '0');
      Digits++;

      if (Value > 255) return false;
      Ip++;
    } /* End of while */

    if ((!Digits) ||
        ((Digits > 1) && (*Start == '0')))
      return false;

    Octets++;

    if (Octets < 4) {
      if (*Ip != '.') return false;
      Ip++;
      if (!*Ip) return false;
    } else {
      if (*Ip) return false;
    }
  } /* End of while */

  return (Octets == 4);
}


/**** Check if it's a valid host name ****/

bool IsHostName(const char *Name, unsigned int MaxLen)
{
  if ((!Name) || (!*Name) || (*Name == '-') || (MaxLen < 1))
    return false;

  const char *  Ptr = Name;
  int           Len = 0;

  while(*Ptr) {
    if ((Len >= MaxLen) ||
        (!(isalnum((unsigned char)*Ptr) || *Ptr == '-')))
      return false;
    Len++;
    Ptr++;
  } /* End of while */

  if (Name[Len - 1] == '-')
    return false;

  return true;
}


/**** Set the LED color cyclically ****/

#ifdef RGB_BUILTIN
void LedColorWheel(uint8_t Pos, uint8_t Brightness)
{
  uint8_t   r, g, b;

  Pos = 255 - Pos;

  if (Pos < 85) {
    r = 255 - Pos * 3;
    g = 0;
    b = Pos * 3;
  } else if (Pos < 170) {
    Pos -= 85;
    r    = 0;
    g    = Pos * 3;
    b    = 255 - Pos * 3;
  } else {
    Pos -= 170;
    r    = Pos * 3;
    g    = 255 - Pos * 3;
    b    = 0;
  }

  r = (r * Brightness) >> 8;
  g = (g * Brightness) >> 8;
  b = (b * Brightness) >> 8;

  neopixelWrite(RGB_BUILTIN, r, g, b);
}
#endif


/**** Turn off the led ****/

void LedOff(void)
{
#ifdef ESP8266
  digitalWrite(TR_PIN_LED, HIGH);
#else
#  ifdef RGB_BUILTIN
  neopixelWrite(RGB_BUILTIN, 0, 0, 0);
#  else
  digitalWrite(TR_PIN_LED, LOW);
#  endif 
#endif
}


/**** Malloc() replacement, using PSRAM if available ****/

void * Malloc(size_t Size)
{
  if (!Size) return NULL;

#if defined(BOARD_HAS_PSRAM) || defined(CONFIG_SPIRAM_SUPPORT)
  return HasPSRAM ? (char *)ps_malloc(Size) : (char *)malloc(Size);
#else
  return malloc(Size);
#endif 
}


/**** Print to serial and/or telnet ****/

void Print(const char *Str) 
{
  if ((!Str) || (!*Str)) return;

  Serial.print(Str);

  if ((TelnetEnabled) && (Telnet) && (Telnet -> isConnected()))
    Telnet -> print(Str);
}


/**** Print to serial and/or telnet (flash version) ****/

void Print(const __FlashStringHelper *Str)
{
  if (!Str) return;

  Serial.print(Str);

  if ((TelnetEnabled) && (Telnet) && (Telnet -> isConnected()))
    Telnet -> print(Str); 
}


/**** Printf to serial and/or telnet****/

void Printf(const char *Fmt, ...) 
{
  if ((!Fmt) || (!*Fmt)) return;
  
  va_list     Arg, Copy;
  
  va_start(Arg, Fmt);

  va_copy(Copy, Arg);
  int Len = vsnprintf(NULL, 0, Fmt, Copy);
  va_end(Copy);

  if (Len > 0) {
    size_t    Size = Len + 1;
    
    if (Size <= 128) {
      char      Buf[128];
      
      vsnprintf(Buf, Size, Fmt, Arg);
      Serial.print(Buf);
      if ((TelnetEnabled) && (Telnet) && (Telnet -> isConnected()))
        Telnet -> print(Buf);
    } else {
      char *    Buf = (char *)Malloc(Size);

      if (Buf) {
        vsnprintf(Buf, Size, Fmt, Arg);
        Serial.print(Buf);
        if ((TelnetEnabled) && (Telnet) && (Telnet -> isConnected()))
          Telnet -> print(Buf);
        free(Buf);
      } else if (Prefs.Debug) {
        Serial.println(FPSTR(MsgErrOutOfMem));
      }
    }
  }

  va_end(Arg);
}


/**** Printf to serial and/or telnet (flash version) ****/

void Printf(const __FlashStringHelper *Fmt, ...) 
{
  va_list     Arg, Copy;
  PGM_P       pFmt = reinterpret_cast<PGM_P>(Fmt);

  va_start(Arg, Fmt);
  va_copy(Copy, Arg);
  int Len = vsnprintf_P(NULL, 0, pFmt, Copy);
  va_end(Copy);

  if (Len > 0) {
    size_t    Size = Len + 1;
    
    if (Size <= 128) {
      char      Buf[128];
      
      vsnprintf_P(Buf, Size, pFmt, Arg);
      Serial.print(Buf);
      if ((TelnetEnabled) && (Telnet) && (Telnet -> isConnected())) 
        Telnet -> print(Buf);
    } else {
      char *    Buf = (char *)Malloc(Size);

      if (Buf) {
        vsnprintf_P(Buf, Size, pFmt, Arg);
        Serial.print(Buf);
        if ((TelnetEnabled) && (Telnet) && (Telnet -> isConnected()))
          Telnet -> print(Buf);
        free(Buf);
      } else if (Prefs.Debug) {
        Serial.println(FPSTR(MsgErrOutOfMem));
      }
    }
  }

  va_end(Arg);
}


/**** Reboot ****/

void Reboot(void)
{
  if (Prefs.Debug)
    Print(F("[DEBUG] Rebooting\r\n"));
 
  if ((TelnetEnabled) && (Telnet) && (Telnet -> isConnected())) {
    Telnet -> flush();
    delay(10);
    Telnet -> disconnectClient();
  }

  LedOff();

  if (Serial) Serial.flush();

  delay(1000);
  ESP.restart();
}


/**** Set the PowerFunction port from string ****/

bool SetPfPort(uint8_t *PfPort, const char *Port)
{
  if ((!PfPort) || (!Port)) return false;

  if (!strcasecmp(Port, PfPortStr[0])) {
    *PfPort = TR_PF_PORT_RED;
    return true;
  }

   if (!strcasecmp(Port, PfPortStr[1])) {
    *PfPort = TR_PF_PORT_BLUE;
    return true;
  }

  return false;
}


/**** Set the Powered Up port from string ****/

#ifdef TR_HAS_BLE
bool SetPuPort(PoweredUpHubPort *PuPort, const char *Port)
{
  if ((!PuPort) || (!Port) || (strlen(Port) != 1)) return false;

  int PortVal = toupper(*Port);

  if ((PortVal == 'A') || (PortVal == 'B')) {
    *PuPort = (PoweredUpHubPort)(PortVal - 'A');
    return true;
  }

  return false;
}
#endif


/**** strlcpy_P() ****/

size_t strlcpy_P(char *Dest, const char *Src, size_t Size) 
{
  size_t    SrcLen = strlen_P(Src);
  
  if (Size > 0) {
    size_t Len = (SrcLen >= Size) ? Size - 1 : SrcLen;
    
    memcpy_P(Dest, Src, Len);
    Dest[Len] = '\0';
  }
  
  return SrcLen;
}


/**** Suspend the system ****/

void Suspend(void)
{
  /**** Stop the trains ****/

  for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel)
    PfTrains.MotorSpeed(Channel, 0);
 
#ifdef ESP32
  ledcWrite(TR_MOTOR_PWM_CH1, 0);
  ledcWrite(TR_MOTOR_PWM_CH2, 0);

  ledcDetachPin(TR_PIN_MOTOR_A_IN1);
  pinMode(TR_PIN_MOTOR_A_IN1, OUTPUT);
  pinMode(TR_PIN_MOTOR_A_IN2, OUTPUT);
  digitalWrite(TR_PIN_MOTOR_A_IN1, LOW);
  digitalWrite(TR_PIN_MOTOR_A_IN2, LOW);
#endif

  /**** Turn off the DRV8833 ****/

  digitalWrite(TR_PIN_MOTOR_EEP, LOW);

  if ((TelnetEnabled) && (Telnet) && (Telnet -> isConnected()))
    Telnet -> disconnectClient();
  
  if ((MqttEnabled) && (MqttClient) && (MqttClient -> connected())) {
    MqttPubAvail(false);
    delay(500);
    MqttClient -> disconnect();
  }

  /**** Turn off the LED ****/

  LedOff();

  if (Serial) Serial.flush();

  delay(1000);

#ifdef ESP32
  esp_deep_sleep_start();
#endif

#ifdef ESP8266
  ESP.deepSleep(0);
#endif
}


/**** Connect to WiFi ****/

void WiFiConnect(void)
{
  if (Prefs.Debug)
    Serial.printf_P(PSTR("[DEBUG] Connecting WiFi to %s "), Prefs.WiFiSSID);

#ifdef ESP32
  WiFi.disconnect(true, true);
#else
  WiFi.disconnect(true);
#endif

  delay(500);

  WiFi.begin(Prefs.WiFiSSID, Prefs.WiFiPass);

  /**** Wait for the connection ****/

#ifdef RGB_BUILTIN
  uint8_t ColorIndex = 0;
#else
#  ifdef ESP32
  int LedStatus = HIGH;
#  else
  int LedStatus = LOW;
#  endif
#endif

  for(int k = 0; k < TR_WIFI_RETRY; ++k) {
#ifdef RGB_BUILTIN
    LedColorWheel(ColorIndex, TR_RGB_BRIGHTNESS);
    ColorIndex += 10;
#else
    digitalWrite(TR_PIN_LED, LedStatus);
    LedStatus = !LedStatus;
#endif
    if (WiFi.status() == WL_CONNECTED) {
#ifdef ESP32
     // WiFi.setSleep(false);
#endif
      WiFiConnected = TR_WIFI_CONN_STA;
      delay(500);
      break;
    }
    if (Prefs.Debug) Serial.print(".");
    delay(500);
  } /* End of for (k) */

  LedOff();

  if ((WiFiConnected) && (Prefs.Debug))
    Serial.printf_P(PSTR("\n[DEBUG] WiFi connected, SSID: %s, IP address: %s\n"), 
                    WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}


/**** WiFi setup ****/

void WiFiSetup(void) 
{
#ifdef CONFIG_IDF_TARGET_ESP32C3
  WiFi.persistent(true);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);
//  WiFi.disconnect(true, true);
  delay(500);
  WiFi.setSleep(false);
#else
  WiFi.persistent(true);
  WiFi.setAutoReconnect(true);
  WiFi.mode(WIFI_STA);
#endif

#ifdef ESP8266
  WiFi.hostname(Prefs.HostName);
#else
  WiFi.setHostname(Prefs.HostName);
//  WiFi.setSleep(true);
#endif

  delay(100);

  WiFiConnect();

  /**** Access point mode ****/

  if (!WiFiConnected) {
    IPAddress    ApIP(192, 168, 1, 1);
 
    if (Prefs.Debug) 
      Serial.print(F("\n[DEBUG] Switching to AP mode\n"));
    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(ApIP, ApIP, IPAddress(255, 255, 255, 0));
    
    if (WiFi.softAP(Prefs.HostName)) {

      /**** DNS server ****/

      DnsServer = new DNSServer();
      DnsServer -> setTTL(300);
      DnsServer -> setErrorReplyCode(DNSReplyCode::ServerFailure);
      DnsServer -> start(53, Prefs.HostName, ApIP);
    
      WiFiConnected = TR_WIFI_CONN_AP;

      if (Prefs.Debug)
        Serial.printf_P(PSTR("\n[DEBUG] WiFi in AP mode, IP address: %s\n"),
                        WiFi.softAPIP().toString().c_str());
      delay(500);
    } else {
      if (Prefs.Debug) 
        Serial.print(F("\n[DEBUG] WiFi not available\n"));
    }
  }
}
