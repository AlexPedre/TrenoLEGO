
/**************************************
****    TrenoLEGO - CLI commands   ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Loacal variables ****/

static const char MsgErrBool[]       PROGMEM = "Invalid boolean value";
static const char MsgErrByte[]       PROGMEM = "The value must be in 0...255 range";
static const char MsgErrInt[]        PROGMEM = "Invalid integer value";
static const char MsgErrInvChannel[] PROGMEM = "ERROR: Invalid channel";
static const char MsgErrTooLong[]    PROGMEM = "ERROR: Argument too long";


/**** Format disk ****/

void CmdFormat(MyCommandParser::Argument *Args, char *Response)
{
  if (strcmp(Args[0].asString, "CONFIRM")) {
    strlcpy_P(Response, PSTR("ERROR: Type CONFIRM as argument"), MyCommandParser::MAX_RESPONSE_SIZE);
    return;
  }

  LittleFS.end();
  if (LittleFS.format()) {
#ifdef ESP32 
    if (LittleFS.begin(false)) {
#elif defined(ESP8266)
    if (LittleFS.begin()) {
#endif
      Prefs.Save();
      strlcpy_P(Response, PSTR("Done"), MyCommandParser::MAX_RESPONSE_SIZE);
    } else {
      strlcpy_P(Response, PSTR("ERROR: Can't mount the disk"), MyCommandParser::MAX_RESPONSE_SIZE); 
    }
  } else {
    strlcpy_P(Response, PSTR("ERROR: Format failed"), MyCommandParser::MAX_RESPONSE_SIZE);
  }
}


/**** Help ****/

void CmdHelp(MyCommandParser::Argument *Args, char *Response)
{
  Print(F("\r\nList of commands:\r\n"
          "format CONFIRM          -> Format disk\r\n"
          "help                    -> Show this help\r\n"
          "load WATH               -> Load preferences (default, saved)\r\n"
          "motor CHANNEL SPEED     -> Change motor SPEED (-255...255) of the CHANNEL (0-4)\r\n"
          "reboot                  -> Reboot the system\r\n"
          "save                    -> Save the settings to flash ram\r\n"
         ));
        
  Print(F("set PARAMETER       VALUE -> Set a parameter:\r\n"
          "    adminuser       STR   -> Administrator user name\r\n"
          "    adminpass       STR   -> Administrator password\r\n"
          "    debug           BOOL  -> Enable/disable the debug mode\r\n"
          "    ftp             BOOL  -> Enable/disable the ftp server\r\n"
          "    hostname        STR   -> Change the host name\r\n"
          "    hue             BOOL  -> Enable/disable the hue emulation\r\n"
          "    lang            STR   -> System language (en, it)\r\n"
         ));

#ifdef TR_HAS_BLE
 Print(F("    legohub         BOOL  -> Enable/disable the LEGO hub emulation\r\n"
         "    legohubport     STR   -> Emulated port (A or B)\r\n"
         "    legohubconch    INT   -> Connected channel/train\r\n"
        ));
#endif

 
  Print(F("    mqtt            BOOL  -> Enable/disable the MQTT service\r\n"
          "    mqttavail       INT   -> MQTT avail refresh time (ms)\r\n"
          "    mqttserver      STR   -> MQTT remote server\r\n"
          "    mqttuser        STR   -> MQTT user name\r\n"
          "    mqttport        INT   -> MQTT server port (1...65535)\r\n"
          "    mqttpwd         STR   -> MQTT password\r\n"
          "    ntpserver       STR   -> NTP server\r\n"
          "    pwmdeband       INT   -> PWM deband (0...255)\r\n"
          "    pwmmax          INT   -> PWM max value (0...255)\r\n"
          "    pwmmin          INT   -> PWM min value (0...255)\r\n"
         ));
 
  Print(F("    sleep           BOOL  -> Enable/disable the suspension\r\n"
          "    sleeptime       INT   -> Inactivity time in sec. for the suspension\r\n"
          "    speech          BOOL  -> Enable/disable the speech synthesis\r\n"
          "    ssid            STR   -> WiFi SSID\r\n"
          "    telnet          BOOL  -> Enable/disable the telnet server\r\n"
          "    timezone        STR   -> Time zone\r\n"
          "    vibration       BOOL  -> Control device vibration\r\n"
          "    wifipwd         STR   -> WiFi password\r\n"
          "showprefs                 -> Show the preference file\r\n"
          "sleep                     -> Suspend the system immediately\r\n"
          "status                    -> Show the status\r\n"
         ));
  
  Print(F("tset CHANNEL KEY VAL      -> Set a train/channel (0-4) parameter\r\n"
          "    accelamount     INT   -> Motor acceleration amount\r\n"
          "    accelsteptime   INT   -> Motor acceleration step time\r\n" 
          "    enable          BOOL  -> Enable/disable the train\r\n"
          "    invert          BOOL  -> Invert motor rotation\n"
          "    name            STR   -> Set the train name\r\n"
          "    namerev         STR   -> Set the train name for reverse (for Alexa)\r\n"
          "    port            STR   -> Set the port used for the motor (red/blue)\r\n"
        ));
}


/**** Load the preferences ****/

void CmdLoad(MyCommandParser::Argument *Args, char *Response)
{
  const char *    What = Args[0].asString;

       if (!strcmp_P(What, PSTR("default"))) Prefs.Load(true);
  else if (!strcmp_P(What, PSTR("saved"  ))) Prefs.Load(false);
  else strlcpy_P(Response, PSTR("ERROR: Invalid argument"), MyCommandParser::MAX_RESPONSE_SIZE);
}


/**** Run the motor ****/

void CmdMotor(MyCommandParser::Argument *Args, char *Response)
{
  uint8_t     Channel = (uint8_t)Args[0].asUInt64;

  if (Channel < TR_PF_CHANNEL_NUM) {
   if (!PfTrains.MotorSpeedAsync(Channel, atoi(Args[1].asString)))
     strlcpy_P(Response, PSTR("ERROR: Channel disabled"), MyCommandParser::MAX_RESPONSE_SIZE);
  } else strlcpy_P(Response, MsgErrInvChannel, MyCommandParser::MAX_RESPONSE_SIZE);
}


/**** Reboot ****/

void CmdReboot(MyCommandParser::Argument *Args, char *Response)
{
  Reboot();
}


/**** Save settings ****/

void CmdSave(MyCommandParser::Argument *Args, char *Response)
{
  if (Prefs.Save())
    snprintf_P(Response, MyCommandParser::MAX_RESPONSE_SIZE, PSTR("Saved to %S"), FPSTR(PrefsFileName));
}


/**** Set a parameter ****/

void CmdSet(MyCommandParser::Argument *Args, char *Response)
{
  const char *          MsgErr               = NULL;
  const char *          Val                  = Args[1].asString;
  const char *          Var                  = Args[0].asString;
  int                   ValB                 = ChkTrue(Val);
  int                   ValI                 = atoi(Val);
  size_t                ValLen               = strlen(Val);


  /**** Administrator credentials ****/
  
  if (!strcasecmp_P(Var, StrKeyAdminPass)) {
    strlcpy(Prefs.AdminPass, Val, sizeof(Prefs.AdminPass));
  } else if (!strcasecmp_P(Var, StrKeyAdminUser)) {
    strlcpy(Prefs.AdminUser, Val, sizeof(Prefs.AdminUser));

  /**** Debug ****/
  
  } else if (!strcasecmp_P(Var, StrKeyDebug)) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.Debug = ValB;

  /**** FTP server ****/

  } else if (!strcasecmp_P(Var, PSTR("Ftp"))) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.FtpEnabled = ValB;

  /**** Host name ****/

  } else if (!strcasecmp_P(Var, StrKeyHostName)) {
    if (!Prefs.SetHostName(Val))
      MsgErr = PSTR("Invalid host name");
 
  /**** Hue emulation for Alexa ****/

  } else if (!strcasecmp_P(Var, PSTR("Hue"))) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.HueEnabled = ValB;

  /**** LEGO hub ****/

