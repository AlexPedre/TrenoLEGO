
/**************************************
****     TrenoLEGO - Utilities     ****
**** (c) 2025, Alessandro Pedretti ****
**************************************/


/**** Global variables ****/

const TrainNum  = 5;
const Year      = new Date().getFullYear();
const Lang      = navigator.language.substring(0, 2);


/**** Ask ****/

function Ask(Msg)
{
  if (ttsAvailable) ttsSpeak(Msg);
  
  return confirm(Msg);
}


/**** Check if a string is avalid domain ****/

function IsDomainName(Name) 
{
//  const Label = '[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?';
//  const Re    = new RegExp(`^(?:${Label})(?:\\.(?:${Label}))*$`);
  const Label = '[A-Za-z0-9](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?';
  const TLD   = '[A-Za-z](?:[A-Za-z0-9-]{0,61}[A-Za-z0-9])?';
  const Re    = new RegExp(`^(?:${Label}\\.)+${TLD}$`);
  
  return Re.test(Name);
}


/**** Check if a string is a valid IP ****/

function IsIP(Ip) 
{
  const Octet = '(?:25[0-5]|2[0-4]\\d|1\\d{2}|[1-9]?\\d)';
  const Re    = new RegExp(`^(?:${Octet}\\.){3}${Octet}$`);

  return Re.test(Ip);
}


/**** Check if a string is a valid HostName ****/

function IsHostName(Name) 
{
  const Re = /^[A-Za-z0-9](?:[A-Za-z0-9-]*[A-Za-z0-9])?$/;

  return Re.test(Name);
}


/**** Check if a string is a valid Posix timezone ****/

function IsPosixTZ(Tz)
{
  const Re = /^[A-Za-z]{3,}[-+]?\d{1,2}(?:[A-Za-z]{3,})?,M\d{1,2}\.[1-5]\.[0-6](?:\/\d{1,2})?,M\d{1,2}\.[1-5]\.[0-6](?:\/\d{1,2})?$/;
  
  return Re.test(Tz);
}


/**** Load an external JavaSCript ****/

function LoadScript(src) 
{
  return new Promise((resolve, reject) => {
    const script = document.createElement('script');
    script.src   = src;
    script.type  = 'text/javascript';
    script.async = false; /* Keep the order! */

    script.onload  = () => resolve();
    script.onerror = () => reject(new Error(`Error loading ${src}`));

    document.head.appendChild(script);
  });
}


/**** Set new location ****/

function Location(Url, Text)
{
  if ((Text !== undefined) && (Text !== "") && (ttsAvailable)) {
    
    ttsSpeak(Text, function () {
      location.href = Url;
    });
    return;
  }

  location.href = Url;  
}


/**** Error message ****/

function ShowError(Err)
{
  if (ttsAvailable) ttsSpeak(Err);
  else alert(Err);
}


/**** Translate textContent ****/

function TransConte(Id, Str)
{
  try {
    document.getElementById(Id).textContent = Str;
  } catch {
    console.error("Can't translate " + Id);
  }
}


/**** Translate innerHTML ****/

function TransInner(Id, Str)
{
  try {
    document.getElementById(Id).innerHTML = Str;
  } catch {
    console.error("Can't translate " + Id);
  }
}


/**** Translate title ****/

function TransTitle(Id, Str)
{
  try {
    document.getElementById(Id).title = Str;
  } catch {
    console.error("Can't translate " + Id);
  }
}