#!/bin/bash

#******************************************
#**** TrenoLEGO - Tansfer files to MCU ****
#****  (c) 2026, Alessandro Pedretti   ****
#******************************************


source credentials.sh

cd ../data/Html
ftp -in <<EOF
open $HostName
user $UserName $Password
bin
mkdir Html
cd Html
mkdir img
mkdir js
mkdir mus
mkdir snd

put config.html.gz
put err_not_found.html
put index.html.gz
put suspended.html.gz
put update.html.gz
put updating.html.gz

cd img
lcd img
mput *

cd ../js
lcd ../js

cd ../mus
lcd ../mus
mput *

cd ../snd
lcd ../snd
mput *

close
bye
EOF
