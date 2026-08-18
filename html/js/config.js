
/**************************************
****   TrenoLEGO - Configuration   ****
**** (c) 2025, Alessandro Pedretti ****
**************************************/


/**** Global variables ****/

var   hBtMenu, hBtMusic, hBtMusicImg;
var   hMenu, hTabs;

var   TextPrefs         = "";
var   TrainAccAmount    = [];
var   TrainAccStpTime   = [];
var   TrainEn           = [];
var   TrainInv          = [];
var   TrainName         = [];
var   TrainNameRev      = [];
var   TrainPort         = [];

/**** Localization ****/

if (Lang == 'it') {

  /**** Italian ****/

  Msg = {
    AskReboot             : "Vuoi riavviare il sistema?",
    AskSave               : "Vuoi salvare le impostazioni?",

    BtApply               : "Applica",
    BtControls            : "Controlli",
    BtMusic               : "Musica",
    BtMusicPlay           : "Play",
    BtMusicStop           : "Stop",
    BtSave                : "Salva",
    BtUpdate              : "Aggiornamento firmware",

    CbTrainPortBlue       : "Blu",
    CbTrainPortRed        : "Rossa",

    Err                   : "ERRORE: ",
    ErrAccAmount          : "Incremento accelerazione non valido (1...255)",
    ErrAccStpTime         : "Incremento tempo accelerazione non valido (1...1000 ms)",
    ErrConfigApply        : "Impossibile applicare la configurazione",
    ErrConfigSave         : "Impossibile salvare la configurazione",
    ErrDefault            : "Impossibile caricare le impostazioni predefinite",
    ErrFields             : "Tutti i campi devono essere compilati.",
    ErrHostName           : "Nome host non valido",
    ErrMqttServer         : "Server MQTT non valido",
    ErrNtpServer          : "Server NTP non valido",
    ErrSendCmd            : "Non posso inviare il comando",
    ErrTcpPort            : "Porta TCP/IP non valida",
    ErrTimeZone           : "Time zone non valido",
    ErrUndo               : "Impossibile tornare alle impostazioni precedenti",

    LblAdmin              : "Credenziali amministratore",
    LblDebug              : "Modalità debug:",
    LblEnabled            : "Abilitato:",
    LblFtpEn              : "FTP abilitato:",
    LblFtpSrv             : "Server FTP e Telnet",
    LblHostName           : "Nome host:",
    LblHue                : "Emulazione Hue/Alexa:",
    LblInvert             : "Inverti direzione:",
    LblLego               : "Abilitata di default:",
    LblMqttPort           : "Porta:",
    LblOther              : "Altre impostazioni",
    LblPoweredUp          : "Emulazione Powered Up",
    LblPuBind             : "Connessa al treno",
    LblPuBindCh           : "Canale:",
    LblPuPort             : "Porta emulata:",
    LblPwrSaving          : "Risparmio energetico",
    LblSleepTime          : "Attesa (sec.):",
    LblSpeechEn           : "Sintesi vocale:",
    LblTelnetEn           : "Telnet abilitato:",
    LblTimeZone           : "Fuso orario:",
    LblTitle              : "Configurazione",
    LblTrain              : "Canale ",
    LblTrainAccAmount     : "Inc. accelerazione:",
    LblTrainAccStpTime    : "Inc. tempo accel. (ms):",
    LblTrainName          : "Nome:",
    LblTrainNameRev       : "Nome indietro:",
    LblTrainPort          : "Porta:",
    LblUserName           : "Nome utente:",
    LblVibrEn             : "Vibrazione disp. contr.:",

    MmDefault             : "Predefiniti",
    MmReboot              : "Riavvia",
    MmSuspend             : "Sospendi",
    MmUndo                : "Annulla",

    TabNetwork            : "Rete",
    TabOthers             : "Altro",
    TabTrains             : "Treni",

    Title                 : "TrenoLEGO - Configurazione",

    TtsApply              : "Applico le impostazioni",
    TtsDefault            : "Impostazioni predefinite caricate",
    TtsReboot             : "Riavvio il sistema",
    TtsSave               : "Salvo le impostazioni",
    TtsSuspend            : "Sospendo il sistema",
    TtsUndo               : "Modifiche annullate"
  };

} else {

  /**** Default language (english) ****/

  Msg = {
    AskReboot             : "Do you want to reboot the system?",
    AskSave               : "Do you want to save the settings?",

    BtApply               : "Apply",
    BtControls            : "Controls",
    BtMusic               : "Music",
    BtMusicPlay           : "Play",
    BtMusicStop           : "Stop",
    BtSave                : "Save",
    BtUpdate              : "Firmware update",

    CbTrainPortBlue       : "Blue",
    CbTrainPortRed        : "Red",

    Err                   : "ERROR: ",
    ErrAccAmount          : "Invalid acceleration amount (1...255)",
    ErrAccStpTime         : "Invalid acceleration step time (1...1000 ms)",
    ErrConfigApply        : "Unable to apply the configuration",
    ErrConfigSave         : "Unable to save the configuration",
    ErrDefault            : "Unable to load the defult settings",
    ErrFields             : "All fields must be filled in",
    ErrHostName           : "Invalid host name",
    ErrMqttServer         : "Invalid MQTT server",
    ErrNtpServer          : "Invalid NTP server",
    ErrSendCmd            : "Unable to send the command",
    ErrTimeZone           : "Invalid time zone",
    ErrTcpPort            : "Invalid TCP/IP port",
    ErrUndo               : "Can't revert to last settings",

    LblAdmin              : "Administartor credentials",
    LblDebug              : "Debug mode:",
    LblEnabled            : "Enabled:",
    LblFtpEn              : "FTP enabled:",
    LblFtpSrv             : "FTP and telnet servers",
    LblHostName           : "Host name:",
    LblHue                : "Hue/Alexa emulation:",
    LblInvert             : "Invert direction:",
    LblLego               : "Default enabled:",
    LblMqttPort           : "Port:",
    LblOther              : "Other settings",
    LblPoweredUp          : "Powered Up emulation",
    LblPuBind             : "Bind to train",
    LblPuBindCh           : "Channel:",
    LblPuPort             : "Emulated port:",
    LblPwrSaving          : "Power saving",
    LblSleepTime          : "Delay (sec.):",
    LblSpeechEn           : "Speech synthesis:",
    LblTelnetEn           : "Telnet enabled:",
    LblTimeZone           : "Time zone:",
    LblTitle              : "Configuration",
    LblTrain              : "Channel ",
    LblTrainAccAmount     : "Acc. amount:",
    LblTrainAccStpTime    : "Acc. step time (ms):",
    LblTrainName          : "Name:",
    LblTrainNameRev       : "Name reverse:",
    LblTrainPort          : "Port:",
    LblUserName           : "User name:",
    LblVibrEn             : "Ctlr device vibration:",

    MmDefault             : "Default",
    MmReboot              : "Reboot",
    MmSuspend             : "Suspend",
    MmUndo                : "Undo",

    TabNetwork            : "Network",
    TabOthers             : "Others",
    TabTrains             : "Trains",

    Title                 : "TrenoLEGO - Configuration",

    TtsApply              : "Settings applyied",
    TtsDefault            : "Default settings loaded",
    TtsReboot             : "System restared",
    TtsSave               : "Settings saved",
    TtsSuspend            : "System suspended",
    TtsUndo               : "Undo last changes"
  };
}


