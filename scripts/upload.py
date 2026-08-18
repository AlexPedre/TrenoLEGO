#!/usr/bin/python3

# *******************************************
# **** TrenoLEGO - Transfer files to MCU ****
# ****   (c) 2026, Alessandro Pedretti   ****
# *******************************************


from ftplib import FTP, error_perm
from pathlib import Path
from credentials import HostName, UserName, Password


BASE_DIR = Path(__file__).resolve().parent
LOCAL_ROOT = BASE_DIR / ".." / "data" / "Html"


#**** Create a directory ****

def FtpMkdir(ftp: FTP, dirname: str) -> None:
  try:
    ftp.mkd(dirname)
    print(f"  Directory {dirname} created")
  except error_perm:
    print(f"  Directory {dirname} already present")


#**** Upload a file ****

def FtpUpload(ftp: FTP, local_file: Path) -> None:
  with local_file.open("rb") as f:
    ftp.storbinary(f"STOR {local_file.name}", f)
  print(f"  File {local_file} uploaded")


#**** Upload all files ****

def FtpUploadAll(ftp: FTP, local_dir: Path) -> None:
  for item in sorted(local_dir.iterdir()):
    if item.is_file():
      FtpUpload(ftp, item)


#**** Main code ****

def main() -> None:
  if not LOCAL_ROOT.exists():
    raise FileNotFoundError(f"Directory not found: {LOCAL_ROOT}")

  try:
    print(f"* Connecting to {HostName}...")
    with FTP() as ftp:
      ftp.connect(HostName, 21, timeout=20)
      ftp.login(UserName, Password)

      ftp.set_pasv(True)
      ftp.voidcmd("TYPE I")

      FtpMkdir(ftp, "Html")
      ftp.cwd("Html")

      for dirname in ["img", "js", "mus", "snd"]:
        FtpMkdir(ftp, dirname)

      for filename in [
        "config.html.gz",
        "err_not_found.html",
        "index.html.gz",
        "suspended.html.gz",
        "update.html.gz",
        "updating.html.gz",
      ]:
        FtpUpload(ftp, LOCAL_ROOT / filename)

      ftp.cwd("img")
      FtpUploadAll(ftp, LOCAL_ROOT / "img")

      ftp.cwd("../js")
      FtpUploadAll(ftp, LOCAL_ROOT / "js")

      ftp.cwd("../mus")
      FtpUploadAll(ftp, LOCAL_ROOT / "mus")

      ftp.cwd("../snd")
      FtpUploadAll(ftp, LOCAL_ROOT / "snd")

      ftp.quit()

    print("* Done")

  except:
    print("ERROR: Can't connect. Check the HostName in credentials.py file.");


if __name__ == "__main__":
    main()
