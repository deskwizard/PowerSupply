#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "LCD_defs.h"
#include "defines.h"
#include "externs.h"

void writeDAC();



bool getRailSetting();
bool getTrackingMode();
bool getOutputMode();

bool getChannel1State();
void setChannel1ErrorFlag();
bool getChannel1ErrorFlag();
void clearChannel1ErrorFlag();

bool getChannel2State();
bool getChannel2ErrorFlag();
void clearChannel2ErrorFlag();
void setChannel2ErrorFlag();

void toggleChan1State();
void toggleChan2State();

void setChan1State(bool state);
void setChan2State(bool state);

void changeChan1V(bool direction);
void changeChan1I(bool direction);
void changeChan2V(bool direction);
void changeChan2I(bool direction);

void toggleOutputMode();
// void toggleRail();
void toggleTracking();

void dac_init();

#endif