/**** Controls button click ****/

function BT_ControlsClick()
{
  Location("index.html", Msg.BtControls);
}


/**** Menu button click ****/

function BT_MenuClick()
{
  Menu.style.visibility     = "visible";
  Menu.style.opacity        = 1;
  hBtMenu.style.visibility  = "hidden";
  hBtMenu.style.opacity     = 0;
}


/**** Update button click ****/

function BT_UpdateClick()
{
  Location("update.html", Msg.BtUpdate);
}


/**** Menu command ****/

function CmdClick(Cmd)
{
  MenuClick();
  if (Cmd === undefined) return;

  switch(Cmd) {
  case "reboot":
    if (!Ask(Msg.AskReboot)) return;
    ttsSpeak(Msg.TtsReboot);
    break;

  case "suspend":
    ttsSpeak(Msg.TtsSuspend);
    break;

  default:
    return;
  } /* End of switch */

  window.location.href = "exec.cgi?" + Cmd;
}


/**** Debounce ****/

function Debounce(Func)
{
  var Timer;

  return function(Event) {
    if (Timer) clearTimeout(Timer);
    Timer = setTimeout(Func, 200, Event);
  };
}


/**** Default preferences ****/

function DefPrefClick()
{
  MenuClick();

  const xHttp = new XMLHttpRequest();
  xHttp.open("GET", "exec.cgi?defprefs", true);

  xHttp.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        ttsSpeak(Msg.TtsDefault);
        setTimeout(PrefsLoad, 1000);
      } else {
        ShowError(Msg.ErrDefault);
        console.error("Error HTTP:", this.status, this.responseText);
      }
    }
  };

  xHttp.send();
}


