#!/usr/bin/python3

#*****************************************
#**** TrenoLEGO - Html bundle creator ****
#****  (c) 2026, Alessandro Pedretti  ****
#*****************************************


# Dependencies:
# pip install beautifulsoup4 minify-html lxml rjsmin

# For Crostini/Chromebook
# pip3 install --break-system-packages beautifulsoup4 minify-html lxml rjsmin

import os
import re
import gzip
import shutil
import base64
import argparse
import minify_html
from bs4 import BeautifulSoup, Comment
from rjsmin import jsmin


#**** File to exclude ****

EXCLUDE_FILES = ['settings.js']


#**** Create the bundle ****

def bundle_web(input_html, output_dir_raw, use_gzip, minify):
  
  output_dir = os.path.abspath(output_dir_raw)
  if not os.path.exists(output_dir):
    os.makedirs(output_dir)

  base_dir = os.path.dirname(os.path.abspath(input_html))
  original_filename = os.path.basename(input_html)
  base_name = os.path.splitext(original_filename)[0].replace('_src', '')
  output_path = os.path.join(output_dir, f"{base_name}.html")

  with open(input_html, 'r', encoding='utf-8', errors='ignore') as f:
    soup = BeautifulSoup(f, 'html.parser')

  print(f"* Processing {original_filename}")

  #**** Image and sound mapping ****

  img_replacements = {}
  for folder in ['img', 'snd']:
    path = os.path.join(base_dir, folder)
    if os.path.exists(path):
      for file in os.listdir(path):
        if file.endswith(('.svg', '.png', '.ico', '.jpg', '.jpeg')):
          full_path = os.path.join(path, file)
          rel_path = f"{folder}/{file}"
          try:
            with open(full_path, 'rb') as f_img:
              b64 = base64.b64encode(f_img.read()).decode('utf-8')
              ext = file.split('.')[-1].lower()
              mime = "image/svg+xml" if ext == 'svg' else f"image/{ext}"
              if ext == 'ico': mime = "image/x-icon"
              img_replacements[rel_path] = f"data:{mime};base64,{b64}"
          except Exception as e:
            print(f"  ERROR imagine {rel_path}: {e}")

  #**** Favicon ****

  for link in soup.find_all('link', rel=re.compile(r'icon|shortcut')):
    href = link.get('href')
    if href in img_replacements:
      link['href'] = img_replacements[href]
      print(f"  Favicon converted: {href}")

  #**** Sidebar ****

  if base_name.lower() == "index":
    sidebar = soup.find(id="sidebar")
    snd_dir = os.path.join(base_dir, 'snd')
    if sidebar and os.path.exists(snd_dir):
      mp3_files = sorted([f for f in os.listdir(snd_dir) if f.endswith('.mp3')])
      for i, f in enumerate(mp3_files, 1):
        snd_id = f"snd{i}"
        snd_base = os.path.splitext(f)[0]
        sidebar.append(soup.new_tag('audio', id=snd_id, src=f"snd/{f}", preload="none"))
        btn = soup.new_tag('button', id=f"bt_{snd_id}", attrs={"class":"button_snd", "onclick":f"document.getElementById('{snd_id}').play();"})
        btn.append(soup.new_tag('img', attrs={"class":"button_sndimg", "src":f"snd/{snd_base}.svg"}))
        sidebar.append(btn)
      print(f"  Sidebar completed: {len(mp3_files)} suonds")

  #**** Inlining CSS and JS ****

  for tag in soup.find_all(['link', 'script']):
    attr          = 'href' if tag.name == 'link' and tag.get('rel') == ['stylesheet'] else 'src'
    file_rel_path = tag.get(attr)
    
    if file_rel_path:
      if tag[attr].startswith(('http', 'data:')): continue

      #**** Skip the files included in EXCLUDE_FILES ****

      file_name = os.path.basename(file_rel_path)
      if file_name in EXCLUDE_FILES:
        print(f"  Skipped dynamic file: {file_name}")
        continue

      file_p = os.path.normpath(os.path.join(base_dir, tag[attr]))
      if os.path.exists(file_p):
        try:
          with open(file_p, 'r', encoding='utf-8', errors='ignore') as f_in:
            content = f_in.read()

            if tag.name == 'script':
              content = jsmin(content)
            
            for rel_p, b64_p in img_replacements.items():
              content = content.replace(f'"{rel_p}"', f'"{b64_p}"')
              content = content.replace(f"'{rel_p}'", f"'{b64_p}'")
            new_tag = soup.new_tag('style' if tag.name == 'link' else 'script')
            new_tag.string = content
            tag.replace_with(new_tag)
            print(f"  Inlined: {tag[attr]}")
        except Exception as e:
          print(f"  [!] Errore in {tag[attr]}: {e}")

  #**** Replace img file sources with base64 ****

  for img in soup.find_all('img'):
    src = img.get('src')
    if src in img_replacements:
      img['src'] = img_replacements[src]

  #**** Minify ****

  if minify:
    print("* Minifying")

    for comment in soup.find_all(string=lambda text: isinstance(text, Comment)):
      comment.extract()
    try:
      final_html = minify_html.minify(str(soup), minify_css=True, minify_js=False)
    except:
      final_html = str(soup)
  else:
    final_html = str(soup)

  #**** Save the bundle ****
  
  with open(output_path, 'w', encoding='utf-8') as f:
    f.write(final_html)
  
  print(f"* Saved file: {output_path}")

  #**** GZip compression ****

  if use_gzip:
    gz_path = output_path + ".gz"
    with open(output_path, 'rb') as f_in, gzip.open(gz_path, 'wb', compresslevel=9) as f_out:
      shutil.copyfileobj(f_in, f_out)
    print(f"* Compressed file: {os.path.basename(gz_path)}")

  print("* Done\n");

#**** Main code ****

if __name__ == "__main__":
  parser = argparse.ArgumentParser(description="TrenoLEGO Web Bundle Builder")
  parser.add_argument("input", help="HTML input file (e.g. index.html)")
  parser.add_argument("-o", "--output", default="data", help="Output directory (default: data)")
  parser.add_argument("-m", "--minify", action="store_true", help="Enable the minification")
  parser.add_argument("-g", "--gzip", action="store_true", help="Enable the Gzip compression (.gz)")

  args = parser.parse_args()
  bundle_web(args.input, args.output, args.gzip, args.minify)
