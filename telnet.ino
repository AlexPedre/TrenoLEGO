
/**************************************
****   TrenoLEGO - Telnet server   ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** On connect event ****/

void TelnetOnConnect(String Ip)
{
  TelnetLoginRetry = 0;
  TelnetMode       = TR_TELNET_USER;
  TelnetLoginUser  = "";

  if (Prefs.Debug)
    Serial.printf_P(PSTR("[DEBUG] Telnet: %s connected\n"), Ip.c_str());

  Printf(F("\r\n"
           "Welcome %s to TrenoLegOS V" TR_VERSION "\r\n"
           "User name: "),
          Telnet -> getIP().c_str());
  LastCmdTime = millis();
}


/**** Telnet connection attempt event ****/

void TelnetOnConnectionAttempt(String Ip) 
{
  if (Prefs.Debug)
    Serial.printf_P(PSTR("[DEBUG] Telnet: %s tried to connect\n"), Ip.c_str());
}


/**** On disconnect event ****/

void TelnetOnDisconnect(String Ip) 
{
  if (Prefs.Debug)
    Serial.printf_P(PSTR("[DEBUG] Telnet: %s disconnected\n"), Ip.c_str());

  LastCmdTime = millis();
}


/**** Telnet reconnect event ****/

void TelnetOnReconnect(String Ip) 
{
  if (Prefs.Debug)
    Serial.printf_P(PSTR("[DEBUG] Telnet: %s reconnected\n"), Ip.c_str());

  LastCmdTime = millis();
}


/**** Telnet input event ****/

void TelnetOnInput(String Str)
{
  Str.trim();
  if (Str.length() > TR_TELNET_MAX_INPUT) Str = Str.substring(0, TR_TELNET_MAX_INPUT);

  switch(TelnetMode) {
  case TR_TELNET_USER:
    TelnetLoginUser = Str;
    TelnetMode      = TR_TELNET_PASS;
    Telnet -> print(F("Password: "));
    break;

  case TR_TELNET_PASS:
    if ((TelnetLoginUser == Prefs.AdminUser) && (Str == Prefs.AdminPass)) {
      TelnetMode = TR_TELNET_COM;
      Printf(F("User %s logged-in\r\n"
               "Type <bye/exit/quit> to disconnect and <help> for the command list\r\n"
               "\r\n"
               "> "),
             Prefs.AdminUser);
    } else {
      Telnet -> println(F("Wrong user name or password"));
      TelnetMode      = TR_TELNET_USER;
      TelnetLoginUser = "";
      if (++TelnetLoginRetry >= TR_TELNET_MAX_RETRY) Telnet -> disconnectClient();
      else Telnet -> print(F("User name: "));
    }
    break;
    
  case TR_TELNET_COM:
    if ((!strcasecmp_P(Str.c_str(), PSTR("bye" ))) ||
        (!strcasecmp_P(Str.c_str(), PSTR("exit"))) ||
        (!strcasecmp_P(Str.c_str(), PSTR("quit")))) {
      Telnet -> println(F("Disconnecting..."));
      Telnet -> flush();
      delay(10);
      Telnet -> disconnectClient();
    } else {
      char    Response[MyCommandParser::MAX_RESPONSE_SIZE];
      *Response = 0;

      if (Str.length() > 0) { 
        CmdParser.processCommand(Str.c_str(), Response);
        if (*Response) Telnet -> println(Response);
      }
      Telnet -> print(F("> "));
    }
    break;
  } /* End of switch */

  LastCmdTime = millis();
}


/**** Telnet setup ****/

void TelnetSetup(void)
{  
  Telnet = new ESPTelnet();
  
  Telnet -> onConnect(TelnetOnConnect);
  Telnet -> onConnectionAttempt(TelnetOnConnectionAttempt);
  Telnet -> onDisconnect(TelnetOnDisconnect);
  Telnet -> onInputReceived(TelnetOnInput);
  Telnet -> onReconnect(TelnetOnReconnect);

  if (!Telnet -> begin(23)) {
    TelnetEnabled = false;
    if (Prefs.Debug)
      Serial.println(F("ERROR: Can't initialize the telnet server"));
  }
}
