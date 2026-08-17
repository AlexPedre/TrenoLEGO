
/**************************************
****     TrenoLEGO - Classes       ****
**** (c) 2026, Alessandro Pedretti ****
**************************************/


#ifndef _TR_CLASSES_H
#  define  _TR_CLASSES_H

/**** Power Functions channels ****/

#  define  TR_PF_CHANNEL_0        0     /* 4.5 V direct connected       */
#  define  TR_PF_CHANNEL_1        1     /* Power Functions 1            */
#  define  TR_PF_CHANNEL_2        2     /* Power Functions 2            */
#  define  TR_PF_CHANNEL_3        3     /* Power Functions 3            */
#  define  TR_PF_CHANNEL_4        4     /* Power Functions 4            */

#  define  TR_PF_CHANNEL_FIRST    0     /* First channel                */
#  define  TR_PF_CHANNEL_FIRSTPF  1     /* First Power Function channel */
#  define  TR_PF_CHANNEL_NUM      5     /* Number of channels           */
#  define  TR_PF_CHANNEL_UNK      255   /* Unknown channel              */

/**** Power Functions ports ****/

#  define  TR_PF_PORT_RED         0
#  define  TR_PF_PORT_BLUE        1

/**** Preferences ****/

# define  TR_PREFS_LANG_NUM       2
# define  TR_PREFS_JSONSIZE       3072

/**** Train structure ****/

typedef struct {
  bool                  Enabled;            /* Train enabled                      */
  int                   AccelAmount;        /* Acceleration amount                */
  int                   Invert;             /* Control inversion                  */
  int                   Speed;              /* Target speed                       */
  int                   CurrentSpeed;       /* Current speed                      */
  unsigned long         LastUpdate;         /* Last update time                   */
  uint8_t               CurrentSpeedStep;   /* Current speed step for PF          */
  uint8_t               Port;               /* Port                               */
  char                  Name[32];           /* Train name                         */
  char                  NameReverse[32];    /* Reverse train name (Alexa command) */
  unsigned char         AlexaID;            /* Alexa device ID                    */
  unsigned long         AccelStepTime;      /* Acceleration step time (ms)        */
} TR_PF_TRAIN;

class Print;

/**** Power Functions class ****/

class TrPowerFunctions {
 
  public:

    /**** Public methods ****/
 
                                TrPowerFunctions(uint16_t Pin);
    int8_t                      MapToPfSpeed(int Value); 
    void                        MotorSpeed(uint8_t Channel, uint8_t Port, int8_t Speed);

  private:

    /**** Private properties ****/

    IRsend *                    IrSend;
    uint8_t                     SendRepeats;
    uint8_t                     ToggleBit[4];

    /**** Private methods ****/

    uint16_t                    BuildSingleOutputPwm(uint8_t Channel, uint8_t Port, uint8_t Toggle, int8_t Step);
    uint8_t                     ChannelBits(uint8_t Channel);
    uint8_t                     PwmDataNibble(int8_t Step);
    void                        SendCommand(uint16_t Payload);
};


/**** Power Functions train class ****/

class TrPfTrains {

    /**** Public methods ****/

