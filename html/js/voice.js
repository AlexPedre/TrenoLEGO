
/**************************************
****     TrenoLEGO - STT & TTS     ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Global variables ****/

var   sttHandle, sttOnEvent;

const SpeechRecognition     = window.SpeechRecognition || window.webkitSpeechRecognition;
var   sttAvailable          = (SpeechRecognition != undefined);
var   ttsAvailable          = (("speechSynthesis" in window) && (typeof SpeechSynthesisUtterance !== "undefined") && (setSpeechEn));
const ttsPreferredLanguages = navigator.languages && navigator.languages.length
                              ? navigator.languages
                              : [navigator.language || "en-US"];
var   sttEnabled            = false;
var   sttRunning            = false;
var   ttsVoice              = "";


/**** Enable/disable STT ****/

function sttEnable(En)
{
  if ((!sttAvailable) || (!sttEnabled))
    return;

  if (En) {
    try {
      if (!sttRunning) sttHandle.start();
    } catch { }
  } else {
    if (sttRunning) sttHandle.stop();
  }
}


/**** Enable/disable TTS ****/

function ttsEnable(En)
{
  setSpeechEn = En;

  if (En) {
    ttsAvailable = (("speechSynthesis" in window) && (typeof SpeechSynthesisUtterance !== "undefined") && (En));
    return ttsAvailable;
  }

  ttsAvailable = false;

  return false;
}


/**** Match the language ****/

function ttsLanguageMatchScore(VoiceLang)
{
  if (!VoiceLang) return 0;
  VoiceLang = VoiceLang.toLowerCase();

  for (let i = 0; i < ttsPreferredLanguages.length; i++) {
    const pref = ttsPreferredLanguages[i].toLowerCase();
    if (VoiceLang === pref) return 500 - i * 100;
    if (VoiceLang.startsWith(pref.split("-")[0])) return 250 - i * 100;
  }

  return 0;
}


/**** Set the quality score of the language ***/

function ttsQualityScore(v)
{
  const Name  = (v.name || "").toLowerCase();
  let   Score = ttsLanguageMatchScore(v.lang);

  if (Name.includes("neural"  )) Score += 50;
  if (Name.includes("google"  )) Score += 40;
  if (Name.includes("enhanced")) Score += 30;
  if (Name.includes("premium" )) Score += 20;
  if (v.default                ) Score += 10;

  return Score;
}


/**** Find the best voice ****/

function ttsFindBestVoice()
{
  const Voices = speechSynthesis.getVoices();

  if (!Voices.length) return;

  const Sorted = [...Voices].sort((a,b) => ttsQualityScore(b) - ttsQualityScore(a));

  ttsVoice = Sorted[0];
}


/**** Speak ****/

function ttsSpeak(Text, CallBackAtEnd)
{
  if (!ttsAvailable) return;

  speechSynthesis.cancel();

  const Utterance = new SpeechSynthesisUtterance(Text);
  if (ttsVoice) {
    Utterance.voice = ttsVoice;
    Utterance.lang  = ttsVoice.lang;
  } else {
    Utterance.lang = ttsPreferredLanguages[0];
  }

  Utterance.volume = 1.0;

  Utterance.onstart = function() {
    sttEnable(false);
  };

  Utterance.onend   = function() {
    sttEnable(true);
    if (CallBackAtEnd) CallBackAtEnd();
  };

  speechSynthesis.speak(Utterance);
}


/**** STT initialization ****/

if (sttAvailable) {
  sttHandle                = new SpeechRecognition();
  sttHandle.lang           = ttsPreferredLanguages;
  sttHandle.interimResults = false;
  sttHandle.continuous     = true;

  sttHandle.onend = function() {
    sttRunning = false;
  };

  sttHandle.onresult = function(Event) {
    var FullText = "";

    for(let i = Event.resultIndex; i < Event.results.length; i++) {
      const Res  = Event.results[i];
      const Text = Res[0]?.transcript ?? "";
      if (Res.isFinal)
        FullText += (FullText ? " " : "") + Text.trim();
    } /* End of for (i) */
    if (sttOnEvent) sttOnEvent(FullText);
  };

  sttHandle.onstart = function() {
    sttRunning = true;
  };
}


/**** TTS initialization ****/

if (ttsAvailable) {
  ttsFindBestVoice();
  speechSynthesis.addEventListener("voiceschanged", ttsFindBestVoice);
}

