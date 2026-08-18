# Scripts directory
Here you can find the scripts to build Html bundles, test the Html pages, upload the data to ESP, etc.

- **bundle_all.cmd** (Windows version)
- **bundle_all.sh** (Linux version)  
  build the Html bundles from the sources in *html* directory. The output files are saved to *data/Html*.
- **bundlehtml.py**  
  it's the Python 3 script called by *bundle_all.cmd* or *bundle_all.sh* to build the bundles. The required dependencies are:
  - beautifulsoup4
  - minify-html lxml
  - rjsmin

  To install the dependencies:
  ```bash
  pip install beautifulsoup4 minify-html lxml rjsmin
  ```
  To install the dependencies on Crostini (ChromeOS):
  ```bash
  pip3 install --break-system-packages beautifulsoup4 minify-html lxml rjsmin
  ```
- **credentials.cmd** (Window version)
- **credentials.py** (Python version)
- **credentials.sh** (Linux version)  
  edit these files to change the credentials (default user name: *admin*, password: *admin*) and host name (default: *TrenoLEGO*) required by *setwifi*, *upload* and *upload_index* scripts.
- **fixsh.sh**  
  runs *chmod 755 \*.sh \*.py* in order to make executable the *.sh and *.py scripts.
- **httptest.cmd** (Windows version)
- **httptest.sh** (Linux version)  
  starts the http server (*server_multi.py*) listening on port 8000 to test the Html pages in *html* directory. Use http://localhost:8000 URL in your browser.
- **httptest-production.cmd** (Windows version)
- **httptest-production.sh** (Linux version)  
  starts the http server (*server_multi.py*) listening on port 8001 to test the Html bundles in *data/Html* directory. Use http://localhost:8001 URL in your browser.
- **minify_all.cmd** (Windows version)
- **minify_all.sh** (Linux version)
  minifies all Html files. No more needed, because the minification is done on-the-fly when the bundle are built.
- **minify.py**
  Python 3 script called by y *minify_all.cmd* or *minify_all.sh*. The required dependencies are:
  - minify-html
  - rcssmin
  - rjsmin
  
  To install the dependencies:
  ```bash
  pip install minify-html rcssmin rjsmin
  ```
  To install the dependencies on Crostini (ChromeOS):
  ```bash
  pip3 install --break-system-packages minify-html rcssmin rjsmin
  ```
- **server_multi.py**  
  it's the Python 3 web server used by *httptest.\** and *httptest-production.\** scripts.
- **setwifi.cmd** (Windows version)
- **setwifi.sh** (Linux version)  
  set the WiFi *ssid* and *password* by telnet (telnet.exe command is not installed by default on Windows). To use these scripts, you must connect your PC to the *TrenoLEGO* WiFi (no autentication is required) and the telnet service must be enabled on the ESP.  
  Usage for Linux:
  ```bash
  ./setwifi.sh <SSID> <PASSWORD>
  ```
  Usage for Windows:
  ```cmd.exe
  setwifi.cmd <SSID> <PASSWORD>
  ```
- **upload.py**  
  since Windows *ftp* command doesn't support the passive mode required by the ftp server implemented in *TrenoLEGO*, a Python 3 script was devoleped to upload all bundles and media files to the ESP file system. User name and password must be defined in *credentials.py* file. This script can be used both on Linux and Windows.
- **upload.sh** (Linux version)  
  uploads all bundles and media files to the ESP file system, by using the *ftp* command. User name and password must be defined in *credentials.sh*
- **upload_index.sh** (Linux version)
  uploads only the *index.html.gz* file. This script is useful for debug. 
