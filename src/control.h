#include "LCD_defs.h"
#include "defines.h"
#include "externs.h"

void writeDAC();


void toggleChan1State();
void toggleChan2State();

void setChan1State(bool state);
void setChan2State(bool state);

void changeChan1V(bool direction);
void changeChan1I(bool direction);
void changeChan2V(bool direction);
void changeChan2I(bool direction);

void toggleMode();
// void toggleRail();
void toggleTracking();

void dac_init();
