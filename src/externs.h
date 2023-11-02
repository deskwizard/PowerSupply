extern MCP23017 expander;

extern BluetoothSerial BT_Serial;
extern bool waitForBT;

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
