
/**************************************
****     TrenoLEGO - Main page     ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Debug mode ****/

const DEBUG               = false;

/**** Gamepad axes sensibility ****/

const GPAX_SEMSIBILITY    = 10;   /* Analog sensibility */

/**** Gamepad button mapping ****/

const GPBT_CONFIG         = 9;    /* PS4 OPTIONS              */
const GPBT_HELP           = 16;   /* PS4 PS                   */
const GPBT_MUSIC          = 8;    /* PS4 SHARE                */
const GPBT_LONGPRESSMS    = 1000; /* Time for long press (ms) */
const GPBT_TRAIN4_UP      = 7;    /* PS4 R2                   */
const GPBT_TRAIN4_DOWN    = 6;    /* PS4 L2                   */
const GPBT_TRAIN4_STOP_L1 = 4;    /* PS4 L1                   */
const GPBT_TRAIN4_STOP_R1 = 5;    /* PS4 R1                   */

const GPBT_SNDBUTTONS     = [  0,  1, 2, 3, 12, 13, 14, 15];
const GPBT_STOP           = [ 10, 11, 4, 5];

/**** Vibration settings ****/

const VIB_ACCEL_DURATION  = 1000; /* Acceleration duration (ms)              */
const VIB_ACCEL_STRONGMAG = 0.0;  /* Acceleration vibration strong magnitude */
const VIB_ACCEL_WEAKMAG   = 0.3;  /* Acceleration vibration weak magnitude   */
const VIB_BRAKE_DURATION  = 2000; /* Brake duration (ms)                     */
const VIB_BRAKE_STRONGMAG = 1.0;  /* Brake vibration strong magnitude        */
const VIB_BRAKE_WEAKMAG   = 0.3;  /* Brake vibration weak magnitude          */

/**** Global variables ****/

var   hBtAdd, hBtMusic, hBtMusicImg, hCapacityValue;
var   hPageLoading, hSoundButtons, hTitle, hVersionValue, Msg;

var   GamepadAxesState    = [];
var   GamepadButtonsState = [];
var   HostNameToAdd       = "";
var   MotorDebounceTimer  = [];
var   MotorSpeedLast      = [];
var   MotorSlider         = [];
var   MotorValue          = [];
var   TrainChannelMap     = [];
var   TrainHost           = [];
var   TrainName           = [];
var   TrainTable          = [];
var   TrainTableEn        = [ true, true, true, true, true ];
var   TrainTableNum       = 5;
var   GamepadAxes         = 0;
var   GamepadButtons      = 0;
var   GamepadIndex        = null;
var   GamepadPollTimer    = null;
var   GamepadVibAvail     = false;

const GamepadPollInter    = 30;  /* Hz */
const MotorDebounce       = 120; /* ms */



/**** Localization ****/

if (Lang == 'it') {

  /**** Italian ****/

  Msg = {
    BtAdd                 : "Aggiungi treno",
    BtConfig              : "Configurazione",
    BtMusic               : "Musica",
    BtMusicPlay           : "Play",
    BtMusicStop           : "Stop",
    BtStop                : "Arresta il treno",

    ErrHostNotAvail       : "Host non raggiungibile",
    ErrHostInvAnswer      : "Risposta non valida dall'host",
    ErrInvHostName        : "Nome dell'host o indirizzo IP non valido",
    ErrHttp               : "Errore HTTP:\n",

    LblBattery            : "Batteria:",
    LblLoading            : "Caricamento...",

    MsgAdded              : "Aggiunti %d treni",
    MsgAddedOne           : "Aggiunto un treno",
    MsgAddedNone          : "Nessun treno aggiunto",
    MsgAddTrain           : "Nome dell'host o indirizzo IP del treno da aggiungere:",
    MsgGamepadConn        : "Gamepad connesso",
    MsgGamepadDisconn     : "Gamepad disconnesso",
    MsgVibrTest           : "Test vibrazione",
    MsgVoiceEn            : "Sintesi vocale abilitata",
    MsgVoiceDis           : "Sintesi vocale disabilitata",

    SpForward             : "Avanti ",
    SpReverse             : "Indietro ",
    SpStop                : "Stop"
  };

} else {

  /**** Default language (english) ****/

  Msg = {
    BtAdd                 : "Add train",
    BtConfig              : "Configuration",
    BtMusic               : "Music",
    BtMusicPlay           : "Play",
    BtMusicStop           : "Stop",
    BtStop                : "Stop the train",

    ErrInvHostName        : "Invalid host name or IP address",
    ErrHostInvAnswer      : "Invalid answer from host",
    ErrHostNotAvail       : "Host not available",
    ErrHttp               : "HTTP error:\n",

    LblBattery            : "Battery:",
    LblLoading            : "Loading...",

    MsdAdded              : "Added %d trains",
    MsgAddedOne           : "Added one train",
    MsgAddedNone          : "No train has been added",
    MsgAddTrain           : "Host name or IP address of the train to add:",
    MsgGamepadConn        : "Gamepad connected",
    MsgGamepadDisconn     : "Gamepad disconnected",
    MsgVibrTest           : "Vibration test",
    MsgVoiceEn            : "Voice synthesis enabled",
    MsgVoiceDis           : "Voice synthesis disabled",

    SpForward             : "Forward ",
    SpReverse             : "Reverse ",
    SpStop                : "Stop"
  };
}


