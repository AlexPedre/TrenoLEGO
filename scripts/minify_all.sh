#!/usr/bin/bash

#***************************************
#**** TrenoLEGO - File minification ****
#**** (c) 2026, Alessandro Pedretti ****
#***************************************


echo "* Minification of the HTML pages"

SRC_DIR=../html
DEST_DIR=../data/Html

python3 minify.py -o $DEST_DIR/config.html $SRC_DIR/config.html
python3 minify.py -o $DEST_DIR/err_not_found.html $SRC_DIR/err_not_found.html
python3 minify.py -o $DEST_DIR/index.html $SRC_DIR/index.html
python3 minify.py -o $DEST_DIR/sidebar.html $SRC_DIR/sidebar.html
python3 minify.py -o $DEST_DIR/suspended.html $SRC_DIR/suspended.html
python3 minify.py -o $DEST_DIR/update.html $SRC_DIR/update.html
python3 minify.py -o $DEST_DIR/updating.html $SRC_DIR/updating.html

echo "* Minification of the stylesheets"

SRC_DIR=../html/css
DEST_DIR=../data/Html/css

python3 minify.py -o $DEST_DIR/trenolego.css $SRC_DIR/trenolego.css

echo "* Minification of the javascripts"

SRC_DIR=../html/js
DEST_DIR=../data/Html/js

python3 minify.py -o $DEST_DIR/config.js   $SRC_DIR/config.js
python3 minify.py -o $DEST_DIR/index.js    $SRC_DIR/index.js
python3 minify.py -o $DEST_DIR/music.js    $SRC_DIR/music.js
python3 minify.py -o $DEST_DIR/update.js   $SRC_DIR/update.js
python3 minify.py -o $DEST_DIR/utils.js    $SRC_DIR/utils.js
python3 minify.py -o $DEST_DIR/voice.js    $SRC_DIR/voice.js

echo "* Done"