#ifdef TR_HAS_BLE
  } else if (!strcasecmp_P(Var, PSTR("LegoHub"))) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.LegoHubEnabled = ValB;
  } else if (!strcasecmp_P(Var, PSTR("LegoHubPort"))) {
    if (!Prefs.SetLegoHubPort(Val))
      MsgErr = PSTR("Invalid port (A or B)");
  } else if (!strcasecmp_P(Var, PSTR("LegoHubConCh"))) {
    if ((ValI >= TR_PF_CHANNEL_FIRST) && (ValI < TR_PF_CHANNEL_NUM)) {
      Prefs.LegoHubConnChannel = (uint8_t)ValI;
    } else strlcpy_P(Response, MsgErrInvChannel, MyCommandParser::MAX_RESPONSE_SIZE);
#endif

  /**** Language ****/

  } else if (!strcasecmp_P(Var, StrKeyLang)) {
    int LangID = Prefs.GetLangID(Val);
    if (LangID == -1) MsgErr = PSTR("Invalid language");
    else Prefs.LangID = LangID;

  /**** MQTT server ****/

  } else if (!strcasecmp_P(Var, PSTR("MqttAvail"))) {
    if (ValI < 1000) MsgErr = PSTR("MQTT avail time must be > 1000");
    else Prefs.MqttAvailRefresh = ValI;
  } else if (!strcasecmp_P(Var, PSTR("Mqtt"))) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.MqttEnabled = ValB;
  } else if (!strcasecmp_P(Var, StrKeyMqttPort)) {
    if ((ValI > 0) && (ValI < 65535)) Prefs.MqttPort = ValI;
    else MsgErr = PSTR("Invalid MQTT port");
  } else if (!strcasecmp_P(Var, PSTR("MqttPwd"))) {
    strlcpy(Prefs.MqttPass, Val, sizeof(Prefs.MqttPass));
  } else if (!strcasecmp_P(Var, StrKeyMqttServer)) {
    if (!Prefs.SetMqttServer(Val))
      MsgErr = PSTR("Invalid MQTT server");
  } else if (!strcasecmp_P(Var, StrKeyMqttUser)) {
    strlcpy(Prefs.MqttUser, Val, sizeof(Prefs.MqttUser));

  /**** NTP ****/

  } else if (!strcasecmp_P(Var, StrKeyNtpServer)) {
    strlcpy(Prefs.NtpServer, Val, sizeof(Prefs.NtpServer));
  } else if (!strcasecmp_P(Var, StrKeyTimeZone)) {
    strlcpy(Prefs.TimeZone, Val, sizeof(Prefs.TimeZone));

  /**** Sleep/suspension ****/

  } else if (!strcasecmp_P(Var, PSTR("Sleep"))) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.SleepEnabled = ValB;
  } else if (!strcasecmp_P(Var, StrKeySleepTime)) {
    if (ValI < 30) MsgErr = PSTR("The sleep time must be >= 30 sec.");
    else Prefs.SleepTime = ValI;
  } else if (!strcasecmp_P(Var, PSTR("Ssid"))) {
    if ((ValLen < 8 ) || (ValLen > 32)) MsgErr = PSTR("Invalid SSID length");
    else strlcpy(Prefs.WiFiSSID, Val, sizeof(Prefs.WiFiSSID));

  /**** Speech synthesis ****/

  } else if (!strcasecmp_P(Var, PSTR("Speech"))) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.SpeechEnabled = ValB;   

  /**** PWM ****/

  } else if (!strcasecmp_P(Var, StrKeyPwmDeband)) {
    if ((ValI >= 0) && (ValI <= 255)) Prefs.PwmDeband = ValI;
    else MsgErr = MsgErrByte;
  } else if (!strcasecmp_P(Var, StrKeyPwmMax)) {
    if ((ValI >= 0) && (ValI <= 255)) Prefs.PwmMax = ValI;
    else MsgErr = MsgErrByte;
  } else if (!strcasecmp_P(Var, StrKeyPwmMin)) {
    if ((ValI >= 0) && (ValI <= 255)) Prefs.PwmMin = ValI;
    else MsgErr = MsgErrByte;

  /**** Telnet ****/

  } else if (!strcasecmp_P(Var, PSTR("Telnet"))) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.TelnetEnabled = ValB;

  /**** Vibration ****/

  } else if (!strcasecmp_P(Var, PSTR("Vibration"))) {
    if (ValB == -1) MsgErr = MsgErrBool;
    else Prefs.VibrEnabled = ValB;   
  
  /**** WiFi ****/

  } else if (!strcasecmp_P(Var, PSTR("WifiPwd"))) {
    if ((ValLen < 8) || (ValLen > 63)) MsgErr = PSTR("Invalid password length");
    else strlcpy(Prefs.WiFiPass, Val, sizeof(Prefs.WiFiPass));
  } else MsgErr = PSTR("Unknown parameter");

  if (MsgErr)
    snprintf_P(Response, MyCommandParser::MAX_RESPONSE_SIZE, PSTR("ERROR: %S"), MsgErr);
}


