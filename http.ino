
/**************************************
****    TrenoLEGO - HTTP server    ****
**** (c) 2025, Alessandro Pedretti ****
**************************************/

/*
 * https://github.com/lbernstone/asyncUpdate/blob/master/AsyncUpdate.ino
 */


/**** Mime types ****/

static const char   MimeHtml[] PROGMEM = "text/html";
static const char   MimeJpeg[] PROGMEM = "image/jpeg";
static const char   MimeJs[]   PROGMEM = "text/javascript";
static const char   MimeJson[] PROGMEM = "application/json";
static const char   MimeText[] PROGMEM = "text/plain";

static const TR_MIME MimeTypes[] PROGMEM = {
  {".css" , "text/css"     },
  {".gif" , "image/gif"    },
  {".htm" , MimeHtml       },
  {".html", MimeHtml       },
  {".ico" , "image/x-icon" },
  {".mp3" , "audio/mpeg"   },
  {".jpeg", MimeJpeg       },
  {".jpg" , MimeJpeg       },
  {".js"  , MimeJs         },
  {".json", MimeJson       },
  {".mid" , "audio/midi"   },
  {".png" , "image/png"    },
  {".svg" , "image/svg+xml"},
  {".webp", "image/webp"   },
  {NULL         , MimeText }  
};

/**** Errors ****/

static const char   HttpErrOutOfMem[] PROGMEM = "Out of memory";

/**** Local variables ****/

static std::map<AsyncWebServerRequest *, String> PostBuffers;

#ifdef ESP8266
static size_t     UpdateContentLen;
#endif


/**** Http find mime ****/

const char * HttpServerFindMime(String &FileName)
{
  const TR_MIME *   Mime;

  for(Mime = MimeTypes;;++Mime) {
    const char *ExtPtr = (const char *)pgm_read_ptr(&(Mime -> FileExt));

    if (ExtPtr == NULL) break;
    if (FileName.endsWith(FPSTR(ExtPtr))) break;
  } /* End of for (Mime) */

  return (const char *)pgm_read_ptr(&(Mime -> Type));  
}


/**** Check the heap ****/

bool HttpServerHeapCheck(AsyncWebServerRequest *Request)
{
  uint32_t    FreeHeap = ESP.getFreeHeap();
  
  if (Prefs.Debug)
    Printf(F("[DEBUG] Free heap: %u\r\n"), FreeHeap);
  
  if (FreeHeap < TR_HEAP_WARN) {
    Request -> send_P(500, FPSTR(MimeText), HttpErrOutOfMem);
    
    if (Prefs.Debug)
      Print(F("[DEBUG] Low heap\r\n"));
    
    delay(1000);
    return false;
  }

  return true;
}


/**** Perform the update ****/

void HttpServerDoUpdate(AsyncWebServerRequest *Request, const String &FileName, size_t Index, uint8_t *Data, size_t Len, bool Final) 
{
  static const char   MsgErrUpd[] PROGMEM = "[DEBUG] Firmware update error\r\n";

  if (!Index) {
    if (Prefs.Debug)
      Print(F("[DEBUG] Begin firmware update\r\n"));

#ifdef RGB_BUILTIN
    neopixelWrite(RGB_BUILTIN, TR_RGB_BRIGHTNESS, TR_RGB_BRIGHTNESS, 0);
#endif

    OtaMode            = TR_OTA_HTTP;
    Prefs.SleepEnabled = false;

#ifdef ESP8266
    UpdateContentLen = Request -> contentLength();
#endif

    int Cmd          = (FileName.indexOf("spiffs") > -1) ? U_PART : U_FLASH;

#ifdef ESP32
  Update.onProgress(PrintUpdateProgress);
#endif

#ifdef ESP8266
    Update.runAsync(true);
    if (!Update.begin(UpdateContentLen, Cmd)) {
#else
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, Cmd)) {
#endif
      if (Prefs.Debug) {
        Print(FPSTR(MsgErrUpd));
        Update.printError(Serial);
      }
    }
  }

  if (Update.write(Data, Len) != Len) {
    if (Prefs.Debug) {
      Print(FPSTR(MsgErrUpd));
      Update.printError(Serial);
    }
#ifdef ESP8266
  } else if (Prefs.Debug) {
    Printf(F("[DEBUG] Progress: %d%%\r\n"), (Update.progress() * 100) / Update.size());
#endif
  }

  if (Final) {
    if (!Update.end(true)) {
      if (Prefs.Debug) {
        Print(FPSTR(MsgErrUpd));
        Update.printError(Serial);
      }
    } else {
      if (Prefs.Debug) {
        Print(FPSTR("[DEBUG] Update complete\r\n"));
        if (Serial) Serial.flush();
      }

      AddPendingCmd(TR_PENDCMD_REBOOT, 3000);
    }
  }
}


