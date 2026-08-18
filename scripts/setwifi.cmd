@echo off

REM *****************************************
REM **** TrenoLEGO - Set the WiFi of MCU ****
REM **** (c) 2026, Alessandro Pedretti   ****
REM *****************************************


call credentials.cmd

if "%~2"=="" (
  echo Usage: %~nx0 ^<SSID^> ^<WIFI_PASSWORD^>
  exit /b 1
)

if not "%~3"=="" (
  echo Usage: %~nx0 ^<SSID^> ^<WIFI_PASSWORD^>
  exit /b 1
)

set SSID=%~1
set WifiPasswd=%~2

if "%SSID%"=="" (
  echo ERROR: Invalid SSID.
  exit /b 1
)

if "%WifiPasswd%"=="" (
  echo ERROR: Invalid WiFi password.
  exit /b 1
)

call :strlen WifiPasswd PASSLEN
if %PASSLEN% LSS 8 (
  echo ERROR: The WiFi password length must have from 8 to 63 characters.
  exit /b 1
)
if %PASSLEN% GTR 63 (
  echo ERROR: The WiFi password length must have from 8 to 63 characters.
  exit /b 1
)

echo * Setting the WiFi (SSID: %SSID%)

(
echo %UserName%
timeout /t 1 /nobreak > nul
echo %Password%
timeout /t 1 /nobreak > nul
echo set ssid %SSID%
timeout /t 1 /nobreak > nul
echo set wifipwd %WifiPasswd%
timeout /t 1 /nobreak > nul
echo save
timeout /t 1 /nobreak > nul
) | telnet %HostName% > nul 2>&1

if errorlevel 1 (
  echo ERROR: Unable to set the WiFi configuration.
  del "%TELNET_SCRIPT%"
  exit /b 1
)

echo * Done
exit /b 0


REM **** Calculate the string length ****

:strlen
setlocal EnableDelayedExpansion
set "s=!%1!"
set len=0
:loop
if defined s (
  set "s=!s:~1!"
  set /a len+=1
  goto loop
)
endlocal & set "%2=%len%"
exit /b
