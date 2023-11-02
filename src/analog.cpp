#include "analog.h"
#include <DAC7678.h>

DAC7678 dac(DAC_ADDR);

// Needs to be signed for encoder step size things
int16_t chan1Vcode = 500;
int16_t chan1Icode = -1; // ----------- WTF?? -----------
int16_t chan2Vcode = 500;
int16_t chan2Icode = 0;

void setChannelState(uint8_t channel, bool state) {
  dac.setChannelState(channel, state);
}

/****************************************************************/
/*                             DAC                              */
/****************************************************************/
void dac_init() {

  dac.begin(EXT); // Vref provided by ADC
  dac.reset();
  dac.offMode(L1K);
  dac.disable();

  // Preload in case we need offsetting later
  dac.set(DAC_CHAN1_V, chan1Vcode);
  dac.set(DAC_CHAN2_V, chan2Vcode);
  dac.set(DAC_CHAN1_I, chan1Icode);
  dac.set(DAC_CHAN2_I, chan2Icode);

  //---------------------------------------------------------
  // Current outputs always on (TESTING)
  dac.setChannelState(DAC_CHAN1_I, true);
  dac.setChannelState(DAC_CHAN2_I, true);

  // Here
  // dac.setChannelState(DAC_CHAN1_V, true);
  // dac.setChannelState(DAC_CHAN2_V, true);

  //---------------------------------------
}

void setChan1V() {
  dac.set(DAC_CHAN1_V, chan1Vcode);
  printChan1V();
}

void setChan1V(uint16_t code) {
  chan1Vcode = code;
  dac.set(DAC_CHAN1_V, chan1Vcode);
  printChan1V();
}

void setChan1I() {
  dac.set(DAC_CHAN1_I, chan1Icode);
  printChan1I();
}

void setChan1I(uint16_t code) {
  chan1Icode = code;
  dac.set(DAC_CHAN1_I, chan1Icode);
  printChan1I();
}

void setChan2V() {
  dac.set(DAC_CHAN2_V, chan2Vcode);
  printChan2V();
}

void setChan2V(uint16_t code) {
  chan2Vcode = code;
  dac.set(DAC_CHAN2_V, chan2Vcode);
  printChan2V();
}

void setChan2I() {
  dac.set(DAC_CHAN2_I, chan2Icode);
  printChan2I();
}

void setChan2I(uint16_t code) {
  chan2Icode = code;
  dac.set(DAC_CHAN2_I, chan2Icode);
  printChan2I();
}

// Debug
void writeDAC() {

  dac.enable();

  static uint16_t dacValue = 0;

  if (dacValue < 4000) {
    dacValue = dacValue + 100;
  } else {
    dacValue = 0;
  }

  dac.set(0, dacValue);

  Serial.print("DAC: ");
  Serial.println(dacValue);
}