/**** Exec cgi callback ****/

void HttpServerExecCgiCallBack(AsyncWebServerRequest *Request)
{
  if (!HttpServerHeapCheck(Request)) return;

  bool          DoRefresh   = false;
  int           ParNum      = Request -> params();
  long          SkipMotMask = 0;

  if (Prefs.Debug)
    Printf("[DEBUG] exec.cgi request with %d parameters\n", ParNum);

  for(int i = 0; i < ParNum; i++) {
    const AsyncWebParameter * Par   = Request -> getParam(i);
    const char *              Name  = Par -> name().c_str();
    const char *              Value = Par -> value().c_str();
    
    if (Prefs.Debug)
      Printf(F("[DEBUG] exec.cgi parameter %d %s = %s\n"), i + 1, Name, Value);  

    /**** Default preferences ****/
    
    if (!strcasecmp_P(Name, PSTR("DefPrefs"))) {
      if(!Request -> authenticate(Prefs.AdminUser, Prefs.AdminPass)) {
        Request -> requestAuthentication();
        return;
      }
      Prefs.Load(true);
      HttpServerGetConfigCallBack(Request);
      return;
    }

    /**** Motor ****/

    if ((!strncasecmp_P(Name, PSTR("Motor"), 5)) && (strlen(Name) == 6)) {
      uint8_t Channel = Name[5] - '0';
      PfTrains.MotorSpeedSoft(Channel, atoi(Value));
      SkipMotMask |= 1u << Channel;
    }
 
    /**** Reboot ****/
 
    if (!strcasecmp_P(Name, PSTR("Reboot"))) {
      if(!Request -> authenticate(Prefs.AdminUser, Prefs.AdminPass)) {
        Request -> requestAuthentication();
        return;
      }
      AddPendingCmd(TR_PENDCMD_REBOOT, 3000);
      HttpServerSendGzFile(Request, F(TR_HTML_DIR "/updating.html.gz"));
      return;
    }

    /**** Refresh ****/
 
    if (!strcasecmp_P(Name, PSTR("Refresh"))) DoRefresh = true;
  
    /**** Suspend ****/

    if (!strcasecmp_P(Name, PSTR("Suspend"))) {
      if (!Request -> authenticate(Prefs.AdminUser, Prefs.AdminPass)) {
        Request -> requestAuthentication();
        return;
      }
      AddPendingCmd(TR_PENDCMD_SUSPEND, 3000);
      //Request -> send(LittleFS, F(TR_HTML_DIR "/suspended.html"), FPSTR(MimeHtml));
      HttpServerSendGzFile(Request, F(TR_HTML_DIR "/suspended.html.gz"));
      return;
    }
  } /* End of for (i) */

  AsyncResponseStream * Response = Request -> beginResponseStream(FPSTR(MimeJson));

  Response -> addHeader(F("Access-Control-Allow-Origin" ), F("*"                 ));
  Response -> addHeader(F("Access-Control-Allow-Methods"), F("GET, POST, OPTIONS"));
  Response -> addHeader(F("Access-Control-Allow-Headers"), F("Content-Type"      ));
  
  Response -> print("{");

  /**** Refresh ****/

  if (DoRefresh) {
    char          StrKey[32];

    const char *  FmtKey = "\"%s\": \"%s\",";

    /**** Host name ****/
    
    strncpy_P(StrKey, StrKeyHostName, sizeof(StrKey));
    Response -> printf("\"%s\": \"%s\",", StrKey, Prefs.HostName);
 
    /**** Trains ****/ 

    for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel) {
      
      /**** Enabled ****/
 
      snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainEnabled, (unsigned int)Channel);
      Response -> printf("\"%s\": %s,", StrKey, Bool2Tf(PfTrains.GetEnabled(Channel)));

      /**** Train name ****/
      
      snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainName, (unsigned int)Channel);
      Response -> printf(FmtKey, StrKey, PfTrains.GetName(Channel));

      /**** Train reverse name ****/
      
      snprintf_P(StrKey, sizeof(StrKey), StrKeyTrainNameRev, (unsigned int)Channel);
      Response -> printf(FmtKey, StrKey, PfTrains.GetNameReverse(Channel));
    } /* End of for (Channel) */
  }
  
  /**** Motor status ****/

  for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel) {
    if (SkipMotMask & (1u << Channel)) continue;
    Response -> printf("\"Motor%u\": %d,", (unsigned int)Channel, PfTrains.GetSpeed(Channel));
  } /* End of for (Channel) */

  /**** Battery & version ****/

  Response -> printf(
    "\"Battery\": %d,"
    "\"BatteryLow\":%s,"
    "\"Voltage\": %d,"
    "\"Version\": \"" TR_VERSION "\""
    "}",
    BattLevel,
    Bool2Tf(BattLow),
    BattMilliVolts
  );

  Request -> send(Response);
  LastCmdTime = millis();
}


