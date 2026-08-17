
/**************************************
****    TrenoLEGO - Train class    ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


/**** Constructor ****/

TrPfTrains::TrPfTrains(void)
{
  hIr = new TrPowerFunctions(TR_PIN_IR);
  Default();
}


/**** Default ****/

void TrPfTrains::Default(void)
{
  TR_PF_TRAIN * TrainPtr = Train;

  for(unsigned int Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel) {
    if (Channel >= TR_PF_CHANNEL_FIRSTPF) {
      TrainPtr -> AccelAmount   = 15;
      TrainPtr -> AccelStepTime = 150;
      TrainPtr -> Enabled       = false;
      TrainPtr -> Port          = TR_PF_PORT_RED;

      snprintf_P(TrainPtr -> Name       , sizeof(TrainPtr -> Name       ), PSTR(TR_ALEXA_TRAIN_PF        ), Channel); 
      snprintf_P(TrainPtr -> NameReverse, sizeof(TrainPtr -> NameReverse), PSTR(TR_ALEXA_TRAIN_PF_REVERSE), Channel);      
    } else {
      TrainPtr -> AccelAmount   = 5;
      TrainPtr -> AccelStepTime = 15;
      TrainPtr -> Enabled       = true;
      strlcpy_P(TrainPtr -> Name       , PSTR(TR_ALEXA_TRAIN_0        ), sizeof(TrainPtr -> Name       ));
      strlcpy_P(TrainPtr -> NameReverse, PSTR(TR_ALEXA_TRAIN_0_REVERSE), sizeof(TrainPtr -> NameReverse));
    }
   
    TrainPtr -> AlexaID          = 255;
    TrainPtr -> CurrentSpeed     = 0;
    TrainPtr -> CurrentSpeedStep = 0;
    TrainPtr -> Invert           = 1;
    TrainPtr -> LastUpdate       = 0L;     
    TrainPtr -> Speed            = 0;
    ++TrainPtr;
  } /* End of for (k) */
}


/**** Find the channel by Alexa ID ****/

uint8_t TrPfTrains::FindChannel(unsigned char AlexaID)
{
  for(uint8_t Channel = TR_PF_CHANNEL_FIRST; Channel < TR_PF_CHANNEL_NUM; ++Channel) {
    unsigned char TrainID = Train[Channel].AlexaID;
    if ((AlexaID == TrainID) || (AlexaID == (TrainID + 1)))
      return Channel; 
  } /* End of for (Channel) */

  return TR_PF_CHANNEL_UNK;
}


/**** Motor speed ****/

bool TrPfTrains::MotorSpeed(uint8_t Channel, int Speed)
{
  if (!MotorSpeedFix(Channel, Speed)) return false;  
  
  Train[Channel].Speed        = Speed;
  Train[Channel].CurrentSpeed = Speed;

  MotorSpeedApply(Channel, Speed);

  return true;
}


/**** Motor speed apply ****/