/**** Add a train control ****/

function AddTrain(id, name) 
{
  const Html = `
  <table id="train${id}" width="100%">
    <tr><td id="lbl_name${id}" align="center" style="line-height: 7vw;">
      ${name}
    </td></tr>
    <tr>
      <td width="92%" valign="middle">
        <input id="MotorSlider${id}" type="range" min="-255" max="255" value="0" class="motor"
          onchange="MotorChange(${id}, true)" oninput="MotorChange(${id}, false)"
          ondblclick="MotorStop(${id})">
        <div width="100%" align="right">
          <span id="MotorValue${id}" class="motorvalue"></span>
        </div>
      </td>
      <td width="8%" align="right">
        <button id="bt_trainstop${id}" class="button_stop" onclick="MotorStop(${id})">
          <img class="button_sndimg" src="img/brake.svg">
        </button>
      </td>
    </tr>
  </table>
  `;
    
  document.getElementById("TrainContainer").insertAdjacentHTML("beforeend", Html);
}


/**** Add train data ****/

function AddTrainData(Id, MapId, HostName = "")
{
  MotorSlider.push(document.getElementById("MotorSlider" + Id));
  MotorSpeedLast.push(0);
  MotorValue.push(document.getElementById("MotorValue" + Id));
  TrainName.push(document.getElementById("lbl_name" + Id));
  TrainTable.push(document.getElementById("train" + Id));
  MotorDebounceTimer.push(null)
  TrainChannelMap.push(MapId);
  TrainHost.push(HostName);
}


/**** Add train click ****/

function BT_AddClick()
{
  ttsSpeak(Msg.BtAdd);
  HostNameToAdd = prompt(Msg.MsgAddTrain, HostNameToAdd);
  if (HostNameToAdd === null) return;
  if (!IsHostName(HostNameToAdd) && !IsIP(HostNameToAdd)) {
    ToastMessage(Msg.ErrInvHostName);
    ttsSpeak(Msg.ErrInvHostName);
    return;
  }

  const xHttp = new XMLHttpRequest();

  xHttp.onreadystatechange = function() {
    if (this.readyState === 4) {
      if (this.status === 200) {
        try {
          const JsonReply  = JSON.parse(this.responseText);
          var   TrainAdded = 0;

          for(let i = 0; i < TrainNum; i++) {
            const MotorID = "Motor" + i;
            const TrainID = "Train" + i;

            if ((JsonReply[TrainID + "Enabled"] !== undefined) &&
                (JsonReply[TrainID + "Enabled"])) {
              var TrainName;

              TrainTableEn.push(true);
              const Id = TrainTableEn.length - 1;

              if (JsonReply[TrainID + "Name"] !== undefined)
                TrainName = JsonReply[TrainID + "Name"];

              AddTrain(Id, TrainName);
              AddTrainData(Id, i, HostNameToAdd);

              MotorValue[Id].innerHTML = Msg.SpStop;
              if (MotorSlider[Id] && JsonReply[MotorID] !== undefined)
                MotorSlider[Id].value = JsonReply[MotorID];
              ++TrainAdded;
            }            
          } /* End of for (i) */

          switch(TrainAdded) {
          case 0:
            ToastMessage(Msg.MsgAddedNone);  
            break;

          case 1:
            ToastMessage(Msg.MsgAddedOne);
            break;

          default:
            ToastMessage(Msg.MsgAdded.replace("%d", TrainAdded));
          } /* End of switch */

          TrainTableNum = TrainTableEn.filter(Boolean).length;
        } catch (e) {
          ToastMessage(Msg.ErrHostInvAnswer);
        }
        hBtAdd.disabled = false;
      } else {
        if (this.status) 
          ToastMessage(Msg.ErrHttp + this.status + "\n" + this.responseText);
      }
    }
  };

  xHttp.onerror = function () {
    ToastMessage(Msg.ErrHostNotAvail);
    hBtAdd.disabled = false;
  };

  hBtAdd.disabled = true;
  xHttp.open("GET", "http://" + HostNameToAdd + "/exec.cgi?refresh", true);
  xHttp.send();
}


