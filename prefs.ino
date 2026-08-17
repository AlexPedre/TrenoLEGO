
/**************************************
****    TrenoLEGO - Preferences    ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Errors ****/

static const char   PrefsErrJson[] PROGMEM = "[DEBUG] ERROR: Prefs JSON parse failed: %s\r\n";


/**** Constructor ****/

TrPrefs::TrPrefs(void)
{
  Load(true);  
}


/**** Tranfert the preferences to a JSON doc ****/

void TrPrefs::FillDoc(JsonDocument &Doc)
{
  Doc[FPSTR(StrKeyVersion           )] = TR_PREFS_VER;
  Doc[FPSTR(StrKeyDebug             )] = Debug;
  Doc[FPSTR(StrKeyLang              )] = (const char *)LangStr[LangID];
  Doc[FPSTR(StrKeyWiFiSSID          )] = (const char *)WiFiSSID;
  Doc[FPSTR(StrKeyWiFiPass          )] = (const char *)WiFiPass;
  Doc[FPSTR(StrKeyAdminUser         )] = (const char *)AdminUser;
  Doc[FPSTR(StrKeyAdminPass         )] = (const char *)AdminPass;
  Doc[FPSTR(StrKeyFtpEnabled        )] = FtpEnabled;
  Doc[FPSTR(StrKeyHostName          )] = (const char *)HostName;
  Doc[FPSTR(StrKeyHueEnabled        )] = HueEnabled;

#ifdef TR_HAS_BLE
  Doc[FPSTR(StrKeyLegoHubAvailable  )] = true;
  Doc[FPSTR(StrKeyLegoHubEnabled    )] = LegoHubEnabled;
  Doc[FPSTR(StrKeyLegoHubPort       )] = GetLegoHubPort();
  Doc[FPSTR(StrKeyLegoHubConnChannel)] = LegoHubConnChannel;
#else
  Doc[FPSTR(StrKeyLegoHubAvailable  )] = false;
#endif

  Doc[FPSTR(StrKeyNtpServer         )] = (const char *)NtpServer;
  Doc[FPSTR(StrKeyTimeZone          )] = (const char *)TimeZone;
 
  Doc[FPSTR(StrKeyMqttAvailRefresh  )] = MqttAvailRefresh;
  Doc[FPSTR(StrKeyMqttEnabled       )] = MqttEnabled;
  Doc[FPSTR(StrKeyMqttServer        )] = (const char *)MqttServer;
  Doc[FPSTR(StrKeyMqttPort          )] = MqttPort;
  Doc[FPSTR(StrKeyMqttUser          )] = (const char *)MqttUser;
  Doc[FPSTR(StrKeyMqttPass          )] = (const char *)MqttPass;

  Doc[FPSTR(StrKeyPwmDeband         )] = PwmDeband;
  Doc[FPSTR(StrKeyPwmMax            )] = PwmMax;
  Doc[FPSTR(StrKeyPwmMin            )] = PwmMin;
  Doc[FPSTR(StrKeySleepEnabled      )] = SleepEnabled;
  Doc[FPSTR(StrKeySleepTime         )] = SleepTime;
  Doc[FPSTR(StrKeySpeechEnabled     )] = SpeechEnabled;
  Doc[FPSTR(StrKeyTelnetEnabled     )] = TelnetEnabled;
  Doc[FPSTR(StrKeyVibrationEnabled  )] = VibrEnabled;

  for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel) {
    char Key[32];

    snprintf_P(Key, sizeof(Key), StrKeyTrainAccelAmount, (unsigned int)Channel);
    Doc[Key] = PfTrains.GetAccelAmount(Channel);

    snprintf_P(Key, sizeof(Key), StrKeyTrainAccelStepTime, (unsigned int)Channel);
    Doc[Key] = PfTrains.GetAccelStepTime(Channel);

    snprintf_P(Key, sizeof(Key), StrKeyTrainEnabled, (unsigned int)Channel);
    Doc[Key] = PfTrains.GetEnabled(Channel);

    snprintf_P(Key, sizeof(Key), StrKeyTrainInvert, (unsigned int)Channel);
    Doc[Key] = PfTrains.GetInvert(Channel);

    snprintf_P(Key, sizeof(Key), StrKeyTrainName, (unsigned int)Channel);
    Doc[Key] = PfTrains.GetName(Channel);

    snprintf_P(Key, sizeof(Key), StrKeyTrainNameRev, (unsigned int)Channel);
    Doc[Key] = PfTrains.GetNameReverse(Channel);

    if (Channel >= TR_PF_CHANNEL_FIRSTPF) {
      snprintf_P(Key, sizeof(Key), StrKeyTrainPort, (unsigned int)Channel);
      Doc[Key] = PfTrains.GetPortStr(Channel);
    }
  } /* End of for (Channel) */
}


