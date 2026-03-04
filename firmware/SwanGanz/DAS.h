enum CHANNEL {
    PRESSURE1A,
    PRESSURE1B,
    THERM_LOW,      // Low gain
    THERM_HI,       // High gain
    PRESSURE2A,     
    PRESSURE2B
};

void DASInit();
void DAS();
void InputPolling();
void HeartBeat();