/**** Configuration button click ****/

function BT_ConfigClick()
{
  Location("config.html", Msg.BtConfig);
}


/**** Mange the gamepad ****/

async function GamepadPoll()
{
  const GamepadHandle = navigator.getGamepads()[GamepadIndex];
  
  if (!GamepadHandle) return;

  const Now = performance.now();

  /**** Buttons ****/

  for(let k = 0; k < GamepadButtons; ++k) {
    var   BtVal   = Math.round((Math.round((GamepadHandle.buttons[k].value + Number.EPSILON) / 0.05) * 0.05) * 100) / 100;
    const BtState = GamepadButtonsState[k];
    if (Math.abs(BtVal) < 0.1) BtVal = 0;

    if (BtVal) {
      if ((TrainTableNum > 4) && 
          ((k == GPBT_TRAIN4_DOWN) || (k == GPBT_TRAIN4_UP))) {
        var Value = parseInt(MotorSlider[4].value);

        BtVal = Math.round(BtVal * GPAX_SEMSIBILITY);
        if (k == GPBT_TRAIN4_DOWN) Value -= BtVal;
        else Value += BtVal;

             if (Value >  255) Value =  255;
        else if (Value < -255) Value = -255;
        MotorSlider[4].value = Value;
        MotorChange(4, false);
      }

      if (!BtState.Pressed) {
        BtState.Pressed   = true;
        BtState.DownAt    = Now;
        BtState.LongPress = false;

        if (DEBUG)
          console.log("Button " + k + " pressed");
      }

      if ((!BtState.LongPress) &&
          (Now - BtState.DownAt >= GPBT_LONGPRESSMS)) {
        BtState.LongPress = true;

        /**** Long press ****/

        switch(k) {
        case GPBT_MUSIC:
          ttsEnable(!setSpeechEn);
          ToastMessage(setSpeechEn ? Msg.MsgVoiceEn : Msg.MsgVoiceDis);
          break;

        case GPBT_CONFIG:
          ToastMessage(Msg.MsgVibrTest);
          GamepadVibration(VIB_BRAKE_DURATION, VIB_BRAKE_STRONGMAG, VIB_BRAKE_WEAKMAG);
          break;

        case GPBT_TRAIN4_STOP_L1:
        case GPBT_TRAIN4_STOP_R1:
          if (TrainTableNum > 4) MotorStop(4, false);
          break;
        } /* End of switch */

        if (DEBUG)
          console.log("Button " + k + " long pressed");
      }
    } else if (BtState.Pressed) {
      if (!BtState.LongPress) {
        switch(k) {
        case GPBT_CONFIG: /* Show the configuration page */
          BT_ConfigClick();
          break;

        case GPBT_HELP:   /* Show the gamepad help */
          GamepadHelp();
          break;

        case GPBT_MUSIC:  /* Toggle the music */
          if (!BtState.LongPress) MusicToggle();
          break;

        default:
          const StopButtonID = GPBT_STOP.indexOf(k);
          if (StopButtonID >= 0) {
            var Idx = 0;
            for(let k = 0; k < TrainNum; ++k) {
              if (TrainTableEn[k]) {
                if (StopButtonID == Idx) {
                  MotorStop(k, false);
                  break;
                }
                ++Idx;
              }
            } /* End of for (k) */
          }
        
          const SoundButtonID = GPBT_SNDBUTTONS.indexOf(k);

          if ((SoundButtonID >= 0) && (SoundButtonID < hSoundButtons.length))
            hSoundButtons[SoundButtonID].click();
        } /* End of switch */

        if ((TrainTableNum > 4) && 
            ((k == GPBT_TRAIN4_DOWN) || (k == GPBT_TRAIN4_UP))) {
          MotorChange(4, true, false);
        }
      }

      if (DEBUG)
        console.log("Button " + k + " released");
      
      BtState.Pressed   = false;
      BtState.DownAt    = null;
      BtState.LongPress = false;
    }
  } /* End of for (k) */


  /**** Axis ****/

  const AxisPri = [ 1, 3, 0, 2 ];

  for(let Axis = 0; Axis < GamepadAxes; ++Axis) {
    var AxisVal = Math.round((Math.round((GamepadHandle.axes[Axis] + Number.EPSILON) / 0.05) * 0.05) * 100) / 100;
    if (Math.abs(AxisVal) < 0.1) AxisVal = 0;
    if (AxisVal) {
      var Idx                = 0;
      GamepadAxesState[Axis] = AxisVal;
      AxisVal                = Math.round(AxisVal * GPAX_SEMSIBILITY);

      for(let i = 0; (i < TrainNum) && (Idx < AxisPri.length); ++i) { 
        if (TrainTableEn[i]) {
          if (Axis == AxisPri[Idx]) {
            var Value = parseInt(MotorSlider[i].value);

            if (AxisPri[Idx] & 1) Value -= AxisVal;
            else Value += AxisVal;

                 if (Value >  255) Value =  255;
            else if (Value < -255) Value = -255;
            MotorSlider[i].value = Value;
            MotorChange(i, false);
          } 
          ++Idx;
        }
      } /* End of for (i) */

      if (DEBUG)
        console.log("Axis " + Axis + " changed: " + AxisVal);
    } else if (GamepadAxesState[Axis]) {
      GamepadAxesState[Axis] = 0;
      MotorChange(AxisPri.indexOf(Axis), true, false);
    }
  } /* End of for (k) */
}


