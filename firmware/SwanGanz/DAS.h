enum CHANNEL {
    PRESSURE1A,
    PRESSURE1B,
    THERM_LOW,      // Low gain
    THERM_HI,       // High gain
    PRESSURE2A,     
    PRESSURE2B
};

enum BUTTON {
    MODE,
    ENTER
};

void DASInit();
void DasStartMenu();
void DasAllSamples();
void InputPolling();
void HeartBeat();