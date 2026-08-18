@echo off

REM ***************************************
REM **** TrenoLEGO - File minification ****
REM **** (c) 2026, Alessandro Pedretti ****
REM ***************************************


echo * Minification of the HTML pages

set SRC_DIR=..\html
set DEST_DIR=..\data\Html

python minify.py -o "%DEST_DIR%\config.html"        "%SRC_DIR%\config.html"
python minify.py -o "%DEST_DIR%\err_not_found.html" "%SRC_DIR%\err_not_found.html"
python minify.py -o "%DEST_DIR%\index.html"         "%SRC_DIR%\index.html"
python minify.py -o "%DEST_DIR%\sidebar.html"       "%SRC_DIR%\sidebar.html"
python minify.py -o "%DEST_DIR%\suspended.html"     "%SRC_DIR%\suspended.html"
python minify.py -o "%DEST_DIR%\update.html"        "%SRC_DIR%\update.html"
python minify.py -o "%DEST_DIR%\updating.html"      "%SRC_DIR%\updating.html"

echo * Minification of the stylesheets

set SRC_DIR=..\html\css
set DEST_DIR=..\data\Html\css

python minify.py -o "%DEST_DIR%\trenolego.css" "%SRC_DIR%\trenolego.css"

echo * Minification of the javascripts

set SRC_DIR=..\html\js
set DEST_DIR=..\data\Html\js

python minify.py -o "%DEST_DIR%\config.js"   "%SRC_DIR%\config.js"
python minify.py -o "%DEST_DIR%\index.js"    "%SRC_DIR%\index.js"
python minify.py -o "%DEST_DIR%\update.js"   "%SRC_DIR%\update.js"
python minify.py -o "%DEST_DIR%\utils.js"    "%SRC_DIR%\utils.js"
python minify.py -o "%DEST_DIR%\voice.js"    "%SRC_DIR%\voice.js"

echo * Done

pause
