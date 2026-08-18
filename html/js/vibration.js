
/**************************************
****     TrenoLEGO - Vibration     ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Global variables ****/

var   VibrationAvailable = ("vibrate" in navigator)  && (setVibrationEn);


/**** Gamepad vibration ****/

function GamepadVibration(Duration, StrongMag, WeakMag)
{
  if ((!setVibrationEn) || (!GamepadVibAvail)) return;
  
  navigator.getGamepads()[GamepadIndex].vibrationActuator.playEffect(
    "dual-rumble", {
      duration: Duration,
      strongMagnitude: StrongMag,
      weakMagnitude: WeakMag
    });  
}


/**** Enable/disable TTS ****/

function VibrationEnable(En)
{
  setVibrationEn = En;

  if (En) {
    VibrationAvailable = ("vibrate" in navigator)  && (En);
    return VibrationAvailable;
  }

  VibrationAvailable = false;

  return false;
}


/**** Multiple vibrations ****/

function VibrationMultiple(Duration, AmountOfVibs, PauseLength)
{
  if (!VibrationAvailable) return;

  let VibArray = [];
  
  for(let i = 0; i < AmountOfVibs; i++) {
    VibArray.push(Duration);
    VibArray.push(PauseLength);
  } /* End of for (i) */
  
  navigator.vibrate(VibArray);
}


/**** Single vibration ****/

function VibrationSingle(Duration)
{
  if ((!VibrationAvailable) || (Duration <= 0))
    return;  

  navigator.vibrate(Duration);
}


/**** Vibration stop ****/

function VibrationStop()
{
  if (VibrationAvailable)
    navigator.vibrate(0);
}