/**** Go to deep sleep ****/

void CmdSleep(MyCommandParser::Argument *Args, char *Response)
{
  Print(F("Sleeping...\r\n"));
  Suspend();
}


/**** Show the preference file ****/

void CmdShowPrefs(MyCommandParser::Argument *Args, char *Response)
{
  Prefs.Show();
}


/**** Status ****/

void CmdStatus(MyCommandParser::Argument *Args, char *Response)
{
  time_t      Now;
  tm          Tm;
  time(&Now);
  localtime_r(&Now, &Tm);

  /**** Main ****/

  Printf(F("\r\n"
           "Main:\r\n"
           "  Version: " TR_VERSION "\r\n"
           "  Debug: %s\r\n"
           "  System language: %s\r\n"
           "  Speech synthesis: %s\r\n"
           "  Vibration: %s\r\n"
           "  Sleep enabled: %s\r\n"
           "  Sleep time: %lu sec.\r\n"
#ifdef TR_HAS_BLE
           "  Switch: %d\r\n"
#endif
         ), Bool2Str(Prefs.Debug),
         Prefs.GetLangStr(Prefs.LangID),
         Bool2Str(Prefs.SpeechEnabled),
         Bool2Str(Prefs.VibrEnabled),
         Bool2Str(Prefs.SleepEnabled),
         Prefs.SleepTime
#ifdef TR_HAS_BLE
         , digitalRead(TR_PIN_SWITCH)
#endif
         );

  /**** Administrator credentials ****/

  Printf(F("\r\n"
           "Administrator credentials:\r\n"
           "  User name: %s\r\n"
           "  Password: %s\r\n"),
         Prefs.AdminUser,
         Prefs.AdminPass);

  /**** Time & date ****/

  Printf(F("\r\n"
           "Time & date:\r\n"
           "  NTP server: %s\r\n"
           "  Time zone: %s\r\n"
           "  Time: %02d:%02d:%02d\r\n"
           "  Date: %02d/%02d/%d\r\n"),
         Prefs.NtpServer,
         Prefs.TimeZone,
         Tm.tm_hour, Tm.tm_min, Tm.tm_sec,
         Tm.tm_mday, Tm.tm_mon + 1, Tm.tm_year + 1900);

  /**** WiFi ****/

  String        StrIp;
  const char *  StrMode;

  if (WiFiConnected == TR_WIFI_CONN_AP)  {
    StrIp   = WiFi.softAPIP().toString();
    StrMode = "Access point";
  } else {
    StrIp   = WiFi.localIP().toString();
    StrMode = "Station";
  }

  Printf(F("\r\n"
           "WiFi network:\r\n"
           "  Connected: %s\r\n"
           "  Mode: %s\r\n"
           "  Host name: %s\r\n"
           "  IP: %s\r\n"
           "  RSSI: %d dBm\r\n"),
         Bool2Str(WiFiConnected != TR_WIFI_CONN_NONE),
         StrMode,
         Prefs.HostName,
         StrIp.c_str(),
         WiFi.RSSI());

  if (WiFiConnected == TR_WIFI_CONN_STA) {
    Printf(F("  SSID: %s\r\n"
             "  Password: %s\r\n"),
           Prefs.WiFiSSID,
           Prefs.WiFiPass);
  }

  /**** Services ****/

  Printf(F("\r\n"
           "Services:\r\n"
           "  FTP enabled: %s (active: %s)\r\n"
           "  Telnet enabled: %s (active: %s)\r\n"
           "  Hue/Alexa enabled: %s (active %s)\r\n"),
         Bool2Str(Prefs.FtpEnabled), Bool2Str(FtpEnabled),
         Bool2Str(Prefs.TelnetEnabled), Bool2Str(TelnetEnabled),
         Bool2Str(Prefs.HueEnabled), Bool2Str(HueEnabled));
 
  for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel) {
    Printf(F("    Train %u: %s\r\n"
             "    Train %u reverse: %s\r\n"),
           (unsigned int)Channel, PfTrains.GetName(Channel),
           (unsigned int)Channel, PfTrains.GetNameReverse(Channel)); 
  } /* End of for (Channel) */

