#include "control.h"
#include "analog.h"


bool errorFlagChan1 = false;
bool errorFlagChan2 = false;

bool chan1_enabled = false;
bool chan2_enabled = false;
bool tracking = false;
bool modeSetting = MODE_CV;

bool railSetting = DUAL; // Need to default to DUAL for the LCD to work (WTF??)

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

  if (tracking) {
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

  if (tracking) {
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

  if (tracking) {
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

  if (tracking) {
    chan1Vcode = chan2Vcode;
    setChan1V();
    displayUpdateChan1V();
  }

  displayUpdateChan2V();
}

void toggleMode() {

  modeSetting = !modeSetting;

  serial_print("Mode: ");

  if (modeSetting == MODE_CV) {
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

  tracking = !tracking;

  // Turn off both channels
  setChan1State(false);
  setChan2State(false);

  serial_print("Tracking mode: ");
  serial_println(tracking);

  if (tracking) {
    chan2Vcode = chan1Vcode;
    //dac.set(DAC_CHAN2_V, chan2Vcode);
    setChan2V(chan2Vcode);

    stepSizeEnc2 = stepSizeEnc1;
  }

  expander.digitalWrite(PORT_B, LED_TRACKING_EN, tracking);

  displayTracking();
}

void toggleChan1State() {

  // Will update the chan1_enabled variable so our call
  // to setChan2State doesn't need inversion
  setChan1State(!chan1_enabled);

  if (tracking) {
    setChan2State(chan1_enabled);
    displayCh2State();
  }

  displayCh1State();
}

void toggleChan2State() {

  setChan2State(!chan2_enabled);

  if (tracking) {
    setChan1State(chan2_enabled);
    displayCh1State();
  }

  displayCh2State();
}

void setChan1State(bool state) {

  // Return if we're already in that state
  if (chan1_enabled == state) {
    serial_println("Chan1 no change");
    return;
  }

  chan1_enabled = state;

  setChannelState(DAC_CHAN1_V, chan1_enabled);

  digitalWrite(OUT_VPLUS, chan1_enabled);
  expander.digitalWrite(PORT_B, LED_VPLUS_EN, chan1_enabled);

  /*************** DEBUG ***************/
  if (chan1_enabled) {
    serial_println("Chan1 enabled");
  } else {
    serial_println("Chan1 disabled");
  }
  /*************************************/
}

void setChan2State(bool state) {

  // Return if we're already in that state
  if (chan2_enabled == state) {
    serial_println("Chan2 no change");
    return;
  }

  chan2_enabled = state;

  setChannelState(DAC_CHAN2_V, chan2_enabled);

  digitalWrite(OUT_VMINUS, chan2_enabled);
  expander.digitalWrite(PORT_B, LED_VMINUS_EN, chan2_enabled);

  /*************** DEBUG ***************/
  if (chan2_enabled) {
    serial_println("Chan2 enabled");
  } else {
    serial_println("Chan2 disabled");
  }
  /*************************************/
}