/**** Prefs from json string ****/

bool TrPrefs::FromJson(const char *JsonStr)
{
  if ((!JsonStr) || (!*JsonStr)) return false;

  if (Prefs.Debug) 
    Printf(F("[DEBUG] Json string:\r\n%s\r\n"), JsonStr);

  JsonDocument          Doc;
  DeserializationError  Err = deserializeJson(Doc, JsonStr);

  if (Err) {
    if (Prefs.Debug)
      Printf(FPSTR(PrefsErrJson), Err.c_str());
    return false;
  }

  MapFromDoc(Doc);

  return true;
}

  
/**** Prefs from json file stream ****/

bool TrPrefs::FromJson(Stream &Source)
{
  JsonDocument            Doc; 
  DeserializationError    Err = deserializeJson(Doc, Source);
 
  if (Err) {
    if (Prefs.Debug)
      Printf(FPSTR(PrefsErrJson), Err.c_str());
    return false;
  }

  MapFromDoc(Doc);

  return true;
}


/**** Get the language ID ****/

int TrPrefs::GetLangID(const char *Str)
{ 
  if ((Str) && (*Str != 0)) {
    for(int ID = 0; LangStr[ID]; ++ID) {
      if (!strcasecmp(Str, LangStr[ID])) return ID;
    } /* End of for (LangID) */
  }
  
  return -1;  
}

/**** Pop Json key ****/

template <typename T, typename K>

bool TrPrefs::JsonPop(T &Val, JsonDocument &Json, K *Key)
{
  JsonVariant     v = Json[Key];
  
  if (!v.isNull()) {
    Val = v.as<T>();
    return true;
  }

  return false;
}


/**** Pop Json key to char ****/

template <typename K>

bool TrPrefs::JsonPop(char *Val, JsonDocument &Json, K *Key, size_t ValSize)
{
  JsonVariant   v = Json[Key];
  
  if (v.is<const char *>()) {
    strlcpy(Val, v.as<const char *>(), ValSize);
    return true;
  }

  return false;
}


/**** Load the preferences ****/

void TrPrefs::Load(bool Default)
{
  if (Default) {
    Debug            = false;
    FtpEnabled       = true;
    HueEnabled       = false;
    LangID           = TR_LANG_DEFAULT;

#ifdef TR_HAS_BLE
    LegoHubEnabled     = false;
    LegoHubConnChannel = TR_PF_CHANNEL_FIRST;
    LegoHubPort        = PoweredUpHubPort::B;   
#endif

    MqttAvailRefresh = TR_MQTT_AVAIL_REFRESH;
    MqttEnabled      = false;
    MqttPort         = TR_MQTT_PORT;
    PwmDeband        = TR_PWM_DEBAND;
    PwmMax           = TR_PWM_MAX;
    PwmMin           = TR_PWM_MIN;
    SleepEnabled     = true;
    SleepTime        = TR_SLEEP_TIME;
    SpeechEnabled    = true;
    TelnetEnabled    = false;
    Version          = TR_PREFS_VER;
    VibrEnabled      = true;
 
    strlcpy_P(AdminUser , PSTR(TR_ADMIN_USER    ), sizeof(AdminUser ));
    strlcpy_P(AdminPass , PSTR(TR_ADMIN_PASS    ), sizeof(AdminPass ));
    strlcpy_P(HostName  , PSTR(TR_HOSTNAME      ), sizeof(HostName  ));
    strlcpy_P(NtpServer , PSTR(TR_NTP_SERVER    ), sizeof(NtpServer ));
    strlcpy_P(MqttServer, PSTR(TR_MQTT_SERVER   ), sizeof(MqttServer));
    strlcpy_P(MqttTopic , PSTR(TR_MQTT_BASETOPIC), sizeof(MqttTopic));
    strlcpy_P(MqttUser  , PSTR(TR_MQTT_USER     ), sizeof(MqttUser  ));
    strlcpy_P(MqttPass  , PSTR(TR_MQTT_PASS     ), sizeof(MqttPass  ));
    strlcpy_P(TimeZone  , PSTR(TR_TIMEZONE      ), sizeof(TimeZone  ));
    strlcpy_P(WiFiPass  , PSTR(TR_WIFI_PASS     ), sizeof(WiFiPass  ));
    strlcpy_P(WiFiSSID  , PSTR(TR_WIFI_SSID     ), sizeof(WiFiSSID  ));

    PfTrains.Default();

    return;
  }

  /**** Load from file ****/

  File Fh = LittleFS.open(FPSTR(PrefsFileName), "r");

  if ((!Fh) || (Fh.size() == 0)) {
    if (Fh) Fh.close();
    Fh = LittleFS.open(FPSTR(PrefsFileBak), "r");
  }

  if (Fh) {
    if ((!FromJson(Fh)) && (Prefs.Debug))
      Printf(F("ERROR: Invalid preference file\r\n"));
    Fh.close();
  }
}