  public:
                                TrPfTrains(void);
    void                        Default(void);
    inline void                 Enable(uint8_t Channel, bool Enable) { if (Channel < TR_PF_CHANNEL_NUM) Train[Channel].Enabled = Enable; }
    uint8_t                     FindChannel(unsigned char AlexaID);
    inline int                  GetAccelAmount(uint8_t Channel) { return (Channel < TR_PF_CHANNEL_NUM) ? Train[Channel].AccelAmount : 0; }
    inline unsigned long        GetAccelStepTime(uint8_t Channel) { return (Channel < TR_PF_CHANNEL_NUM) ? Train[Channel].AccelStepTime : 0; }
    inline bool                 GetEnabled(uint8_t Channel) { return (Channel < TR_PF_CHANNEL_NUM) ? Train[Channel].Enabled : false; }
    inline int                  GetInvert(uint8_t Channel) { return (Channel < TR_PF_CHANNEL_NUM) ? (Train[Channel].Invert == -1) : false; }
    inline const char *         GetName(uint8_t Channel) { return (Channel < TR_PF_CHANNEL_NUM) ? Train[Channel].Name : Train[0].Name; }
    inline const char *         GetNameReverse(uint8_t Channel) { return (Channel < TR_PF_CHANNEL_NUM) ? Train[Channel].NameReverse : Train[0].NameReverse; }
    inline uint8_t              GetPort(uint8_t Channel) { 
                                  return ((Channel >= TR_PF_CHANNEL_FIRSTPF) && (Channel < TR_PF_CHANNEL_NUM)) ? Train[Channel].Port : TR_PF_PORT_RED;
                                }
    inline const char *         GetPortStr(uint8_t Channel) { 
                                  return ((Channel >= TR_PF_CHANNEL_FIRSTPF) && (Channel < TR_PF_CHANNEL_NUM)) ? ::PfPortStr[(int)Train[Channel].Port] : ::PfPortStr[2];
                                }                                
    inline int                  GetSpeed(uint8_t Channel) { return (Channel < TR_PF_CHANNEL_NUM) ? Train[Channel].Speed : 0; }
    bool                        MotorSpeed(uint8_t Channel, int Speed);
    void                        MotorSpeedApply(uint8_t Channel, int Speed);
    bool                        MotorSpeedAsync(uint8_t Channel, int Speed);
    bool                        MotorSpeedFix(uint8_t Channel, int &Speed);
    bool                        MotorSpeedLoop(void);
    bool                        MotorSpeedSoft(uint8_t Channel, int Speed);
    inline void                 SetAccelAmount(uint8_t Channel, int AccelAmount) { if (Channel < TR_PF_CHANNEL_NUM) Train[Channel].AccelAmount = AccelAmount; }
    inline void                 SetAccelStepTime(uint8_t Channel, unsigned long AccelStepTime) { if (Channel < TR_PF_CHANNEL_NUM) Train[Channel].AccelStepTime = AccelStepTime; }
    inline void                 SetAlexaID(uint8_t Channel, unsigned char AlexaID) { if (Channel < TR_PF_CHANNEL_NUM) Train[Channel].AlexaID = AlexaID; }
    inline void                 SetCurrentSpeed(uint8_t Channel, int Speed) { if (Channel < TR_PF_CHANNEL_NUM) Train[Channel].CurrentSpeed = Speed; }
    inline void                 SetInvert(uint8_t Channel, bool En) { if (Channel < TR_PF_CHANNEL_NUM) Train[Channel].Invert = En ? -1 : 1; }
    inline void                 SetName(uint8_t Channel, const char *Name) {
                                  if ((Channel < TR_PF_CHANNEL_NUM) && (Name) && (*Name)) 
                                    strlcpy(Train[Channel].Name, Name, sizeof(Train[Channel].Name)); 
                                }
    inline void                 SetNameReverse(uint8_t Channel, const char *NameReverse) {
                                  if ((Channel < TR_PF_CHANNEL_NUM) && (NameReverse) && (*NameReverse))
                                    strlcpy(Train[Channel].NameReverse,  NameReverse, sizeof(Train[Channel].NameReverse));
                                }
    inline void                 SetPort(uint8_t Channel, uint8_t Port) {
                                  if ((Channel >= TR_PF_CHANNEL_FIRSTPF) && (Channel < TR_PF_CHANNEL_NUM))
                                    Train[Channel].Port = Port;
                                }
    inline bool                 SetPort(uint8_t Channel, const char *Port) { 
                                  return ((Channel >= TR_PF_CHANNEL_FIRSTPF) && (Channel < TR_PF_CHANNEL_NUM)) ?
                                         ::SetPfPort(&Train[Channel].Port, Port) : false;
                                }

    /**** Private properties ***/

