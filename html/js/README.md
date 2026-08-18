# *html/js* directory
JavaScript directory. It contains JavaScript used in the Html pages of *TrenoLEGO* graphic interface:
- *config.js*  
  Common configuration file, including localization strings.
- *index.js*  
  JavaScript for *index.html*.
- *manifest.js*  
  Manifest for WebApp installation.
- *music.js*  
  Routines to play music from different sources (*MIDI*, *Mp3* and several *Mod* formats) stored in the ESP file system (*MIDI*)
  or on-line (Mp3/Mod).
- *settings.js*  
  This file is not needed by ESP, because it is generated dinamically, but it is required to debug the *Html* pages locally with
  the Python Web server (see *scripts/httptest.cmd* and *scripts/httptest.sh*).
- *toastpopup.js*  
  Routines for popups.
- *update.js*  
  JavaScript for *update.html*.
- *utils.js*  
  Common utilities.
- *vibration.js*  
  Joypad/phone aptic/vibration routines.
- *voice.js*  
  TTS and STT rouines. STT not yet implemented, but still available by *Alexa Hue emulation*.