/**** Map the Prefs from json ****/

void TrPrefs::MapFromDoc(JsonDocument &Json)
{
  char    StrKey[64];

  JsonPop(Debug             , Json, FPSTR(StrKeyDebug             ));
  JsonPop(FtpEnabled        , Json, FPSTR(StrKeyFtpEnabled        ));
  
  JsonPop(HueEnabled        , Json, FPSTR(StrKeyHueEnabled        ));

#ifdef TR_HAS_BLE
  JsonPop(LegoHubEnabled    , Json, FPSTR(StrKeyLegoHubEnabled    ));
  JsonPop(LegoHubConnChannel, Json, FPSTR(StrKeyLegoHubConnChannel));
#endif

  JsonPop(MqttAvailRefresh  , Json, FPSTR(StrKeyMqttAvailRefresh  ));
  JsonPop(MqttEnabled       , Json, FPSTR(StrKeyMqttEnabled       ));
  JsonPop(MqttPort          , Json, FPSTR(StrKeyMqttPort          ));

  JsonPop(PwmDeband         , Json, FPSTR(StrKeyPwmDeband         ));
  JsonPop(PwmMax            , Json, FPSTR(StrKeyPwmMax            ));
  JsonPop(PwmMin            , Json, FPSTR(StrKeyPwmMin            ));

  JsonPop(SleepEnabled      , Json, FPSTR(StrKeySleepEnabled      ));
  JsonPop(SleepTime         , Json, FPSTR(StrKeySleepTime         ));

  JsonPop(SpeechEnabled     , Json, FPSTR(StrKeySpeechEnabled     ));

  JsonPop(TelnetEnabled     , Json, FPSTR(StrKeyTelnetEnabled     ));

  JsonPop(VibrEnabled      , Json, FPSTR(StrKeyVibrationEnabled   ));

  JsonPop(AdminUser        , Json, FPSTR(StrKeyAdminUser          ), sizeof(AdminUser ));
  JsonPop(AdminPass        , Json, FPSTR(StrKeyAdminPass          ), sizeof(AdminPass ));
 
  JsonPop(NtpServer        , Json, FPSTR(StrKeyNtpServer          ), sizeof(NtpServer ));
 
  JsonPop(MqttUser         , Json, FPSTR(StrKeyMqttUser           ), sizeof(MqttUser  ));
  JsonPop(MqttPass         , Json, FPSTR(StrKeyMqttPass           ), sizeof(MqttPass  ));
  JsonPop(TimeZone         , Json, FPSTR(StrKeyTimeZone           ), sizeof(TimeZone  ));
  JsonPop(WiFiPass         , Json, FPSTR(StrKeyWiFiPass           ), sizeof(WiFiPass  ));
  JsonPop(WiFiSSID         , Json, FPSTR(StrKeyWiFiSSID           ), sizeof(WiFiSSID  ));

#ifdef TR_HAS_BLE
  if (JsonPop(StrKey, Json, FPSTR(StrKeyLegoHubPort), sizeof(StrKey)))
    SetLegoHubPort(StrKey);
#endif

  if (JsonPop(StrKey, Json, FPSTR(StrKeyHostName), sizeof(HostName)))
    SetHostName(StrKey);

  if (JsonPop(StrKey, Json, FPSTR(StrKeyMqttServer), sizeof(MqttServer)))
    SetMqttServer(StrKey);  

  for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel) {
    bool            ValBool;
    int             ValInt;
    String          Val;
    unsigned long   ValUlong;

    snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainAccelAmount, (unsigned int)Channel);
    if (JsonPop(ValInt, Json, StrKey))
      PfTrains.SetAccelAmount(Channel, ValInt);

    snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainAccelStepTime, (unsigned int)Channel);
    if (JsonPop(ValUlong, Json, StrKey))
      PfTrains.SetAccelStepTime(Channel, ValUlong);

    snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainEnabled, (unsigned int)Channel);
    if (JsonPop(ValInt, Json, StrKey))
      PfTrains.Enable(Channel, ValInt);
 
    snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainInvert, (unsigned int)Channel);
    if (JsonPop(ValBool, Json, StrKey))
      PfTrains.SetInvert(Channel, ValBool);
 
    snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainName, (unsigned int)Channel);
    if (JsonPop(Val, Json, StrKey))
      PfTrains.SetName(Channel, Val.c_str());
    
    snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainNameRev, (unsigned int)Channel);
    if (JsonPop(Val, Json, StrKey))
      PfTrains.SetNameReverse(Channel, Val.c_str());

    if (Channel >= TR_PF_CHANNEL_FIRSTPF) {
      snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainPort, (unsigned int)Channel);
      if (JsonPop(Val, Json, StrKey))
        PfTrains.SetPort(Channel, Val.c_str()); 
    }
 } /* End of for (Channel) */

  /**** Language ID ****/

  int ID = GetLangID(Json[FPSTR(StrKeyLang)]);
  if (ID != -1) LangID = ID;

  /**** Save the settings ****/

  if (Json["Save"] == true) {
    if (Prefs.Debug) 
      Print(F("[DEBUG] Saving the posted parameters\r\n"));
    Prefs.Save();
  }
}


