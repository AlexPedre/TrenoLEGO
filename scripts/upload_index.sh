#!/bin/bash

#**************************************************
#**** TrenoLEGO - Tansfer index.html.gz to MCU ****
#****      (c) 2026, Alessandro Pedretti       ****
#**************************************************


source credentials.sh

cd ../data/Html
ftp -in <<EOF
open $HostName
user $UserName $Password
bin
cd Html

put index.html.gz

close
bye
EOF