void TrPfTrains::MotorSpeedApply(uint8_t Channel, int Speed)
{
  TR_PF_TRAIN * TrainPtr = Train + Channel;

  Speed *= TrainPtr -> Invert;

  if (Channel >= TR_PF_CHANNEL_FIRSTPF) {
    int8_t SpeedStep = hIr -> MapToPfSpeed(Speed);

    /**** Avoid the IR overload ****/

    if (TrainPtr -> CurrentSpeedStep != SpeedStep) {
      hIr -> MotorSpeed(Channel, TrainPtr -> Port, SpeedStep);
      TrainPtr -> CurrentSpeedStep = SpeedStep;
    }
  } else {
    int Denom = Prefs.PwmMax - Prefs.PwmDeband;

    if (!Denom) Denom = 1;
    long Duty = Prefs.PwmMin + (long)(abs(Speed) - Prefs.PwmDeband) * (Prefs.PwmMax - Prefs.PwmMin) / Denom;

    if (Duty < Prefs.PwmMin) Duty = Prefs.PwmMin;
    if (Duty > Prefs.PwmMax) Duty = Prefs.PwmMax;

#ifdef ESP32
    if (Speed > 0) {
      ledcWrite(TR_MOTOR_PWM_CH1, Duty);
      ledcWrite(TR_MOTOR_PWM_CH2, 0);
    } else if (Speed < 0) {
      ledcWrite(TR_MOTOR_PWM_CH1, 0);
      ledcWrite(TR_MOTOR_PWM_CH2, Duty);
    } else {
      ledcWrite(TR_MOTOR_PWM_CH1, 0);
      ledcWrite(TR_MOTOR_PWM_CH2, 0);
    }
#else
    if (Speed > 0) {
      analogWrite(TR_PIN_MOTOR_A_IN1, Duty);
      analogWrite(TR_PIN_MOTOR_A_IN2, 0);
    } else if (Speed < 0) {
      analogWrite(TR_PIN_MOTOR_A_IN1, 0);
      analogWrite(TR_PIN_MOTOR_A_IN2, Duty);
    } else {
      analogWrite(TR_PIN_MOTOR_A_IN1, 0);
      analogWrite(TR_PIN_MOTOR_A_IN2, 0);
    }
#endif
  }
}


/**** Motor speed async ****/

bool TrPfTrains::MotorSpeedAsync(uint8_t Channel, int Speed)
{
  if ((!MotorSpeedFix(Channel, Speed)) ||
      ((PendingCmd != TR_PENDCMD_NONE) && (PendingCmd != TR_PENDCMD_MOTOR)))
    return false;

  PendingMotorChannel = Channel;
  PendingMotorSpeed   = Speed;
  PendingCmd          = TR_PENDCMD_MOTOR;

  return true;
}


/**** Fix the motor speed ****/

bool TrPfTrains::MotorSpeedFix(uint8_t Channel, int &Speed)
{  
  if (Channel >= TR_PF_CHANNEL_NUM) return false;

  TR_PF_TRAIN * TrainPtr = Train + Channel;

  if (!TrainPtr -> Enabled) return false;

       if ((Speed > -Prefs.PwmDeband) && (Speed < Prefs.PwmDeband)) Speed = 0;
  else if (Speed >  255) Speed =  255;
  else if (Speed < -255) Speed = -255;

  return true;
}


/**** Motor speed loop ****/

bool TrPfTrains::MotorSpeedLoop(void) 
{
  bool              Updated = false;
  unsigned long     Now     = millis();
  
  for (int i = TR_PF_CHANNEL_FIRST; i < TR_PF_CHANNEL_NUM; i++) {
    TR_PF_TRAIN * TrainPtr = Train + i;
    
    if ((!TrainPtr -> Enabled) || (TrainPtr -> CurrentSpeed == TrainPtr -> Speed)) 
      continue;

    if (Now - Train -> LastUpdate >= TrainPtr -> AccelStepTime) {
      int Diff = TrainPtr -> Speed - TrainPtr -> CurrentSpeed;
      
      if (abs(Diff) <= TrainPtr -> AccelAmount)
        TrainPtr -> CurrentSpeed = TrainPtr -> Speed;
      else 
        TrainPtr -> CurrentSpeed += (Diff > 0) ? TrainPtr -> AccelAmount : -TrainPtr -> AccelAmount;

      MotorSpeedApply(i, TrainPtr -> CurrentSpeed);
      TrainPtr -> LastUpdate = Now;
      Updated                = true;
    }
  } /* End of for (i) */

  return Updated;
}


/**** Motor speed soft ****/

bool TrPfTrains::MotorSpeedSoft(uint8_t Channel, int Speed)
{
  if (MotorSpeedFix(Channel, Speed)) {
    Train[Channel].Speed = Speed;
    return true;
  }

  return false;
}