/**** Menu item click ****/

function MenuClick()
{
  hMenu.style.visibility   = "hidden";
  hMenu.style.opacity      = 0;
  hBtMenu.style.opacity    = 1;
  hBtMenu.style.visibility = "visible";
}


/**** Send to the server ****/

function HttpSend(Key, Val)
{
  const xHttp = new XMLHttpRequest();
  xHttp.open("GET", "exec.cgi?" + Key + "=" + Val, true);

  xHttp.onreadystatechange = function() {
    if ((this.readyState == 4) && (this.status != 200))
      ShowError(Msg.Err + Msg.ErrSendCmd + ".");
  }

  xHttp.send();
}


/**** Music ****/

function MusicToggle()
{
  if (!ChipPlayer) return;

  Music = !Music;
  if (Music) {
    if (!MusicLoaded) {
      ChipPlayer.load(MusUrl, function(Buffer) {
        ChipPlayer.play(Buffer);
        MusicLoaded = true;
      });
    }
    hBtMusicImg.src = "img/music_on.svg";
    hBtMusic.style.backgroundColor = "#0ACD1D";
  } else {
    hBtMusicImg.src = "img/music_off.svg";
    hBtMusic.style.backgroundColor = "#F9C124";
  }
  ChipPlayer.togglePause();
}


/**** Open the tab ****/

function OpenTab(PageName, Elem)
{
  var TabContent = document.getElementsByClassName("tabcontent");
  var TabLinks   = document.getElementsByClassName("tablink");

  for(var i = 0; i < TabContent.length; i++) {
    TabContent[i].style.display       = "none";
    TabLinks[i].style.backgroundColor = "";
    TabLinks[i].style.color           = "white";
  }

  document.getElementById(PageName).style.display = "block";
  Elem.style.backgroundColor                      = "rgba(0.8, 0.8, 0.8, 0.0)";
  Elem.style.color                                = "navy";
}


/**** Load the preferences ****/

function PrefsLoad()
{
  const xHttp = new XMLHttpRequest();

  xHttp.onreadystatechange = function() {
    if (this.readyState == 4) {
      if (this.status == 200) {
        PrefsParse(this.responseText);
      } else {
        console.error("Error HTTP:", this.status, this.responseText);
      }
    }
  };

  xHttp.open("GET", "config.cgi", true);
  xHttp.send();
}


/**** Parse the preferences ****/

