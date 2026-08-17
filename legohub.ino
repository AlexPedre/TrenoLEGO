
/**************************************
****  TrenoLEGO - Powered Up hub   ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/

    
#ifdef TR_HAS_BLE

/**** LEGO hub handle for loop() ****/

void LegoHubHandle(void)
{
  bool                  LegoHubConnected    = EmulatedHub.isConnected;
  static bool           LegoHubWasConnected = false;
  unsigned long         Now                 = millis();
  static uint8_t        InitStep            = 0;
  static unsigned long  NextInitAt          = 0;

  if (LegoHubWasConnected) {
    if (!LegoHubConnected) {
      PfTrains.MotorSpeedAsync(Prefs.LegoHubConnChannel, 0);
      LastCmdTime = Now;
      if (Prefs.Debug)
        Print(F("[DEBUG] LEGO hub disconnected\r\n"));      
    }
  } else if (LegoHubConnected) {
    InitStep                      = 1;
    NextInitAt                    = Now;
    EmulatedHub.isPortInitialized = false;
    PfTrains.MotorSpeedAsync(Prefs.LegoHubConnChannel, 0);
    LastCmdTime                   = Now;

    if (Prefs.Debug)
      Print(F("[DEBUG] LEGO hub connected\r\n"));  
  }

  if (LegoHubConnected && !EmulatedHub.isPortInitialized && InitStep != 0 && Now >= NextInitAt) {
    switch (InitStep) {
    case 1:
      EmulatedHub.attachDevice((byte)PoweredUpHubPort::A, DeviceType::TRAIN_MOTOR);
      InitStep   = 2;
      NextInitAt = Now + 1000;
      break;

    case 2:
      EmulatedHub.attachDevice((byte)PoweredUpHubPort::LED, DeviceType::HUB_LED);
      InitStep   = 3;
      NextInitAt = Now + 1000;
      break;

    case 3:
      EmulatedHub.attachDevice((byte)PoweredUpHubPort::B, DeviceType::TRAIN_MOTOR);
      InitStep   = 4;
      NextInitAt = Now + 1000;
      break;

    case 4:
      EmulatedHub.isPortInitialized = true;
      InitStep         = 0;
      PfTrains.MotorSpeedAsync(Prefs.LegoHubConnChannel, 0);
      LastCmdTime      = Now;
      if (Prefs.Debug) 
        Print(F("[DEBUG] LEGO ports initialized\r\n"));
      break;
    }
  }

  LegoHubWasConnected = LegoHubConnected;
}


/**** LEGO hub callback ****/

void LegoHubCallback(byte Port, byte Value)
{
  if (Prefs.Debug) {
    Serial.println("[DEBUG] LEGO message received:");
    Serial.print("Port=0x");
    Serial.print(Port, HEX);
    Serial.print(" Value=0x");
    Serial.println(Value, HEX);
  }

  int8_t ValueSign = (int8_t)Value;
  
  if ((Port == (byte)Prefs.LegoHubPort) && 
      (ValueSign >= -100) && (ValueSign <= 100)) {
    PfTrains.MotorSpeedAsync(Prefs.LegoHubConnChannel, ((int8_t)Value * 255) / 100);
    LastCmdTime = millis();
  }
}
#endif


/**** Constructor ****/

TrPowerFunctions::TrPowerFunctions(uint16_t Pin)
{   
  IrSend      = new IRsend(Pin);
  SendRepeats = 5;

  for(int Ch = 0; Ch < sizeof(ToggleBit); ++Ch)
    ToggleBit[Ch] = 0;

  IrSend -> begin();
}


/**** Build the output ****/

uint16_t TrPowerFunctions::BuildSingleOutputPwm(uint8_t Channel, uint8_t Port, uint8_t Toggle, int8_t Step)
{
  const uint8_t a  = 0;                       /* Address = 0 (PF receiver expects 0) */
  const uint8_t E  = 0;                       /* Escape  = 0 (uses normal Mode)      */
  const uint8_t M  = 0;                       /* 0 = PWM                             */
  const uint8_t D  = PwmDataNibble(Step);     /* PWM data                            */

  /**** Nibble1: T E C C ****/
  
  const uint8_t n1 = (uint8_t)((Toggle & 0x1) << 3) | (E << 2) | (Channel & 0x3);

  /**** Nibble2: a 1 M O  ****/
  
  const uint8_t n2 = (uint8_t)((a & 0x1) << 3) | (1 << 2) | ((M & 0x1) << 1) | (Port & 0x1);

  /**** Nibble3: DDDD ****/
  
  const uint8_t n3 = D & 0xF;

  /**** LRC: 0xF xor N1 xor N2 xor N3 ****/

  const uint8_t lrc = (uint8_t)(0xF ^ n1 ^ n2 ^ n3);

  return (uint16_t)((n1 << 12) | (n2 << 8) | (n3 << 4) | (lrc & 0xF));
}


/**** Channel helper ****/

uint8_t TrPowerFunctions::ChannelBits(uint8_t Channel)
{
  if (Channel < 1) return 0;
  if (Channel > 4) return 3;
  
  return Channel - 1;
}


/**** Map motor value from -255...255 to -7...7 range ****/

int8_t TrPowerFunctions::MapToPfSpeed(int Value) 
{
  if (Value >  255) return 7;
  if (Value < -255) return -7;

  if (Value == 0) return 0;

  int16_t Scaled = (Value * 7 + (Value > 0 ? 127 : -127)) / 255;

  if (Scaled >  7) Scaled =  7;
  if (Scaled < -7) Scaled = -7;

  return (int8_t)Scaled;
}


/**** Control the motor ****/

void TrPowerFunctions::MotorSpeed(uint8_t Channel, uint8_t Port, int8_t Speed)
{
  Channel             = ChannelBits(Channel);
  ToggleBit[Channel] ^= 1;

  SendCommand(BuildSingleOutputPwm(Channel, Port, ToggleBit[Channel], Speed));
}


/**** Pwm data nibble ****/

uint8_t TrPowerFunctions::PwmDataNibble(int8_t Step)
{
  if (Step == 0) return 0;
  if (Step >  7) return 7;
  if (Step < -7) Step = -7;

  if (Step > 0) return Step;

  return (uint8_t)(0x10 - (uint8_t)abs(Step));
}


/**** Send the command ****/

void TrPowerFunctions::SendCommand(uint16_t Payload)
{
  IrSend -> sendLegoPf(Payload, kLegoPfBits, SendRepeats - 1);
}
