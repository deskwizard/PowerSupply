#include "LCD_defs.h"
#include "defines.h"
#include "externs.h"

// DAC channels 
// (Even #s since the outputs pins are on the same side of the chip)
#define DAC_CHAN1_V 0
#define DAC_CHAN1_I 2
#define DAC_CHAN2_V 4
#define DAC_CHAN2_I 6

void writeDAC();


void toggleChan1State();
void toggleChan2State();

void setChan1State(bool state);
void setChan2State(bool state);

void changeChan1V(bool direction);
void changeChan1I(bool direction);
void changeChan2V(bool direction);
void changeChan2I(bool direction);

void setChan1V();
void setChan1V(uint16_t code);
void setChan1I();
void setChan1I(uint16_t code);
void setChan2V();
void setChan2V(uint16_t code);
void setChan2I();
void setChan2I(uint16_t code);

void toggleMode();
// void toggleRail();
void toggleTracking();

void dac_init();
