#include "analog.h"
#include "control.h"
#include <ADS7828.h>
#include <DAC7678.h>

ADS7828 adc(ADC_ADDR);
DAC7678 dac(DAC_ADDR);

// Needs to be signed for encoder step size things...
// -1 here is used to display dashes on the display
// Yeah, wtf was I thinking... FixMe
int16_t chan1Vcode = 2000;
int16_t chan1Icode = -1;

int16_t chan2Vcode = 2000;
int16_t chan2Icode = -1;

// Running totals
uint32_t totalChan1V = 0;
uint32_t totalChan1I = 0;
uint32_t totalChan2V = 0;
uint32_t totalChan2I = 0;

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

  if (((uint32_t)(currentMillis - previousMillis) >= 1)) {

    static uint8_t readIndex = 0;

    static uint16_t readingsChan1V[ADC_READ_AVG] = {0};
    static uint16_t readingsChan1I[ADC_READ_AVG] = {0};
    static uint16_t readingsChan2V[ADC_READ_AVG] = {0};
    static uint16_t readingsChan2I[ADC_READ_AVG] = {0};

    if (getChannel1State()) {
      // Subtract the last readings
      totalChan1V = totalChan1V - readingsChan1V[readIndex];
      totalChan1I = totalChan1I - readingsChan1I[readIndex];

      // Read ADC channels
      readingsChan1V[readIndex] = adc.read(ADC_CHAN1_V, SD);
      readingsChan1I[readIndex] = adc.read(ADC_CHAN1_I, SD);

      // Add readings to the total
      totalChan1V = totalChan1V + readingsChan1V[readIndex];
      totalChan1I = totalChan1I + readingsChan1I[readIndex];
    }

    if (getChannel2State()) {

      // Subtract the last readings
      totalChan2V = totalChan2V - readingsChan2V[readIndex];
      totalChan2I = totalChan2I - readingsChan2I[readIndex];

      // Read ADC channels
      readingsChan2V[readIndex] = adc.read(ADC_CHAN2_V, SD);
      readingsChan2I[readIndex] = adc.read(ADC_CHAN2_I, SD);

      // Add readings to the total
      totalChan2V = totalChan2V + readingsChan2V[readIndex];
      totalChan2I = totalChan2I + readingsChan2I[readIndex];
    }

    // Advance to the next position in the array:
    readIndex = readIndex + 1;

    // End of the array reached, wrap around
    if (readIndex >= ADC_READ_AVG) {
      readIndex = 0;
    }

    previousMillis = currentMillis;
  }
}

uint16_t getChannel1Voltage() { return totalChan1V / ADC_READ_AVG; }
uint16_t getChannel1Current() { return totalChan1I / ADC_READ_AVG; }
uint16_t getChannel2Voltage() { return totalChan2V / ADC_READ_AVG; }
uint16_t getChannel2Current() { return totalChan2I / ADC_READ_AVG; }

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
  dac.set(2, dacValue);
  dac.set(4, dacValue / 2);
  dac.set(6, dacValue);
  Serial.print("DAC: ");
  Serial.println(dacValue);
}

void readADC() {

  uint16_t read_value = 0;
  float voltage = 0;

  // for (int x = 0; x <= 7; x++) {
  for (int x = 0; x <= 6; x = x + 2) {

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

    Serial.print("Chan1V: ");
    Serial.println(getChannel1Voltage());

    Serial.print("Chan2V: ");
    Serial.println(getChannel2Voltage());

    previousMillis = currentMillis;
  }
}