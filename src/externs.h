#ifndef _EXTERNS_H_
#define _EXTERNS_H_

extern BluetoothSerial BT_Serial;
extern bool waitForBT;

extern int16_t chan1Vcode;
extern int16_t chan1Icode;
extern int16_t chan2Vcode;
extern int16_t chan2Icode;

extern uint16_t stepSizeEnc1;
extern uint16_t stepSizeEnc2;

extern volatile bool ExpanderIntFlag;

#endif