#ifdef TR_HAS_BLE
  Printf(F("\r\n"
           "LEGO Powered Up:\r\n"
           "  Enabled: %s\r\n"
           "  Emulated port: %s\r\n"
           "  Connected channel/train: %u\r\n"),
         Bool2Str(Prefs.LegoHubEnabled),
         Prefs.GetLegoHubPort(),
         Prefs.LegoHubConnChannel);
#endif

  /**** MQTT ****/

  Printf(F("\r\n"
           "MQTT:\r\n"
           "  Enablded: %s (active: %s)\r\n"
           "  Server: %s\r\n"
           "  Port: %d\r\n"
           "  User name: %s\r\n"
           "  Password: %s\r\n"
           "  Client ID: %s\r\n"
           "  Connected: %s\r\n"
           "  Avail refresh: %lu ms\r\n"),
         Bool2Str(Prefs.MqttEnabled), Bool2Str(MqttEnabled),
         Prefs.MqttServer,
         Prefs.MqttPort,
         Prefs.MqttUser,
         Prefs.MqttPass,
         MqttClientID,
         Bool2Str((MqttClient) && (MqttClient -> connected())),
         Prefs.MqttAvailRefresh);

  /**** System ****/

#ifdef ESP8266
  Printf(F("\r\n"
           "System:\r\n"
           "  Chip ID: %08X\r\n"
           "  Core version: %s\r\n"
           "  SDK version: %s\r\n"
           "  CPU clock: %d MHz\r\n"
           "  Sketch size: %.2f Kb\r\n"
           "  Free sketch space: %.2f Kb\r\n"
           "  Free heap: %d bytes\r\n"
           "  Heap fragmentation: %d%%\r\n"),
         ESP.getChipId(),
         ESP.getCoreVersion(),
         ESP.getSdkVersion(),
         ESP.getCpuFreqMHz(),
         (float)ESP.getSketchSize() / 1024.0f,
         (float)ESP.getFreeSketchSpace() / 1024.0f,
         ESP.getFreeHeap(),
         ESP.getHeapFragmentation());
