#include "analog.h"
#include "control.h"
#include <ADS7828.h>
#include <DAC7678.h>

ADS7828 adc(ADC_ADDR);
DAC7678 dac(DAC_ADDR);

// Needs to be signed for encoder step size things...
int16_t chan1Vcode = 500;
int16_t chan1Icode = -1; // ----------- WTF?? -----------
int16_t chan1VRead;
int16_t chan1IRead;

int16_t chan2Vcode = 500;
int16_t chan2Icode = 0;
// int16_t chan1VRead;
// int16_t chan1IRead;

/****************************************************************/
/*                             DAC                              */
/****************************************************************/

void setChannelState(uint8_t channel, bool state) {
  dac.setChannelState(channel, state);
}

void initDAC() {

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

  //---------------------------------------------------------
}

void setChan1V(uint16_t code) {
  chan1Vcode = code;
  dac.set(DAC_CHAN1_V, chan1Vcode);
  printChan1V();
}

void setChan1I(uint16_t code) {
  chan1Icode = code;
  dac.set(DAC_CHAN1_I, chan1Icode);
  printChan1I();
}

void setChan2V(uint16_t code) {
  chan2Vcode = code;
  dac.set(DAC_CHAN2_V, chan2Vcode);
  printChan2V();
}

void setChan2I(uint16_t code) {
  chan2Icode = code;
  dac.set(DAC_CHAN2_I, chan2Icode);
  printChan2I();
}

/****************************************************************/
/*                             ADC                              */
/****************************************************************/

void initADC() { adc.begin(INT); }

void handleAnalog() {

  uint32_t currentMillis = millis();
  static uint32_t previousMillis = 0;

  if (((uint32_t)(currentMillis - previousMillis) >= 1) && getChannel1State()) {

    static uint8_t readIndex = 0;

    static uint16_t readingsV[ADC_READ_AVG] = {0};
    static uint32_t totalV = 0;

    static uint16_t readingsI[ADC_READ_AVG] = {0};
    static uint32_t totalI = 0;

    // Subtract the last readings
    totalV = totalV - readingsV[readIndex];
    totalI = totalI - readingsI[readIndex];

    // Read ADC channels
    readingsV[readIndex] = adc.read(ADC_CHAN1_V, SD);
    readingsI[readIndex] = adc.read(ADC_CHAN1_I, SD);

    // Add reading to the total:
    totalV = totalV + readingsV[readIndex];
    totalI = totalI + readingsI[readIndex];

    // Advance to the next position in the array:
    readIndex = readIndex + 1;

    // End of the array, wrap around
    if (readIndex >= ADC_READ_AVG) {
      readIndex = 0;
    }

    // calculate the average:
    chan1VRead = totalV / ADC_READ_AVG;
    chan1IRead = totalI / ADC_READ_AVG;

    previousMillis = currentMillis;
  }
}

/****************************************************************/
/*                            DEBUG                             */
/****************************************************************/

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

void readADC() {

  uint16_t read_value = 0;
  float voltage = 0;

  // for (int x = 0; x <= 7; x++) {
  for (int x = 0; x <= 1; x++) {

    read_value = adc.read(x, SD);
    voltage = read_value * (2500.0 / 4096.0);

    Serial.print("Channel ");
    Serial.print(x);
    Serial.print(": ");
    Serial.print(read_value);
    Serial.print(" (");
    Serial.print(voltage, 0);
    Serial.println(" mV)");
  }

  Serial.println();
}

void testAnalog() {

  uint32_t currentMillis = millis();  // Get snapshot of time
  static uint32_t previousMillis = 0; // Tracks the time since last event fired
  static bool ledState;

  if ((uint32_t)(currentMillis - previousMillis) >= 500) {
    writeDAC();
    readADC();
    previousMillis = currentMillis;
  }
}