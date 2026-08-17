# Scripts directory
Here you can find the scripts to build Html bundles, test the Html pages, upload the data to ESP, etc.

- **bundle_all.cmd** (Windows version)
- **bundla_all.sh** (Linux version)  
  build the Html bundles from the html directory sources. The output files are saved to data/Html directory.
- **bundlehtml.py**  
  it's the Python 3 script called by *bundle_all.cmd* or *bundle_all.sh* to build the bundles. The required dependencies are:
  - beautifulsoup4
  - minify-html lxml
  - rjsmin

  To install the dependencies:
  ```bash
  pip install beautifulsoup4 minify-html lxml rjsmin
  ```
