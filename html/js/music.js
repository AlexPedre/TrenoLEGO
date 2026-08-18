
/**************************************
****       TrenoLEGO - Music       ****
**** (c) 2025, Alessandro Pedretti ****
**************************************/


/**** Music ****/

/**** Mod ****/
/*
const MusicType = 0;
const MusicUrl  = "https://api.modarchive.org/downloads.php?moduleid=41078#effigy.mod";
*/

/**** Mp3 ****/

const MusicType = 1;
const MusicVol  = 0.5;

// Harlequin - Coming To Chimerica by Binster - Original by Barry Leitch (The Jackal)
const MusicUrl  = "https://www.amigaremix.com/download/2771";

// Pinball Dreams
// const MusicUrl  = "https://www.amigaremix.com/download/1949"; 


/**** Midi ****/

/*
const MusicType = 2;
const MusicUrl  = "mus/main.mid"
*/

/*
 **** Other Mod musics ****

 https://api.modarchive.org/downloads.php?moduleid=40475#ELYSIUM.MOD
 https://api.modarchive.org/downloads.php?moduleid=34553#BALLAD3.S3M
 https://api.modarchive.org/downloads.php?moduleid=40340#cyborgjeff-ael.it
 https://api.modarchive.org/downloads.php?moduleid=46272#jos-love.xm
 https://api.modarchive.org/downloads.php?moduleid=40091#ctpnew.xm
 https://api.modarchive.org/downloads.php?moduleid=137376#beerbelly_ii.xm
 https://api.modarchive.org/downloads.php?moduleid=59344#stardstm.mod

*/


/**** Global variables ****/

var AudioPlayer, ChipPlayer, MidiPlayer;

var Music       = false;
var MusicLoaded = false;


/**** Create the Midi player ****/

function CreateMidiPlayer(midiUrl)
{
  if (!midiUrl) return undefined;

  const Player = document.createElement("midi-player");
  Player.src   = midiUrl;
  Player.loop  = true;

  Player.setAttribute("sound-font", "");
  document.body.appendChild(Player);

  return Player;
}


/**** Load ChipTune ****/

async function MusicLoadChiptune()
{
  await LoadScript("//cdn.jsdelivr.net/gh/deskjet/chiptune2.js@master/libopenmpt.js");
  await LoadScript("//cdn.jsdelivr.net/gh/deskjet/chiptune2.js@master/chiptune2.js");
}


/**** Load Midi ****/

async function MusicLoadMidi()
{
  await LoadScript("//cdn.jsdelivr.net/combine/npm/tone@14.7.58,npm/@magenta/music@1.23.1/es6/core.js,npm/html-midi-player@1.5.0");
  await customElements.whenDefined("midi-player");
}


/**** Music ****/

async function MusicToggle()
{
  switch(MusicType) {
  case 0: /* Mod player */
    if (!ChipPlayer) return;

    Music = !Music;
    if ((Music) && (!MusicLoaded)) {
      ChipPlayer.load(MusicUrl, function(Buffer) {
        ChipPlayer.play(Buffer);
        hBtMusic.disabled = true;
        MusicLoaded       = true;
      });
    }
    ChipPlayer.togglePause();
    break;

  case 1: /* Mp3 player */
    if (AudioPlayer === undefined) {
      AudioPlayer        = new Audio(MusicUrl);
      AudioPlayer.loop   = true;
      AudioPlayer.volume = MusicVol;
      MusicLoaded        = true;
    }
    Music = !Music;
    if (AudioPlayer.paused) AudioPlayer.play();
    else AudioPlayer.pause();
    break;

  case 2: /* Midi player */
    if (MidiPlayer === undefined) {
      hBtMusicImg.src                = "img/sleep.svg";
      hBtMusic.style.backgroundColor = "gray";
      hBtMusic.disabled              = true;

      MidiPlayer = CreateMidiPlayer(MusicUrl);

      await new Promise((resolve) =>
        MidiPlayer.addEventListener("load", resolve, { once: true })
      );
      hBtMusic.disabled = false;
    }

    Music = !Music;
    if (Music) {
      if (window.Tone?.start) await Tone.start();
      MidiPlayer.start();
    } else MidiPlayer.stop();
    break;

  default:
    console.error("Unsupported music format");
    return;
  } /* End of switch */

  ttsSpeak(Music ? Msg.BtMusicPlay : Msg.BtMusicStop);
  hBtMusicImg.src                = Music ? "img/music_on.svg" : "img/music_off.svg";
  hBtMusic.style.backgroundColor = Music ? "#0ACD1D"          : "#F9C124";
}


/**** Initialize the libraries ****/

if (MusicType == 0) {
  window['libopenmpt'] = {};
  libopenmpt.locateFile = function (FileName) {
    return "//cdn.jsdelivr.net/gh/deskjet/chiptune2.js@master/" + FileName;
  };

  libopenmpt.onRuntimeInitialized = () => {
    ChipPlayer      = new ChiptuneJsPlayer(new ChiptuneJsConfig(-1));
    document.getElementById("bt_music").visible = true;
  };

  MusicLoadChiptune();
} else if (MusicType == 2) {
  MusicLoadMidi();
}