function PrefsParse(Text)
{
  try {
    const JsonReply = JSON.parse(Text);

    SetCheckBox("ck_debug"   , JsonReply.Debug);
    SetCheckBox("ck_ftpen"   , JsonReply.FtpEnabled);
    SetCheckBox("ck_hueen"   , JsonReply.HueEnabled);
    SetCheckBox("ck_legoen"  , JsonReply.LegoHubEnabled);
    SetCheckBox("ck_mqtten"  , JsonReply.MqttEnabled);
    SetCheckBox("ck_sleep"   , JsonReply.SleepEnabled);
    SetCheckBox("ck_telneten", JsonReply.TelnetEnabled);
    SetCheckBox("ck_speechen", JsonReply.SpeechEnabled);
    SetCheckBox("ck_vibren"  , JsonReply.VibrationEnabled);

    SetValue("cb_pubport"    , JsonReply.LegoHubPort);
    SetValue("cb_pubindch"   , JsonReply.LegoHubConnChannel);

    SetValue("str_adminpwd"  , JsonReply.AdminPass);
    SetValue("str_adminuser" , JsonReply.AdminUser);
    SetValue("str_hostname"  , JsonReply.HostName);
    SetValue("str_mqttport"  , JsonReply.MqttPort);
    SetValue("str_mqttpwd"   , JsonReply.MqttPass);
    SetValue("str_mqttserver", JsonReply.MqttServer);
    SetValue("str_mqttuser"  , JsonReply.MqttUser);
    SetValue("str_ntpserver" , JsonReply.NtpServer);
    SetValue("str_sleeptime" , JsonReply.SleepTime);
    SetValue("str_ssid"      , JsonReply.WiFiSSID);
    SetValue("str_timezone"  , JsonReply.TimeZone);
    SetValue("str_wifipwd"   , JsonReply.WiFiPass);

    for (let i = 0; i < TrainNum; i++) {
      const TrainID = "Train" + i;

      if (TrainAccAmount[i] && JsonReply[TrainID + "AccelAmount"] !== undefined)
        TrainAccAmount[i].value = String(JsonReply[TrainID + "AccelAmount"]);

      if (TrainAccStpTime[i] && JsonReply[TrainID + "AccelStepTime"] !== undefined)
        TrainAccStpTime[i].value = String(JsonReply[TrainID + "AccelStepTime"]);

      if (TrainEn[i] && JsonReply[TrainID + "Enabled"] !== undefined)
        TrainEn[i].checked = Boolean(JsonReply[TrainID + "Enabled"]);

      if (TrainInv[i] && JsonReply[TrainID + "Invert"] !== undefined)
        TrainInv[i].checked = Boolean(JsonReply[TrainID + "Invert"]);

      if (TrainName[i] && JsonReply[TrainID + "Name"] !== undefined)
        TrainName[i].value = String(JsonReply[TrainID + "Name"]);

      if (TrainNameRev[i] && JsonReply[TrainID + "NameRev"] !== undefined)
        TrainNameRev[i].value = String(JsonReply[TrainID + "NameRev"]);

      if (TrainPort[i] && JsonReply[TrainID + "Port"] !== undefined)
        TrainPort[i].value = String(JsonReply[TrainID + "Port"]);
    } /* End of for (i) */

    TextPrefs = Text;

    /**** Check if BLE is not available ****/

    if (!Boolean(JsonReply.LegoHubAvail))  {
      document.getElementById("PoweredUp"    ).style.display = "none";
      document.getElementById("tab_poweredup").style.display = "none";
    }

  } catch (e) {
    console.error("Errore parsing JSON:", e, this.responseText);
    TextPrefs = "";
  }
}


/**** Save the settings ****/

