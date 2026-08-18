# *html* directory
This directory includes the sources for bundles and media. If you want to modify the GUI and the media, edit the files in this
directory tree, than build the bundles with *scripts/bundle_all.cmd* or *scripts/bundle_all.sh*.
Here is the directory tree description:

- **/**  
root of the directory. It contains both gzipped- and not-gizziped- bundles. Only the former must be copyed into the ESP file system, while the latter must be considered temporary files.
- **img**  
  image directory. It includes the images that cannot bundled as the application icon.
- **js**  
  JavaScript library directory. In the future, it will contain JavaScript libraries used to extend the features of TrenoLEGO. Currently, it contains only the Acorn library (not yet in use), which is intended to support the implementation of scripting capabilities.
- **mus**  
  music directory. It contains MIDI files.
- **snd**  
sound effect directory. It includes the sound effects in mp3 format (VBR, 48KHz, mono, Audacity VBR Quality 9).
