#!/usr/bin/python3

#***************************************
#**** TrenoLEGO - File minification ****
#**** (c) 2026, Alessandro Pedretti ****
#***************************************


# Dependencies:
# pip install minify-html
# pip install rcssmin
# pip install rjsmin

# For Crostini/Chromebook
# python3 -m pip install minify-html --break-system-packages
# python3 -m pip install rcssmin --break-system-packages
# python3 -m pip install rjsmin --break-system-packages


import argparse, sys
import minify_html
import rcssmin
import rjsmin

from pathlib import Path

p = argparse.ArgumentParser()
p.add_argument("input", help="Input file (use - for stdin)")
p.add_argument("-o", "--output", help="output file (default: stdout)")
args = p.parse_args()

data = sys.stdin.read() if args.input == "-" else open(args.input, "r", encoding="utf-8").read()

Ext  = Path(args.input).suffix
if Ext == ".css":
  out = rcssmin.cssmin(data)
elif Ext == ".html" or Ext == ".htm":
  out = minify_html.minify(data, minify_js=True, minify_css=True)
elif Ext == ".js":
  out = rjsmin.jsmin(data)
else:
  out = data

if args.output:
  open(args.output, "w", encoding="utf-8").write(out)
else:
  sys.stdout.write(out)