/**** Send to the server ****/

function HttpSend(Key, Val, Host = "")
{
  const xHttp = new XMLHttpRequest();


  function EndLoading() {
    hPageLoading.classList.add("hide");
    document.getElementById("page_main").classList.add("show");

    setTimeout(() => {
      hPageLoading.style.display = "none";
    }, 1500);
  }

  xHttp.onreadystatechange = function() {
    if (this.readyState === 4) {
      if (this.status === 200) {
        try {
          const JsonReply = JSON.parse(this.responseText);

          if ((hCapacityValue) && (JsonReply.Battery !== undefined)) {
            if (JsonReply.Battery == '0') 
              document.getElementById("grp_batterybox").style.display = "none";
            else
              hCapacityValue.innerHTML = JsonReply.Battery;
          }
          
          if ((hTitle) && (JsonReply.HostName !== undefined)) {
            hTitle.innerHTML = JsonReply.HostName;
            document.title = JsonReply.HostName;
          }

          if ((hVersionValue) && (JsonReply.Version !== undefined))
            hVersionValue.innerHTML = JsonReply.Version;

          for(let i = 0; i < TrainNum; i++) {
            const MotorID = "Motor" + i;
            const TrainID = "Train" + i;

            if (MotorSlider[i] && JsonReply[MotorID] !== undefined)
              MotorSlider[i].value = JsonReply[MotorID];

            if (TrainName[i] && JsonReply[TrainID + "Name"] !== undefined)
              TrainName[i].innerHTML = JsonReply[TrainID + "Name"];

            if (TrainTable[i] && JsonReply[TrainID + "Enabled"] !== undefined) {
              TrainTableEn[i]             = JsonReply[TrainID + "Enabled"];
              TrainTable[i].style.display = TrainTableEn[i] ? "table" : "none";
              //TrainTable[i].style.display = JsonReply[TrainID + "Enabled"] ? "table" : "none";
            }
          } /* End of for (i) */

          TrainTableNum = TrainTableEn.filter(Boolean).length;
        } catch (e) {
          console.error("Error parsing JSON:", e, this.responseText);
        }
      } else {
        console.error("Error HTTP:", this.status, this.responseText);
      }

      EndLoading();
    }
  };

  var Url = "exec.cgi?";
  if (Host !== "") Url = "http://" + Host + "/" + Url;

  xHttp.open("GET", Url + Key + "=" + Val, true);
  xHttp.send();
}


/**** Change the motor speed ****/

