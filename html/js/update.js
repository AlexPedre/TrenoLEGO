
/**************************************
****       TrenoLEGO - Update      ****
**** (c) 2025, Alessandro Pedretti ****
**************************************/


/**** Localization ****/

if (Lang == 'it') {

  /**** Italian ****/

  Msg = {
    BtConfig              : "Configurazione",
    BtControls            : "Controlli",
    BtFileClick           : "Seleziona il file del firmware",
    BtUpdate              : "Aggiorna",
    BtUpdateClick         : "Aggiornamento in corso...",
    BtUpdateTitle         : "Clicca qui per aggiornare il firmware",
    BtUpdateWait          : "Attendere...",

    ErrFile               : "Seleziona il file del firmware prima di aggiornare.",
    LblTitle              : "Aggiornamento firmware",
    Title                 : "TrenoLEGO - Aggiornamento firmware"
  };

} else {

  /**** Default language (english) ****/

  Msg = {
    BtConfig              : "Configuration",
    BtControls            : "Controls",
    BtFileClick           : "Select the firmware file",
    BtUpdate              : "Update",
    BtUpdateClick         : "Updating the system...",
    BtUpdateTitle         : "Click here to update the firmware",
    BtUpdateWait          : "Wait...",

    ErrFile               : "Please select the firmware file before to update.",
    LblTitle              : "Firmware update",
    Title                 : "TrenoLEGO - Firmware update"
  };
}


/**** Configuration button click ****/

function BT_ConfigClick()
{
  Location("config.html", Msg.BtConfig);
}


/**** Controls button click ****/

function BT_ControlsClick()
{
  Location("index.html", Msg.BtControls);
}


/**** Setup ****/

function Setup() 
{
  InputFile    = document.querySelector('input[type="file"]');
  UpdateButton = document.getElementById("bt_update");

  InputFile.addEventListener('click', function (event) {
    ttsSpeak(Msg.BtFileClick);
  });
  
  UpdateButton.addEventListener('click', function(event) {
    if (InputFile.files.length === 0) {
		  ShowError(Msg.ErrFile);
		  event.preventDefault();
		  return;
    }

		UpdateButton.value = Msg.BtUpdateWait;
    ttsSpeak(Msg.BtUpdateClick);

    setTimeout(function() {
		  UpdateButton.disabled = true;
    }, 50);
  });

  document.querySelector('#year').textContent = Year;

  /**** Translate the labels ****/

  document.title     = Msg.Title;
  UpdateButton.title = Msg.BtUpdateTitle;
  UpdateButton.value = Msg.BtUpdate;
  
  TransTitle("bt_controls", Msg.BtControls);
  TransTitle("bt_config"  , Msg.BtConfig);
  
  TransInner("lbl_title", Msg.LblTitle);
}