#endif

#ifdef ESP32
  uint64_t Mac = ESP.getEfuseMac();
  uint32_t ChipId = (uint32_t)(Mac ^ (Mac >> 32));

  Printf("\r\n"
         "System:\r\n"
         "  Chip: %s\r\n"
         "  Chip rev.: %d\r\n"
         "  Cores: %d\r\n"
         "  Chip ID: %08X\r\n"
         "  SDK version: %s\r\n"
         "  CPU clock: %d MHz\r\n"
         "  CPU temperature: %.1f C\r\n",
         ESP.getChipModel(),
         ESP.getChipRevision(),
         ESP.getChipCores(),
         ChipId,
         ESP.getSdkVersion(),
         ESP.getCpuFreqMHz(),
         temperatureRead());
                  
  Printf("  Sketch size: %.2f Kb\r\n"
         "  Free heap: %d bytes\r\n"
         "  Min free heap: %d bytes\r\n"
         "  PSRAM: %s\r\n"
         "  PSRAM size: %d bytes\r\n",
         (float)ESP.getSketchSize() / 1024.0f,
         ESP.getFreeHeap(),
         ESP.getMinFreeHeap(),
         HasPSRAM ? "yes" : "no",
         (int)ESP.getPsramSize());
#endif

  /**** Internal storage ****/

#ifdef ESP8266
  FSInfo      FsInfo;
  LittleFS.info(FsInfo);

  Printf(F("\r\n"
           "Internal storage:\r\n"
           "  Size: %.2f Mb\r\n"
           "  Used: %.2f Mb\r\n"
           "  Free: %.2f %%\r\n"
           "  Block size: %d bytes\r\n"),
         (float)FsInfo.totalBytes / 1024.0f / 1024.0f,
         (float)FsInfo.usedBytes / 1024.0f / 1024.0f,
         (100.0f * (float)(FsInfo.totalBytes - FsInfo.usedBytes)) / (float)FsInfo.totalBytes,
         FsInfo.blockSize);
#endif

#ifdef ESP32
  size_t  TotalBytes = LittleFS.totalBytes();
  size_t  UsedBytes  = LittleFS.usedBytes();
  float   FreePct    = (TotalBytes > 0) ? (100.0f * (float)(TotalBytes - UsedBytes)) / (float)TotalBytes : 0.0f;

  Printf(F("\r\n"
           "Internal storage:\r\n"
           "  Size: %.2f Mb\r\n"
           "  Used: %.2f Mb\r\n"
           "  Free: %.2f %%\r\n"),
         (float)TotalBytes / 1024.0f / 1024.0f,
         (float)UsedBytes  / 1024.0f / 1024.0f,
         FreePct);