  private:

    TrPowerFunctions *          hIr;
    TR_PF_TRAIN                 Train[TR_PF_CHANNEL_NUM];
};

/**** Preferences class ****/

class TrPrefs {

  public:

    enum {
      ENG = 0,
      ITA
    } LangIDs;

    /**** Public properties ****/

    bool                Debug;                /* Debug flag                      */
    bool                FtpEnabled;           /* FTP server enabled               */
    bool                MqttEnabled;          /* MQTT enabled                     */
    bool                SleepEnabled;         /* Sleep enabled                    */
    bool                SpeechEnabled;        /* Speech synthesis enabled         */
    bool                TelnetEnabled;        /* Telnet server enabled            */
    bool                VibrEnabled;          /* Control device vibration enabled */
    uint8_t             LangID;               /* Language ID                      */
    unsigned long       SleepTime;            /* Inactivity time to sleep         */
    unsigned long       Version;              /* Preferences version              */

    /**** Administrator credentials ****/

    char                AdminUser[32];        /* Administrator user name          */
    char                AdminPass[32];        /* Administrator password           */

    /**** Philips Hue emulation ****/

    bool                HueEnabled;           /* Hue enabled                      */

    /**** NTP server ****/

    char                NtpServer[32];        /* NTP server                       */
    char                TimeZone[32];         /* Time zone                        */

    /**** MQTT server ****/

    char                MqttServer[32];       /* MQTT server                      */
    char                MqttTopic[32];        /* MQTT base topic                  */
    char                MqttUser[32];         /* MQTT user name                   */
    char                MqttPass[32];         /* MQTT password                    */
    int                 MqttPort;             /* MQTT port                        */
    unsigned long       MqttAvailRefresh;     /* MQTT availability refresh (ms)   */

    /**** Powered Up ****/

#ifdef TR_HAS_BLE
    bool                LegoHubEnabled;       /* LEGO Powered Up hub enabled      */
    PoweredUpHubPort    LegoHubPort;          /* Emulated port                    */
    uint8_t             LegoHubConnChannel;   /* Connected channel/train          */
#endif

    /**** PWM parameters ****/

    int                 PwmDeband;            /* Deband interval (0...255)        */
    int                 PwmMax;               /* Max. PWM                         */
    int                 PwmMin;               /* Min. PWM                         */

    /**** WiFi ****/

    char                HostName[32];         /* Host name                        */
    char                WiFiSSID[34];         /* WiFi SSID (max 32 chars)         */
    char                WiFiPass[64];         /* WiFi password (max 63 chars)     */

    /**** Public methods ****/

                        TrPrefs(void);
    void                FillDoc(JsonDocument &Doc);
    bool                FromJson(const char *JsonStr);
    bool                FromJson(Stream &Source);
    int                 GetLangID(const char *Str);
    inline const char * GetLangStr(uint8_t ID) {
                          return (ID < TR_PREFS_LANG_NUM) ? LangStr[ID] : LangStr[0];  
                        }
           
    void                Load(bool Default);
    void                MapFromDoc(JsonDocument &Json);
    bool                Save(void);
    void                SaveToStream(class Print &Out, bool Compact);

    void                Show(void);
    bool                SetHostName(const char *Name);
    bool                SetMqttServer(const char *Name);

    /**** Powered Up methods ****/
 
#ifdef TR_HAS_BLE
    inline const char * GetLegoHubPort(void) { return GetPuPort(LegoHubPort); }
    inline bool         SetLegoHubPort(const char *Port) { return ::SetPuPort(&LegoHubPort, Port); }
#endif

 private:

    /**** Private properties ****/

    const char *        LangStr[3] = { "en", "it", NULL };

    /**** Private methods ****/

    template <typename T, typename K>
    bool                JsonPop(T &Val, JsonDocument &Json, K *Key);

    template <typename K>
    bool                JsonPop(char *Val, JsonDocument &Json, K *Key, size_t ValSize);
};

#endif
