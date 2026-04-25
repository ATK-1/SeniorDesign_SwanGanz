enum BUTTON {
    NULL_INPUT,
    START_BUTTON,
    VOLUME_BUTTON,
    TEMP_BUTTON,
    LEFT_BUTTON,
    UP_BUTTON,
    DOWN_BUTTON,
    RIGHT_BUTTON,
    RESET_BUTTON
};

void DASInit();
void DasStartMenu();
void DasAllSamples();
void InputPolling();
void HeartBeat();
void TestDas();