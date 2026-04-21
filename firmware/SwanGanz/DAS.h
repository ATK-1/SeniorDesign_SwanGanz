enum CHANNEL {
    PRESSURE1A,
    PRESSURE1B,
    THERM_LOW,      // Low gain
    THERM_HI,       // High gain
    PRESSURE2A,     
    PRESSURE2B
};

enum BUTTON {
    NULL_INPUT,
    START_BUTTON,
    VOLUME_BUTTON,
    TEMP_BUTTON,
    LEFT_BUTTON,
    UP_BUTTON,
    DOWN_BUTTON,
    RIGHT_BUTTON
};

void DASInit();
void DasStartMenu();
void DasAllSamples();
void InputPolling();
void HeartBeat();