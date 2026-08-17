
/**************************************
****    TrenoLEGO - MQTT client    ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** MQTT callback ****/

void MqttCallback(char *Topic, byte *Payload, unsigned int Length) 
{
  size_t              BaseLen = strlen(Prefs.MqttTopic);

  if (Prefs.Debug) { 
    Printf(F("[DEBUG] MQTT Message arrived %s "), Topic);
    
    for (int i = 0; i < Length; i++)
      Printf(F("%c"), Payload[i]);
    Print(F("\r\n"));
  }

  if ((!strncmp(Topic, Prefs.MqttTopic, BaseLen)) || (!strcmp(Topic + BaseLen, "/train/set")))
    return;

  JsonDocument          Doc;
  DeserializationError   Err = deserializeJson(Doc, Payload, Length);

  if (Err) {
    if (Prefs.Debug)
      Printf(F("[DEBUG] ERROR: MQTT JSON parse failed: %s\r\n"), Err.c_str());
    return;
  }


  JsonObject Obj = Doc.as<JsonObject>();
  
  for(JsonPair p : Obj) {
    const char *    Key = p.key().c_str();

    if (strncmp(Key, "motor", 5) == 0) {
      int Channel = atoi(Key + 5);

      if (((Channel >= TR_PF_CHANNEL_FIRST) && (Channel < TR_PF_CHANNEL_NUM)) &&
          (p.value().is<int>())) {
        PfTrains.MotorSpeedAsync(Channel, p.value().as<int>());
      } else if (Prefs.Debug) {
        Printf(F("[DEBUG] MQTT Error: Invalid channel %d\r\n"), Channel);
      }
    }
  } /* End of for (p) */

  LastCmdTime = millis();
}


/**** MQTT reconnect ****/

void MqttReconnect(void)
{
  static unsigned long  RetryAt  = 0;
  static int            Retries  = 0;

  /**** Not yet time to retry ****/

  if ((long)(millis() - RetryAt) < 0) return;

  /**** Too many retries: back off for 60 seconds ****/

  if (Retries >= TR_MQTT_RETRY) {
    if (Prefs.Debug)
      Print(F("[DEBUG] MQTT: Max retries reached, backing off 60s\r\n"));
      
    Retries = 0;
    RetryAt = millis() + 60000UL;
    return;
  }

  if (Prefs.Debug)
    Printf(F("[DEBUG] Connecting to %s:%d MQTT server (attempt %d/%d)\r\n"),
           Prefs.MqttServer, (int)Prefs.MqttPort, Retries + 1, TR_MQTT_RETRY);

  /**** Create a random client ID ****/

  snprintf(MqttClientID, sizeof(MqttClientID), "TrenoLegoMQTT-%x", random(0xffff));

  if (MqttClient -> connect(MqttClientID, Prefs.MqttUser, Prefs.MqttPass)) {
    Retries = 0;
    RetryAt = 0;

    if (Prefs.Debug) Print(F("[DEBUG] MQTT connected\r\n"));

    /**** Subscribe the topics ****/

    char MqttTopicSet[64];
    strcpy(MqttTopicSet, Prefs.MqttTopic);
    strcat(MqttTopicSet, "/train/set");
    MqttClient -> subscribe(MqttTopicSet);

    /**** Publish the state ****/

    MqttPubAvail(true);
    MqttPubTrainState();

  } else {

    /**** Failed: schedule next attempt in 5 seconds ****/

    if (Prefs.Debug)
      Printf(F("[DEBUG] ERROR: MQTT code %d, retry in 5s\r\n"), MqttClient -> state());
    ++Retries;
    RetryAt = millis() + 5000UL;
  }
}


/**** Publish the availability ****/

void MqttPubAvail(bool Online)
{
  char      Topic[64];

  snprintf_P(Topic, sizeof(Topic), PSTR("%s/availability"), Prefs.MqttTopic); 
  
  MqttClient -> publish(Topic, 
                       Online ? "{\"state\": \"online\"}" : 
                       "{\"state\": \"offline\"}");
  MqttAvailTime = millis();
}


/**** Publish the train state ****/

void MqttPubTrainState(void)
{
  if (!MqttClient -> connected()) return;

  JsonDocument  Doc;

  Doc[FPSTR("battery"    )] = BattLevel;
  Doc[FPSTR("battery_low")] = BattLow;
  Doc[FPSTR("rssi"       )] = WiFi.RSSI();
  Doc[FPSTR("ip"         )] = WiFi.localIP().toString();

  for(int i = TR_PF_CHANNEL_FIRST; i < TR_PF_CHANNEL_NUM; i++) {
    char    Key[8];
    
    snprintf(Key, sizeof(Key), "motor%d", i);
    Doc[Key] = PfTrains.GetSpeed(i);
  } /* End of for (i) */

  Doc[FPSTR("version")] = TR_VERSION;
  Doc[FPSTR("voltage")] = BattMilliVolts;

  char      Topic[64];

  snprintf_P(Topic, sizeof(Topic), PSTR("%s/train"), Prefs.MqttTopic);
  size_t    n       = measureJson(Doc);
  char *    Payload = (char *)Malloc(n + 1);

  if (Payload) {
    serializeJson(Doc, Payload, n + 1);
    
    if (Prefs.Debug)
      Printf(F("[DEBUG] MQTT publish %s %s\n"), Topic, Payload);

    MqttClient -> publish(Topic, Payload);
    free(Payload);
  }
}


/**** MQTT setup ****/

void MqttSetup(void)
{
  EspClient  = new WiFiClient();
  MqttClient = new PubSubClient(*EspClient);

  MqttClient -> setServer(Prefs.MqttServer, Prefs.MqttPort);
  MqttClient -> setCallback(MqttCallback);

  MqttReconnect();
}
