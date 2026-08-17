
/**************************************
****        TrenoLEGO - OTA        ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Arduino OTA setup ****/

void OtaSetup(void)
{
#ifdef ESP32
  ArduinoOTA.setPort(3232);
#else
  ArduinoOTA.setPort(8266);
#endif

  ArduinoOTA.setHostname(Prefs.HostName);
  ArduinoOTA.setPassword(Prefs.AdminPass);

  /**** Start event ****/

  ArduinoOTA.onStart([]() {

    if (Prefs.Debug) {
      Serial.printf_P(PSTR("Start updating from %S\n"),
                     (ArduinoOTA.getCommand() == U_FLASH) ? (PGM_P)PSTR("sketch") : (PGM_P)PSTR("filesystem"));
    }
    
    Prefs.SleepEnabled = false;
    OtaMode            = TR_OTA_ARDUINO;
  });

  /**** End event ****/
   
  ArduinoOTA.onEnd([]() {
    if (Prefs.Debug)
      Serial.println(F("\nEnd"));
    OtaMode = TR_OTA_NONE;
  });

  /**** Progress event ****/

  ArduinoOTA.onProgress([](unsigned int Progress, unsigned int Total) {
    if (!Prefs.Debug) return;
    
    static unsigned long  Last = 0;
    unsigned long         Now  = millis();
    
    if (Now - Last > 250) {
      Serial.printf_P(PSTR("OTA: %u%%\n"), (Progress * 100U) / Total);
      Last = Now;
    }
  });

  /**** Error event ****/

  ArduinoOTA.onError([](ota_error_t Error) {
    const char *          MsgErr;
    
    Serial.printf_P(PSTR("ERROR %u: "), Error);

    switch(Error) {
    case OTA_AUTH_ERROR:
      MsgErr = PSTR("Auth failed");
      break;

    case OTA_BEGIN_ERROR:
      MsgErr = PSTR("Begin failed");
      break;

    case OTA_CONNECT_ERROR:
      MsgErr = PSTR("Connect failed");
      break;
      
    case OTA_RECEIVE_ERROR:
      MsgErr = PSTR("Receive failed");
      break;      

    case OTA_END_ERROR:
      MsgErr = PSTR("End failed");
      break;

    default:
      MsgErr = PSTR("Unknown");
    } /* End of switch */
    
    Serial.println(FPSTR(MsgErr));

    OtaMode = TR_OTA_NONE;
  });

  ArduinoOTA.begin();
}