function MotorChange(Channel, Send, Vibr = true)
{
  if (MotorDebounceTimer[Channel])
    clearTimeout(MotorDebounceTimer[Channel]);

  var Val = parseInt(MotorSlider[Channel].value, 10);
  MotorValue[Channel].style.opacity = 0.3;

  MotorDebounceTimer[Channel] = setTimeout(function () {
    if ((Val > -10) && (Val < 10)) {
      MotorValue[Channel].innerHTML = Msg.SpStop;
      Val = 0;
    } else {
      MotorValue[Channel].innerHTML =
        (Val > 0 ? Msg.SpForward : Msg.SpReverse) +
        Math.round(Math.abs((100 * Val / 255))) + "%";
    }

    MotorValue[Channel].style.opacity = 100;

    /**** Vibration ****/

    if (setVibrationEn) {
      let DeltaSpeed = Val - MotorSpeedLast[Channel];

      if (DeltaSpeed) {
        var Coeff;

        if (Math.abs(DeltaSpeed) > 255) {
          Coeff = VIB_ACCEL_DURATION + VIB_BRAKE_DURATION; 
        } else if ((Val >= 0) && (MotorSpeedLast[Channel] >= 0)) {
          Coeff = (DeltaSpeed < 0) ? VIB_BRAKE_DURATION : VIB_ACCEL_DURATION;
        } else { 
          Coeff = (DeltaSpeed < 0) ? VIB_ACCEL_DURATION : VIB_BRAKE_DURATION;
        }
      
        let VibDur = Math.round(Math.abs(DeltaSpeed) / 255 * Coeff);
        if (Vibr) {
          VibrationSingle(VibDur);
        } else {
          let StrongMag = Coeff >= VIB_BRAKE_DURATION ? VIB_BRAKE_STRONGMAG: VIB_ACCEL_STRONGMAG;
          let WeakMag   = Coeff >= VIB_BRAKE_DURATION ? VIB_BRAKE_WEAKMAG  : VIB_ACCEL_WEAKMAG;
          GamepadVibration(VibDur, StrongMag, WeakMag);
        }
      }
    }

    /**** Send the motor speed ****/

    if (Send)
      HttpSend("Motor" + TrainChannelMap[Channel], Val, TrainHost[Channel]);

    MotorSpeedLast[Channel]     = Val;
    MotorDebounceTimer[Channel] = null;
  }, MotorDebounce);
}


/**** Stop the motor ****/

function MotorStop(Channel, Vibr = true)
{
  MotorSlider[Channel].value = 0;
  MotorChange(Channel, true, Vibr);
}


/**** Setup ****/

async function Setup()
{
  hBtAdd         = document.getElementById("bt_add");
  hBtMusic       = document.getElementById("bt_music");
  hBtMusicImg    = document.getElementById("bt_musicimg");
  hCapacityValue = document.getElementById("str_capacity");
  hPageLoading   = document.getElementById("page_loading");
  hSoundButtons  = document.querySelectorAll('[id^="bt_snd"]');
  hTitle         = document.getElementById("lbl_title");
  hVersionValue  = document.getElementById("str_version");

  for(let k = 0; k < TrainNum; ++k)
    AddTrainData(k, k);

  document.querySelector('#year').textContent = Year;

  /**** Translate the labels ****/

  hBtMusic.title = Msg.BtMusic;

  TransInner("lbl_battery", Msg.LblBattery);
  TransInner("lbl_battery", Msg.LblBattery);
  TransInner("lbl_wait"   , Msg.LblLoading);
  TransTitle("bt_config"  , Msg.BtConfig  );

  for(let k = 0; k < TrainNum; ++k)
    TransTitle("bt_trainstop" + k, Msg.BtStop);

  hPageLoading.classList.add("show");

  /**** Image preload ****/

  const ImgPreload = new Image();
  ImgPreload.src = "img/music_on.svg";

  for(let k = 0; k < TrainNum; ++k)
    MotorValue[k].innerHTML = Msg.SpStop;

  /**** Gamepad connected event ****/

  window.addEventListener("gamepadconnected", e => {
    GamepadIndex        = e.gamepad.index;
    const GamepadHandle = navigator.getGamepads()[GamepadIndex];
    GamepadAxes         = GamepadHandle.axes.length;
    GamepadButtons      = GamepadHandle.buttons.length;

    GamepadAxesState    = new Array(GamepadAxes).fill(0);

    for(let k = 0; k < GamepadButtons; ++k) {
      GamepadButtonsState[k] = {
        Pressed: false,
        DownAt: null,
        LongPressed: false
      };
    } /* End of for (k) */

    if (!GamepadPollTimer)
      GamepadPollTimer = setInterval(GamepadPoll, GamepadPollInter);

    GamepadVibAvail = (GamepadHandle?.vibrationActuator !== undefined) && (GamepadHandle.vibrationActuator !== null);
    ToastMessage(Msg.MsgGamepadConn);
  });

  /**** Gamepad disconnected event ****/

  window.addEventListener("gamepaddisconnected", () => {
    clearInterval(GamepadPollTimer);
    GamepadPollTimer = null;
    GamepadIndex     = null;
    ToastMessage(Msg.MsgGamepadDisconn);
  });
  
  HttpSend("Refresh", 1);
}