/**** Send configuration callback ****/

void HttpServerGetConfigCallBack(AsyncWebServerRequest *Request)
{
  if (!Request -> authenticate(Prefs.AdminUser, Prefs.AdminPass)) {
    Request -> requestAuthentication();
    return;
  }

  AsyncResponseStream * Response = Request -> beginResponseStream(FPSTR(MimeJson));

  Prefs.SaveToStream(*Response, true);

  Request -> send(Response);
  LastCmdTime = millis();
}


/**** Not found event ****/

void HttpServerNotFoundCallBack(AsyncWebServerRequest *Request)
{
  if (!HttpServerHeapCheck(Request)) return;
  
  /**** Hue emulation ****/

  if ((HueEnabled) && (Fauxmo)) {
    String Body = (Request -> hasParam("body", true)) ? Request -> getParam("body", true) -> value() : String();
    if (Fauxmo -> process(Request -> client(), Request -> method() == HTTP_GET, Request -> url(), Body)) return;
  }

  LastCmdTime = millis();

  /**** Page on file system ****/

  String Url = Request -> url();
  if (Url.indexOf("..") != -1) {
    Request -> send(400, FPSTR(MimeText), F("Bad request"));
    return;
  }
  if (Url.endsWith("/")) Url += "index.html";

  String FileName = String(TR_HTML_DIR) + Url;

  if (Prefs.Debug)
    Printf(F("[DEBUG] Http file requested %s\r\n"), FileName.c_str());

  const char *  MimeType = HttpServerFindMime(FileName);

  if (LittleFS.exists(FileName.c_str())) {
    Request -> send(LittleFS, FileName.c_str(), FPSTR(MimeType));
    return;
  }

  /**** Not found ****/

  if (Prefs.Debug)
    Printf(F("[DEBUG] File %s not found\r\n"), FileName.c_str());

  AsyncWebServerResponse *Response = Request -> beginResponse(LittleFS, F(TR_HTML_DIR "/err_not_found.html"), FPSTR(MimeHtml));
  Response -> setCode(404); 
  Request  -> send(Response);
}


/**** Send a compressed HTML page ****/

void HttpServerSendGzFile(AsyncWebServerRequest *Request, const __FlashStringHelper *FileName)
{
  AsyncWebServerResponse *    Response = Request -> beginResponse(LittleFS, FileName, FPSTR("text/html"));
  if (Response) {
    Response -> addHeader(F("Content-Encoding"), F("gzip"));
    Request -> send(Response);
  }
}


/**** HTTP server setup ****/