function SaveClick(SaveCfg)
{
  if (((SaveCfg) && (!Ask(Msg.AskSave))) || (!ValidateFields()))
    return;

  var JsonStr = '{' +
                '"LegoHubPort": "'       + document.getElementById("cb_pubport"        ).value   + '",' +
                '"LegoHubConnChannel": ' + document.getElementById("cb_pubindch"       ).value   + ','  +

                '"Debug": '              + document.getElementById("ck_debug"          ).checked + ','  +
                '"FtpEnabled": '         + document.getElementById("ck_ftpen"          ).checked + ','  +
                '"HueEnabled": '         + document.getElementById("ck_hueen"          ).checked + ','  +
                '"LegoHubEnabled": '     + document.getElementById("ck_legoen"         ).checked + ','  +
                '"MqttEnabled": '        + document.getElementById("ck_mqtten"         ).checked + ','  +
                '"SleepEnabled": '       + document.getElementById("ck_sleep"          ).checked + ','  +
                '"SpeechEnabled": '      + document.getElementById("ck_speechen"       ).checked + ','  +
                '"TelnetEnabled": '      + document.getElementById("ck_telneten"       ).checked + ','  +
                '"VibrationEnabled": '   + document.getElementById("ck_vibren"         ).checked + ','  +

                '"AdminPass": "'         + document.getElementById("str_adminpwd"      ).value   + '",' +
                '"AdminUser": "'         + document.getElementById("str_adminuser"     ).value   + '",' +
                '"HostName": "'          + document.getElementById("str_hostname"      ).value   + '",' +
                '"MqttPort": '           + document.getElementById("str_mqttport"      ).value   + ','  +
                '"MqttPass": "'          + document.getElementById("str_mqttpwd"       ).value   + '",' +
                '"MqttServer": "'        + document.getElementById("str_mqttserver"    ).value   + '",' +
                '"MqttUser": "'          + document.getElementById("str_mqttuser"      ).value   + '",' +
                '"NtpServer": "'         + document.getElementById("str_ntpserver"     ).value   + '",' +
                '"SleepTime": '          + document.getElementById("str_sleeptime"     ).value   + ','  +
                '"TimeZone": "'          + document.getElementById("str_timezone"      ).value   + '",' +
                '"WiFiSSID": "'          + document.getElementById("str_ssid"          ).value   + '",' +
                '"WiFiPass": "'          + document.getElementById("str_wifipwd"       ).value   + '",';

  for (let i = 0; i < TrainNum; i++) {
    const TrainID = "Train" + i;

    JsonStr += '"' + TrainID + 'AccelAmount": '   + TrainAccAmount[i].value  + ','  +
               '"' + TrainID + 'AccelStepTime": ' + TrainAccStpTime[i].value + ','  +
               '"' + TrainID + 'Enabled": '       + TrainEn[i].checked       + ','  +
               '"' + TrainID + 'Invert": '        + TrainInv[i].checked      + ','  +
               '"' + TrainID + 'Name": "'         + TrainName[i].value       + '",' +
               '"' + TrainID + 'NameRev": "'      + TrainNameRev[i].value    + '",';

    if (TrainPort[i] !== null)
      JsonStr += '"' + TrainID + 'Port": "'    + TrainPort[i].value    + '",';
  } /* End of for (i) */

  JsonStr += '"Save": ' + SaveCfg + '}';

  const xHttp = new XMLHttpRequest();
  xHttp.open("POST", "config.cgi", true);
  xHttp.setRequestHeader("Content-Type", "application/json");

  xHttp.onreadystatechange = function() {
    if ((this.readyState == 4) && (this.status != 200))
      ShowError(Msg.Err + (SaveCfg ? Msg.ErrConfigSave : Msg.ErrConfigApply) + ".");
  }

  ttsSpeak(SaveCfg ? Msg.TtsSave : Msg.TtsApply);
  xHttp.send(JsonStr);
}


/**** Set a CheckBox ****/

function SetCheckBox(Id, Value)
{
  const Elem = document.getElementById(Id);

  if (!Elem) {
    console.warn("Element not found: " + Id);
    return;
  }
  Elem.checked = Boolean(Value);
}


/**** Set the menu position ****/

function SetMenuPos()
{
  document.documentElement.style.setProperty('--tab-top', hTabs.offsetTop + 'px');
}


/**** Set a value ****/

function SetValue(Id, Value)
{
  const Elem = document.getElementById(Id);

  if (!Elem) {
    console.warn("Element not found: " + Id);
    return;
  }

  if ((Value !== undefined) && (Value !== null) && (Value !== ""))
    Elem.value = String(Value);
}


/**** Setup ****/

