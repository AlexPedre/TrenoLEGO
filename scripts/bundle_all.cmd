@echo off

REM ***************************************
REM ****   TrenoLEGO - Bundle builder  ****
REM **** (c) 2026, Alessandro Pedretti ****
REM ***************************************


echo * Building the HTML bundles

set SRC_DIR=..\html
set DEST_DIR=..\data\Html
set OPTIONS=-m -g

python bundlehtml.py "%SRC_DIR%/config.html"        -o "%DEST_DIR%" %OPTIONS%
python bundlehtml.py "%SRC_DIR%/err_not_found.html" -o "%DEST_DIR%" %OPTIONS%
python bundlehtml.py "%SRC_DIR%/index.html"         -o "%DEST_DIR%" %OPTIONS%
python bundlehtml.py "%SRC_DIR%/suspended.html"     -o "%DEST_DIR%" %OPTIONS%
python bundlehtml.py "%SRC_DIR%/update.html"        -o "%DEST_DIR%" %OPTIONS%
python bundlehtml.py "%SRC_DIR%/updating.html"      -o "%DEST_DIR%" %OPTIONS%

echo * Done

pause