/**** Save the preferences ****/

bool TrPrefs::Save(void)
{
  File Fh = LittleFS.open(FPSTR(PrefsFileTmp), "w");
  
  if (!Fh) {
    Printf(F("ERROR: Can't write %S\r\n"), FPSTR(PrefsFileTmp));
    return false;
  }

  SaveToStream(Fh, false);
  Fh.close();
  
  /**** Rotate the backup ****/

  if (LittleFS.exists(FPSTR(PrefsFileName))) {
    LittleFS.remove(FPSTR(PrefsFileBak)); 
    LittleFS.rename(FPSTR(PrefsFileName), FPSTR(PrefsFileBak));
  }

  /**** Rename the saved file ****/

  if (LittleFS.rename(FPSTR(PrefsFileTmp), FPSTR(PrefsFileName)))
    return true;

  /**** Fallback ****/
 
  LittleFS.rename(FPSTR(PrefsFileBak), FPSTR(PrefsFileName));

  return false;
}


/**** Save to stream ****/

void TrPrefs::SaveToStream(class Print &Out, bool Compact)
{
  JsonDocument   Doc;

  FillDoc(Doc);

  if (Compact) serializeJson(Doc, Out);
  else serializeJsonPretty(Doc, Out);
}


/**** Set the host name ****/

bool TrPrefs::SetHostName(const char *Name)
{
  if (!IsHostName(Name, sizeof(HostName) - 1))
    return false;

  strcpy(HostName, Name);

  return true;
}


/**** Set the MQTT server ****/

bool TrPrefs::SetMqttServer(const char *Name)
{
  if ((!IsDomainName(Name, sizeof(MqttServer) - 1)) &&
      (!IsIP(Name)))
    return false;

  strcpy(MqttServer, Name);

  return true;
}


/**** Show the preference file ****/

void TrPrefs::Show(void)
{
  File Fh = LittleFS.open(FPSTR(PrefsFileName), "r");

  if (!Fh) {
    Fh = LittleFS.open(FPSTR(PrefsFileBak), "r");
    if (!Fh) {
      Print(F("ERROR: No preference file to show\r\n"));
      return;
    }
    Print(F("--- Showing Backup File ---\r\n"));
  } else {
    Print(F("--- Showing Main Config ---\r\n"));
  }

  char Buffer[128];
  
  while(Fh.available()) {
    int BytesRead = Fh.readBytes(Buffer, sizeof(Buffer) - 1);
    Buffer[BytesRead] = '\0';
    Print(Buffer);
    yield();
  } /* End of while */
  Fh.close();
  
  Print(F("\r\n--- End of File ---\r\n"));
}