function Setup()
{
  hBtMusic    = document.getElementById("bt_music");
  hBtMusicImg = document.getElementById("bt_musicimg");
  hBtMenu     = document.getElementById("BT_Menu");
  hMenu       = document.getElementById("Menu");
  hTabs       = document.getElementById("Tabs");

  document.querySelector('#year').textContent = Year;

  /**** Translate the labels ****/

  TransConte("bt_apply"          , Msg.BtApply);
  TransTitle("bt_controls"       , Msg.BtControls);
  TransConte("bt_save"           , Msg.BtSave);
  TransTitle("bt_update"         , Msg.BtUpdate);

  TransInner("lbl_admin"         , Msg.LblAdmin);
  TransInner("lbl_adminuser"     , Msg.LblUserName);
  TransInner("lbl_debug"         , Msg.LblDebug);
  TransInner("lbl_ftpen"         , Msg.LblFtpEn);
  TransInner("lbl_ftpsrv"        , Msg.LblFtpSrv);
  TransInner("lbl_hostname"      , Msg.LblHostName);
  TransInner("lbl_hue"           , Msg.LblHue);
  TransInner("lbl_lego"          , Msg.LblLego);
  TransInner("lbl_mqtten"        , Msg.LblEnabled);
  TransInner("lbl_mqttport"      , Msg.LblMqttPort);
  TransInner("lbl_mqttuser"      , Msg.LblUserName);
  TransInner("lbl_other"         , Msg.LblOther);
  TransInner("lbl_poweredup"     , Msg.LblPoweredUp);
  TransInner("lbl_pubind"        , Msg.LblPuBind);
  TransInner("lbl_pubindch"      , Msg.LblPuBindCh);
  TransInner("lbl_puport"        , Msg.LblPuPort);
  TransInner("lbl_pwrsaving"     , Msg.LblPwrSaving);
  TransInner("lbl_sleep"         , Msg.LblEnabled);
  TransInner("lbl_sleeptime"     , Msg.LblSleepTime);
  TransInner("lbl_speechen"      , Msg.LblSpeechEn);
  TransInner("lbl_telneten"      , Msg.LblTelnetEn);
  TransInner("lbl_timezone"      , Msg.LblTimeZone);
  TransInner("lbl_title"         , Msg.LblTitle);
  TransInner("lbl_vibren"        , Msg.LblVibrEn);

  TransInner("mm_apply"          , Msg.BtApply);
  TransInner("mm_default"        , Msg.MmDefault);
  TransInner("mm_reboot"         , Msg.MmReboot);
  TransInner("mm_save"           , Msg.BtSave);
  TransInner("mm_suspend"        , Msg.MmSuspend);
  TransInner("mm_undo"           , Msg.MmUndo);

  TransConte("tab_network"       , Msg.TabNetwork);
  TransConte("tab_others"        , Msg.TabOthers);
  TransConte("tab_trains"        , Msg.TabTrains);


  for(let k = 0; k < TrainNum; ++k) {
    TransInner("lbl_train_accamount"  + k, Msg.LblTrainAccAmount);
    TransInner("lbl_train_accstptime" + k, Msg.LblTrainAccStpTime);
    TransInner("lbl_train"            + k, Msg.LblTrain + k + " - " + (k ? "Power Functions" : "4.5V"));
    TransInner("lbl_train_en"         + k, Msg.LblEnabled);
    TransInner("lbl_train_inv"        + k, Msg.LblInvert);
    TransInner("lbl_train_name"       + k, Msg.LblTrainName);
    TransInner("lbl_train_namerev"    + k, Msg.LblTrainNameRev);

    if (k) {
      TransInner("lbl_train_port"    + k, Msg.LblTrainPort);

      document.querySelector('#cb_train_port' + k + ' option[value="RED"]').textContent  = Msg.CbTrainPortRed;
      document.querySelector('#cb_train_port' + k + ' option[value="BLUE"]').textContent = Msg.CbTrainPortBlue;
    }
  } /* End of for (k) */


  document.title = Msg.Title;
  hBtMusic.title = Msg.BtMusic;

  /**** Fill the vectors ****/

  for(let k = 0; k < TrainNum; ++k) {
    TrainAccAmount.push(document.getElementById("str_train_accamount" + k));
    TrainAccStpTime.push(document.getElementById("str_train_accstptime" + k));
    TrainEn.push(document.getElementById("ck_train_en" + k));
    TrainInv.push(document.getElementById("ck_train_inv" + k));
    TrainName.push(document.getElementById("str_train_name" + k));
    TrainNameRev.push(document.getElementById("str_train_namerev" + k));
    TrainPort.push(document.getElementById("cb_train_port" + k));
  } /* End of for (k) */

  /**** Events ****/

  document.getElementById('ck_speechen').addEventListener('change', (Event) => {
    ttsEnable(Event.target.checked);
  });

  /**** Get the preferences ****/

  PrefsLoad();

  document.getElementById("tab_network").click();

  /**** Menu position ***/

  SetMenuPos();
  new ResizeObserver(Debounce(SetMenuPos)).observe(hTabs);
}


