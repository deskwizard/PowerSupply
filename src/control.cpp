#include "control.h"
#include "analog.h"

bool channel1ErrorFlag = false;
bool channel2ErrorFlag = false;

bool channel1State = false;
bool channel2State = false;
bool trackingMode = false;
bool outputMode = MODE_CV;

bool railSetting = DUAL; // Need to default to DUAL for the LCD to work (WTF??)

/************************************************************/

bool getRailSetting() { return railSetting; }

bool getTrackingMode() { return trackingMode; }

bool getOutputMode() { return outputMode; }

bool getChannel1State() { return channel1State; }
bool getChannel1ErrorFlag() { return channel1ErrorFlag; }
void clearChannel1ErrorFlag() { channel1ErrorFlag = false; }
void setChannel1ErrorFlag() { channel1ErrorFlag = true; }

bool getChannel2State() { return channel2State; }
bool getChannel2ErrorFlag() { return channel2ErrorFlag; }
void clearChannel2ErrorFlag() { channel2ErrorFlag = false; }
void setChannel2ErrorFlag() { channel2ErrorFlag = true; }

/************************************************************/

void changeChan1I(bool direction) {

  if (direction == UP) {

    if (chan1Icode == -1) {
      chan1Icode = 0;
    } else {
      chan1Icode = chan1Icode + stepSizeEnc1;

      if (chan1Icode > MAX_I_CODE) {
        chan1Icode = MAX_I_CODE;
      }
    }
  } else {
    chan1Icode = chan1Icode - stepSizeEnc1;

    if (chan1Icode < 0) {
      chan1Icode = -1;
    }
  }

  setChan1I();

  if (trackingMode) {
    chan2Icode = chan1Icode;
    setChan2I();
    displayUpdateChan2I();
  }

  displayUpdateChan1I();
}

void changeChan2I(bool direction) {

  if (direction == UP) {

    chan2Icode = chan2Icode + stepSizeEnc2;

    if (chan2Icode > MAX_I_CODE) {
      chan2Icode = MAX_I_CODE;
    }
  } else {
    chan2Icode = chan2Icode - stepSizeEnc2;

    if (chan2Icode < 0) {
      chan2Icode = 0;
    }
  }

  setChan2I();

  if (trackingMode) {
    chan1Icode = chan2Icode;
    setChan1I();
    displayUpdateChan1I();
  }

  displayUpdateChan2I();
}

void changeChan1V(bool direction) {
  if (direction == UP) {
    chan1Vcode = chan1Vcode + (stepSizeEnc1 >> railSetting);

    if (chan1Vcode > MAX_V_CODE) {
      chan1Vcode = MAX_V_CODE;
    }
  } else {
    chan1Vcode = chan1Vcode - (stepSizeEnc1 >> railSetting);

    if (chan1Vcode < 0) {
      chan1Vcode = 0;
    }
  }

  setChan1V();

  if (trackingMode) {
    chan2Vcode = chan1Vcode;
    setChan2V();
    displayUpdateChan2V();
  }

  displayUpdateChan1V();
}

void changeChan2V(bool direction) {

  if (direction == UP) {
    chan2Vcode = chan2Vcode + stepSizeEnc2;

    if (chan2Vcode > MAX_V_CODE) {
      chan2Vcode = MAX_V_CODE;
    }
  } else {
    chan2Vcode = chan2Vcode - stepSizeEnc2;

    if (chan2Vcode < 0) {
      chan2Vcode = 0;
    }
  }

  setChan2V();

  if (trackingMode) {
    chan1Vcode = chan2Vcode;
    setChan1V();
    displayUpdateChan1V();
  }

  displayUpdateChan2V();
}

