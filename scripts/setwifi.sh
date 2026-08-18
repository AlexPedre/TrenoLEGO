#!/bin/bash

#*****************************************
#**** TrenoLEGO - Set the WiFi of MCU ****
#**** (c) 2026, Alessandro Pedretti   **** 
#*****************************************


source credentials.sh

if [ "$#" -ne 2 ]; then
  echo "Usage: $0 <SSID> <WIFI_PASSWORD>"
  exit 1
fi

SSID="$1"
WifiPasswd="$2"

if [ -z "$SSID" ]; then
  echo "ERROR: Invalid SSID."
  exit 1
fi

if [ -z "$WifiPasswd" ]; then
  echo "ERROR: Invalid WiFi password."
  exit 1
fi

if [ "${#WifiPasswd}" -lt 8 ] || [ "${#WifiPasswd}" -gt 63 ]; then
  echo "ERROR: The WiFi password length must have from 8 to 63 characters."
  exit 1
fi

echo "* Setting the WiFi (SSID: $SSID)"
{ 
  sleep 1; echo "$UserName"; 
  sleep 1; echo "$Password";
  sleep 1; echo "set ssid $SSID";
  sleep 1; echo "set wifipwd $WifiPasswd";
  sleep 1; echo "save";
  sleep 1; echo "reboot";
  sleep 1; 
} | telnet $HostName > /dev/null 2>&1

if [ $? -ne 0 ]; then
  echo "ERROR: Unable to set the WiFi configuration."
  exit 1
fi

echo "* Done"