/**** Undo click ****/

function UndoClick()
{
  MenuClick();
  if (TextPrefs == "") {
    ShowError(Msg.ErrUndo);
    return;
  }
  PrefsParse(TextPrefs);
  ttsSpeak(Msg.TtsApply);
}


/**** Validate the fields ****/

function ValidateFields()
{
  //const Inputs = document.querySelectorAll('input[type="number"], input[type="password"], input[type="text"]');
  const Inputs = [...document.querySelectorAll("input")].filter(input =>
                 ["text", "password", "number"].includes(input.type));
  let   Valid = true;

  Inputs.forEach(Input => {
    if (Input.value.trim() === "") {
      Valid              = false;
      Input.style.border = "2px solid red";
    } else {
      Input.style.border = "";
    }
  });

  if (!Valid) {
    ShowError(Msg.ErrFields);
    return false;
  }

  if ((!ValidateHostNameById("str_hostname", Msg.ErrHostName  )) ||
      (!ValidateTimeZoneById("str_timezone", Msg.ErrTimeZone  )) ||
      (!ValidateHostServerById("str_mqttserver", Msg.ErrMqttServer)) ||
      (!ValidateServerById("str_ntpserver" , Msg.ErrNtpServer )) ||
      (!ValidateNumberById("str_mqttport"  , 1, 65535, Msg.ErrTcpPort)))
    return false;

  for (let i = 0; i < TrainNum; i++) {
    if ((!ValidateNumberById("str_train_accamount"  + i, 1, 255 , Msg.ErrAccAmount )) ||
        (!ValidateNumberById("str_train_accstptime" + i, 1, 1000, Msg.ErrAccStpTime)))
      return false;
  } /* End of for (i) */

  return true;
}


/**** Validate number ****/

function ValidateNumberById(Id, Min, Max, ErrMsg)
{
  const Handle = document.getElementById(Id);
  const Val    = parseInt(Handle.value, 10);

  if ((isNaN(Val)) || (Val < Min) || (Val > Max)) {
    Handle.style.border = "2px solid red";
    ShowError(ErrMsg);
    return false;
  }

  return true;
}


/**** Validate host name ****/

function ValidateHostNameById(Id, ErrMsg)
{
  const Handle = document.getElementById(Id);

  if (!IsHostName(Handle.value)) {
    Handle.style.border = "2px solid red";
    ShowError(ErrMsg);
    return false;
  }

  return true;
}


/**** Validate host & server ****/

function ValidateHostServerById(Id, ErrMsg)
{
  const Handle = document.getElementById(Id);

  if ((!IsHostName(Handle.value)) && (!IsDomainName(Handle.value)) && (!IsIP(Handle.value))) {
    Handle.style.border = "2px solid red";
    ShowError(ErrMsg);
    return false;
  }

  return true;
}


/**** Validate server ****/

function ValidateServerById(Id, ErrMsg)
{
  const Handle = document.getElementById(Id);

  if ((!IsDomainName(Handle.value)) && (!IsIP(Handle.value))) {
    Handle.style.border = "2px solid red";
    ShowError(ErrMsg);
    return false;
  }

  return true;
}


/**** Validate time zone ****/

function ValidateTimeZoneById(Id, ErrMsg)
{
  const Handle = document.getElementById(Id);

  if (!IsPosixTZ(Handle.value)) {
    Handle.style.border = "2px solid red";
    ShowError(ErrMsg);
    return false;
  }

  return true;
}
