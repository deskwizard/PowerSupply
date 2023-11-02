extern MCP23017 expander;


extern BluetoothSerial BT_Serial;
extern bool waitForBT;

extern bool chan1_enabled;
extern bool chan2_enabled;
extern bool errorFlagChan1;
extern bool errorFlagChan2;
extern bool tracking;
extern bool railSetting;
extern bool modeSetting;

extern int16_t chan1VRead;
extern int16_t chan1IRead;

extern int16_t chan1Vcode;
extern int16_t chan1Icode;
extern int16_t chan2Vcode;
extern int16_t chan2Icode;

extern volatile uint8_t currentEnc1Pos;
extern uint16_t stepSizeEnc1;
extern volatile uint8_t currentEnc2Pos;
extern uint16_t stepSizeEnc2;

extern volatile bool ExpanderIntFlag;
