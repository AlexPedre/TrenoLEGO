
/**************************************
****    TrenoLEGO - FTP server     ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** FTP callback ****/

void FtpCallback(FtpOperation ftpOperation, uint32_t freeSpace, uint32_t totalSpace)
{
  LastCmdTime = millis();
  if (!Prefs.Debug) return;

  switch (ftpOperation) {
    case FTP_CONNECT:
      Print(F("[DEBUG] FTP: Connected!\r\n"));
      break;

    case FTP_DISCONNECT:
      Print(F("[DEBUG] FTP: Disconnected!\r\n"));
      break;

    case FTP_FREE_SPACE_CHANGE:
      Printf(F("[DEBUG] FTP: Free space change, free %u of %u!\r\n"), freeSpace, totalSpace);
      break;

    default:
      break;
  } /* End of switch */
}


/**** FTP transfer callback ****/

void FtpTransferCallback(FtpTransferOperation FtpOperation, const char *Name, uint32_t TransferredSize)
{
  LastCmdTime = millis();
  if (!Prefs.Debug) return;

  switch (FtpOperation) {
    case FTP_UPLOAD_START:
      Print(F("[DEBUG] FTP: Upload start!\r\n"));
      break;

    case FTP_UPLOAD:
      Printf(F("[DEBUG] FTP: Upload of file %s byte %u\r\n"), Name, TransferredSize);
      break;

    case FTP_TRANSFER_STOP:
      Print(F("[DEBUG] FTP: Finish transfer!\r\n"));
      break;

    case FTP_TRANSFER_ERROR:
      Print(F("[DEBUG] FTP: Transfer error!\r\n"));
      break;

    default:
      break;
  } /* End of switch */
}


/**** FTP setup ****/

void FtpSetup(void)
{
  FtpSrv = new FtpServer();

  FtpSrv -> setCallback(FtpCallback);
  FtpSrv -> setTransferCallback(FtpTransferCallback);
  FtpSrv -> begin(Prefs.AdminUser, Prefs.AdminPass, "Welcome to TrenoLEGO FTP server");
}