#endif

  /**** Motor & Pwm ****/

  Printf(F("\r\n"
           "Motor:\r\n"
           "  PWM deband: %d\r\n"
           "  PWM min: %d\r\n"
           "  PWM max: %d\r\n"),
         Prefs.PwmDeband,
         Prefs.PwmMin,
         Prefs.PwmMax);

  for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel)
    Printf(F("  Channel %u.%-4s: enabled %s speed %d/255 acc_amount %d/255 acc_steptime %lu ms\r\n"),
           (unsigned int)Channel, PfTrains.GetPortStr(Channel),
           Bool2Str(PfTrains.GetEnabled(Channel)), PfTrains.GetSpeed(Channel),
           PfTrains.GetAccelAmount(Channel), PfTrains.GetAccelStepTime(Channel));

  /**** Battery ****/

  Printf(F("\r\n"
           "Battery:\r\n"
           "  Voltage: %d mV\r\n"
           "  Remaining charge: %d%%\r\n"),
         BattMilliVolts,
         BattLevel);
}


/**** Set the train parameters ****/

void CmdTset(MyCommandParser::Argument *Args, char *Response)
{
  uint8_t     Channel = (uint8_t)Args[0].asUInt64;

  if (Channel < TR_PF_CHANNEL_NUM) {
    int                   k;

    const char *          KeyStr    = Args[1].asString;
    static const char *   KeyVect[] = { "Enable"     , "Invert"       , "Name", "NameRev", "Port", 
                                        "AccelAmount", "AccelStepTime", NULL };

    for(k = 0; KeyVect[k] && strcasecmp(KeyVect[k], KeyStr); ++k);

    const char *  ValStr  = Args[2].asString;
    int           ValInt  = atoi(ValStr);

    switch(k) {
    case 0:   /* Enable */
    case 1:   /* Invert */
      {
        int ValBool = ChkTrue(ValStr);
 
             if (ValBool == -1) strlcpy_P(Response, PSTR("ERROR: Invalid boolean value"), MyCommandParser::MAX_RESPONSE_SIZE);
        else if (k == 0) PfTrains.Enable(Channel, ValBool == 1);
             else PfTrains.SetInvert(Channel, ValBool);
      }
      break;

    case 2:   /* Name */
      if (strlen(ValStr) < sizeof(((TR_PF_TRAIN*)0) -> Name))
        PfTrains.SetName(Channel, ValStr);
      else
        strlcpy_P(Response, MsgErrTooLong, MyCommandParser::MAX_RESPONSE_SIZE);
      break;

    case 3:   /* NameRev */
      if (strlen(ValStr) < sizeof(((TR_PF_TRAIN*)0) -> NameReverse))
        PfTrains.SetNameReverse(Channel, ValStr);
      else
        strlcpy_P(Response, MsgErrTooLong, MyCommandParser::MAX_RESPONSE_SIZE);
      break;

    case 4:   /* Port */
      if (Channel >= TR_PF_CHANNEL_FIRSTPF) {
             if (!strcasecmp_P(ValStr, PSTR("Red" ))) PfTrains.SetPort(Channel, (uint8_t)TR_PF_PORT_RED );
        else if (!strcasecmp_P(ValStr, PSTR("Blue"))) PfTrains.SetPort(Channel, (uint8_t)TR_PF_PORT_BLUE);
        else strlcpy_P(Response, PSTR("ERROR: Invalid port"), MyCommandParser::MAX_RESPONSE_SIZE);
      } else strlcpy_P(Response, PSTR("ERROR: Channel must be >= 1"), MyCommandParser::MAX_RESPONSE_SIZE);
      break;

    case 5:   /* AccelAmount */
      if ((ValInt > 0) && (ValInt <= 255))
        PfTrains.SetAccelAmount(Channel, ValInt);
      else
        strlcpy_P(Response, MsgErrByte, MyCommandParser::MAX_RESPONSE_SIZE);
      break;

    case 6:   /* AccelStepTine */
      if ((ValInt > 0) && (ValInt <= 1000))
        PfTrains.SetAccelStepTime(Channel, ValInt);
      else
        strlcpy_P(Response, MsgErrInt, MyCommandParser::MAX_RESPONSE_SIZE);
      break;

    default:
      strlcpy_P(Response, PSTR("ERROR: Unknown parameter"), MyCommandParser::MAX_RESPONSE_SIZE);
      break;
    } /* End of switch */
  } else strlcpy_P(Response, MsgErrInvChannel, MyCommandParser::MAX_RESPONSE_SIZE);
}
