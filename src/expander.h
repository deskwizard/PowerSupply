#ifndef _EXPANDER_H_
#define _EXPANDER_H_

#include "defines.h"
#include "externs.h"

void initExpander();

void IRAM_ATTR setInterruptFlag(bool value);

void setTrackingLED(bool state);
void setChannel1StateLED(bool state);
void setChannel2StateLED(bool state);
void setAllLEDs(bool state);
void setExpanderDebugLED(bool state);

void handleExpanderInputs();

#endif