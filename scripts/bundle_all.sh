#!/usr/bin/bash

#***************************************
#****   TrenoLEGO - Bundle builder  ****
#**** (c) 2026, Alessandro Pedretti ****
#***************************************


echo "* Building the HTML bundles"

SRC_DIR=../html
DEST_DIR=../data/Html
OPTIONS="-m -g"

./bundlehtml.py $SRC_DIR/config.html -o $DEST_DIR $OPTIONS
./bundlehtml.py $SRC_DIR/err_not_found.html -o $DEST_DIR $OPTIONS
./bundlehtml.py $SRC_DIR/index.html -o $DEST_DIR $OPTIONS
./bundlehtml.py $SRC_DIR/suspended.html -o $DEST_DIR $OPTIONS
./bundlehtml.py $SRC_DIR/update.html -o $DEST_DIR $OPTIONS
./bundlehtml.py $SRC_DIR/updating.html -o $DEST_DIR $OPTIONS