void toggleOutputMode() {

  outputMode = !outputMode;

  serial_print("Mode: ");

  if (outputMode == MODE_CV) {
    serial_println("CV");
  } else {
    serial_println("CC");
  }

  // Reset indicator positions
  stepSizeEnc1 = 1;
  stepSizeEnc2 = stepSizeEnc1;

  displayMode();

  displayUpdateChan1V();
  displayUpdateChan1I();
  displaySelector1();

  // Since tracking is only in dual rail, that'll take care of it automagically
  if (railSetting == DUAL) {
    displayUpdateChan2V();
    displayUpdateChan2I();
    displaySelector2();
  }
}
/*
void toggleRail() {

  railSetting = !railSetting;

  // Reset indicator position
  stepSizeEnc1 = 2;

  serial_print("Range: ");

  if (railSetting == SINGLE) {

    serial_println("Single rail");
    serial_print("Code before: ");
    serial_print(chan1Vcode);

    // Change the dac out to half so out gain switch from 5x to 10x doesn't
    // produce twice the output when we change mode
    chan1Vcode = chan1Vcode >> 1;

    serial_print("  Code after: ");
    serial_println(chan1Vcode);

    setChan2State(false); // Turn the negative rail off since we don't need it
                          // anymore (cap discharge)

    // Disable tracking since we only have one rail now
    tracking = false;
    expander.digitalWrite(PORT_B, LED_TRACKING_EN,
                          tracking); // Need invertion because led is active low
  }

  else {
    serial_println("Dual rail");
    serial_print("Code before: ");
    serial_print(chan1Vcode);

    // Double the DAC output so we don't drop to half on range switch
    chan1Vcode = chan1Vcode << 1;

    serial_print("  Code after: ");
    serial_println(chan1Vcode);
  }

  // Update the DAC and set the mode
  dac.set(DAC_CHAN1_V, chan1Vcode);

  digitalWrite(OUT_RANGE, railSetting);
  expander.digitalWrite(PORT_B, LED_RANGE,
                        railSetting); // This works because railSettings are 0/1

  displayRange();
}
*/
void toggleTracking() {

  if (railSetting == SINGLE) {
    serial_println("Single rail mode, tracking not available");
    return;
  }

  trackingMode = !trackingMode;

  // Turn off both channels
  setChan1State(false);
  setChan2State(false);

  serial_print("Tracking mode: ");
  serial_println(trackingMode);

  if (trackingMode) {
    chan2Vcode = chan1Vcode;
    setChan2V(chan2Vcode);

    stepSizeEnc2 = stepSizeEnc1;
  }

  expander.digitalWrite(PORT_B, LED_TRACKING_EN, trackingMode);

  displayTracking();
}

void toggleChan1State() {

  // Will update the chan1_enabled variable so our call
  // to setChan2State doesn't need inversion
  setChan1State(!channel1State);

  if (trackingMode) {
    setChan2State(channel1State);
    displayCh2State();
  }

  displayCh1State();
}

void toggleChan2State() {

  setChan2State(!channel2State);

  if (trackingMode) {
    setChan1State(channel2State);
    displayCh1State();
  }

  displayCh2State();
}

void setChan1State(bool state) {

  // Return if we're already in that state
  if (channel1State == state) {
    serial_println("Channel 1 no change");
    return;
  }

  channel1State = state;

  setChannelState(DAC_CHAN1_V, channel1State);

  digitalWrite(OUT_VPLUS, channel1State);
  expander.digitalWrite(PORT_B, LED_VPLUS_EN, channel1State);

  /*************** DEBUG ***************/
  if (channel1State) {
    serial_println("Channel 1 enabled");
  } else {
    serial_println("Channel 1 disabled");
  }
  /*************************************/
}

void setChan2State(bool state) {

  // Return if we're already in that state
  if (channel2State == state) {
    serial_println("Channel 2 no change");
    return;
  }

  channel2State = state;

  setChannelState(DAC_CHAN2_V, channel2State);

  digitalWrite(OUT_VMINUS, channel2State);
  expander.digitalWrite(PORT_B, LED_VMINUS_EN, channel2State);

  /*************** DEBUG ***************/
  if (channel2State) {
    serial_println("Channel 2 enabled");
  } else {
    serial_println("Channel 2 disabled");
  }
  /*************************************/
}