void HttpServerSetup(void)
{
  static const char UriConfig[] PROGMEM = "/config.cgi";
 
  /**** config.cgi ****/

  HttpServer.on((const char *)UriConfig, HTTP_GET, HttpServerGetConfigCallBack);

  HttpServer.on((const char *)UriConfig, HTTP_POST, [](AsyncWebServerRequest *Request){}, NULL, [](
                AsyncWebServerRequest *Request, uint8_t *Data, size_t Len, size_t Index, size_t Total) {
    if (!Request -> authenticate(Prefs.AdminUser, Prefs.AdminPass)) {
      Request -> requestAuthentication();
      return;
    }

    if ((Total > 0) && (Total <= 4096)) {
      if (!Index) {
        PostBuffers[Request] = String();
        PostBuffers[Request].reserve(Total);       
      }

      PostBuffers[Request].concat((const char *)Data, (unsigned int)Len);

      if (Index + Len == Total) {
        const String &FullPost = PostBuffers[Request];
        if (Prefs.Debug)
          Serial.printf_P(PSTR("[DEBUG] POST Json config: %s\n"), FullPost.c_str());
 
        Request -> send(Prefs.FromJson(FullPost.c_str()) ? 200 : 400);
        PostBuffers.erase(Request);
      }
    } else Request -> send(400);
    LastCmdTime = millis();
  });

  /**** config.html ****/

  HttpServer.on((const char*)F("/config.html"), HTTP_GET, [](AsyncWebServerRequest *Request) {
    if (!HttpServerHeapCheck(Request)) return;
    if (!Request -> authenticate(Prefs.AdminUser, Prefs.AdminPass)){
      Request -> requestAuthentication();
      return;
    }

    //Request -> send(LittleFS, F(TR_HTML_DIR "/config.html"), FPSTR(MimeHtml));
    HttpServerSendGzFile(Request, F(TR_HTML_DIR "/config.html.gz"));
    LastCmdTime = millis();
  });

  /**** exec.cgi ****/
  
  HttpServer.on((const char*)F("/exec.cgi"), HTTP_GET, HttpServerExecCgiCallBack);

  /**** index.html ****/
  
  HttpServer.on("/", HTTP_GET, [](AsyncWebServerRequest *Request) {
    if (!HttpServerHeapCheck(Request)) return;
    Request -> redirect(F("/index.html"));
    LastCmdTime = millis();
  });

  HttpServer.on((const char*)F("/index.html"), HTTP_GET, [](AsyncWebServerRequest *Request) {
    if (!HttpServerHeapCheck(Request)) return;
    HttpServerSendGzFile(Request, F(TR_HTML_DIR "/index.html.gz"));
    LastCmdTime = millis();
  });

  /**** settings.js ****/

  HttpServer.on((const char*)F("/js/settings.js"), HTTP_GET, [](AsyncWebServerRequest *Request) {
    if (!HttpServerHeapCheck(Request)) return;
    
    AsyncResponseStream * Response = Request -> beginResponseStream(FPSTR(MimeJs));

    /**** Speech synthesis ****/

    Response -> print(F("var setSpeechEn = "));
    Response -> print(Bool2Tf(Prefs.SpeechEnabled));
    Response -> println(F(";"));

    /**** Control device vibration ****/
 
    Response -> print(F("var setVibrationEn = "));
    Response -> print(Bool2Tf(Prefs.VibrEnabled));
    Response -> println(F(";"));
    
    Response -> addHeader("Cache-Control", "no-store, no-cache, must-revalidate, max-age=0");
    Response -> addHeader("Pragma", "no-cache");
    Response -> addHeader("Expires", "0");
    Request  -> send(Response); 

    LastCmdTime = millis();             
  });

  /**** OTA ****/

  HttpServer.on((const char*)F("/update"     ), HTTP_GET, HttpServerUpdateCallBack);
  HttpServer.on((const char*)F("/update.html"), HTTP_GET, HttpServerUpdateCallBack);

  HttpServer.on((const char*)F("/updating.html"), HTTP_GET, [](AsyncWebServerRequest *Request) {
    if (!HttpServerHeapCheck(Request)) return;
    HttpServerSendGzFile(Request, F(TR_HTML_DIR "/updating.html.gz"));
    LastCmdTime = millis();
  });

  HttpServer.on((const char*)F("/doUpdate"), HTTP_POST, [](AsyncWebServerRequest *Request) {
    if (!Request -> authenticate(Prefs.AdminUser, Prefs.AdminPass))
      return Request -> requestAuthentication();
 
      //Request -> send(LittleFS, F(TR_HTML_DIR "/updating.html"), FPSTR(MimeHtml));
      HttpServerSendGzFile(Request, F(TR_HTML_DIR "/updating.html.gz"));
      
  }, [](AsyncWebServerRequest *Request, const String &FileName, size_t Index, uint8_t *Data, size_t Len, bool Final) {
    HttpServerDoUpdate(Request, FileName, Index, Data, Len, Final);
  });

  /**** These two callbacks are required for gen1 and gen3 hue compatibility ****/
  
  HttpServer.onRequestBody([](AsyncWebServerRequest *Request, uint8_t *Data, size_t Len, size_t Index, size_t Total) 
  {
//    LastCmdTime = millis();
    if ((HueEnabled) && (Fauxmo) &&
        (Fauxmo -> process(Request -> client(), Request -> method() == HTTP_GET, Request -> url(), String((char *)Data))))
      return;
    // Handle any other body request here...
  });

  /**** Not found ****/

  HttpServer.onNotFound(HttpServerNotFoundCallBack);

  /**** Close the connection to preserve the heap ****/

  DefaultHeaders::Instance().addHeader("Connection", "close");
  
  /**** Start the server ****/

  HttpServer.begin();
}


/**** Send the update page ****/

void HttpServerUpdateCallBack(AsyncWebServerRequest *Request)
{
  if (!HttpServerHeapCheck(Request)) return;

  if (!Request -> authenticate(Prefs.AdminUser, Prefs.AdminPass))
    return Request -> requestAuthentication();

  //Request -> send(LittleFS, F(TR_HTML_DIR "/update.html"), FPSTR(MimeHtml));
  HttpServerSendGzFile(Request, F(TR_HTML_DIR "/update.html.gz"));
  LastCmdTime = millis();
}


/**** Print the update progress ****/

void PrintUpdateProgress(size_t Prg, size_t Sz)
{
  if (!Prefs.Debug) return;

  if (Sz == 0) {
    Printf(F("[DEBUG] Update progress: %u bytes\r\n"), (unsigned)Prg);
    return;
  }
  
  Printf(F("[DEBUG] Update progress: %lu%%\r\n"), 
        (unsigned long)((uint64_t)Prg * 100ULL / (uint64_t)Sz));
}
