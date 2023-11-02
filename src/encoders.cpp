#include "encoders.h"

// Encoders position variables
volatile uint8_t currentEnc1Pos = 0;
uint8_t lastEnc1Pos = 0;

volatile uint8_t currentEnc2Pos = 0;
uint8_t lastEnc2Pos = 0;

// Old stuff:
// Gain is 5x on dual rail, so having 2/20/200 does single decade(?) change in
// decimal (10mV, 100mV, 1V) Step size is divided by 2 when in single rail so it
// behaves the same (10x part in the encoder switch handling)
// uint16_t stepSizeEnc1 = 2;
// uint16_t stepSizeEnc2 = 2;

uint16_t stepSizeEnc1 = 1;
uint16_t stepSizeEnc2 = 1;

// Encoder switch variable
volatile uint8_t currentKeyState = 0; // debounced state
volatile uint8_t key_state;           // bit x = 1: key has changed state

void encoder_init() {

  pinMode(ENC1_A_PIN, INPUT_PULLUP);
  pinMode(ENC1_B_PIN, INPUT_PULLUP);
  pinMode(ENC2_A_PIN, INPUT_PULLUP);
  pinMode(ENC2_B_PIN, INPUT_PULLUP);

  // We need to pre-load the current values,
  // otherwise it might trigger on powerup/reset
  currentEnc1Pos = getEnc1Pos();
  lastEnc1Pos = currentEnc1Pos;
  currentEnc2Pos = getEnc2Pos();
  lastEnc2Pos = currentEnc2Pos;
}

// Functions called from interrupt vector
uint8_t IRAM_ATTR getEnc1Pos() {

  uint8_t enc1Pos = 0;

  if (digitalRead(ENC1_A_PIN))
    enc1Pos |= (1 << 1);

  if (digitalRead(ENC1_B_PIN))
    enc1Pos |= (1 << 0);

  return enc1Pos;
}

uint8_t IRAM_ATTR getEnc2Pos() {

  uint8_t enc2Pos = 0;

  if (digitalRead(ENC2_A_PIN))
    enc2Pos |= (1 << 1);

  if (digitalRead(ENC2_B_PIN))
    enc2Pos |= (1 << 0);

  return enc2Pos;
}

// Functions called from loop()
void handleEncoders() {

  bool direction = DOWN;

  // *********************** Encoder 1 ********************************
  if (currentEnc1Pos != lastEnc1Pos) {

    direction = DOWN;

    // Discard invalid encoder states
    if (!(currentEnc1Pos == 3 && lastEnc1Pos == 1) &&
        !(currentEnc1Pos == 3 && lastEnc1Pos == 2)) {
      lastEnc1Pos = currentEnc1Pos;
      return;
    }

    else if ((currentEnc1Pos == 3 && lastEnc1Pos == 1)) {
      direction = UP;
    }

    if (modeSetting == MODE_CV) {
      changeChan1V(direction);
    } else {
      changeChan1I(direction);
    }

    lastEnc1Pos = currentEnc1Pos;
  }

  // *********************** Encoder 2 ********************************
  if (currentEnc2Pos != lastEnc2Pos) {

    direction = DOWN;

    if (railSetting == SINGLE) {
      serial_println("Single rail; Input ignored");
      lastEnc2Pos = currentEnc2Pos;
      return;
    }

    if (!(currentEnc2Pos == 3 && lastEnc2Pos == 1) &&
        !(currentEnc2Pos == 3 && lastEnc2Pos == 2)) {
      lastEnc2Pos = currentEnc2Pos;
      return;
    }

    else if ((currentEnc2Pos == 3 && lastEnc2Pos == 1)) {
      direction = UP;
    }

    if (modeSetting == MODE_CV) {
      changeChan2V(direction);
    } else {
      changeChan2I(direction);
    }
    lastEnc2Pos = currentEnc2Pos;
  }
}

/************************************************************                                                     
 *                     Encoder switches                     *                                                       
 ************************************************************/

void keys_init() {

  // Pins defaults to inputs, so we only need to set the pullups here
  pinMode(ENC1_BTN_PIN, INPUT_PULLUP);
  pinMode(ENC2_BTN_PIN, INPUT_PULLUP);

  bitWrite(currentKeyState, 0, digitalRead(ENC1_BTN_PIN));
  bitWrite(currentKeyState, 1, digitalRead(ENC2_BTN_PIN));

  Serial.println();
  Serial.print("Boot keys:    ENC1 ");
  Serial.print(bitRead(currentKeyState, 0));
  Serial.print("    ENC2 ");
  Serial.println(bitRead(currentKeyState, 1));
  Serial.println();

  // If we set the wait to true by default, don't read the encoder to set it
  if (waitForBT == false) {
    waitForBT = !bitRead(currentKeyState, 0);
  }
}

// Function called from interrupt vector

void IRAM_ATTR pinRead() {

  static uint8_t counter = 0xFF;

  // Read in the different bits
  uint8_t i = 0;
  /*
    bitWrite(i, 0, (ENC1_PIN & (1 << ENC1_KEY)) );
    bitWrite(i, 1, (ENC2_PIN & (1 << ENC2_KEY)) );
  */

  bitWrite(i, 0, digitalRead(ENC1_BTN_PIN));
  bitWrite(i, 1, digitalRead(ENC2_BTN_PIN));

  if (i != currentKeyState) { // If the current debounced keys differs from the
                              // pin read

    if (counter == 0xFF) { // If it's the first time around, decrement the
                           // counter once for next if statement
      counter--;
      // counter = counter >> 1;
    } else if (counter >
               0) { // If it's still different, shift the counter value
      // counter--;
      counter = counter >> 1;
    }

    // If the counter reaches 0, consider the key debounced and reset the
    // debounce counter
    else {
      key_state = i ^ currentKeyState;
      currentKeyState = i;
      counter = 0xFF;
    }
  }

  // If the pin read value changes back to the debounced state before the
  // debounce counter reaches 0, reset the debounce counter.

  else if (counter != 0xFF) {
    counter = 0xFF;
  }

} // pinRead

void handleKeys() {

  if (key_state != 0) { // If it's not zero, some keys have changed.

    // A bit that reads as 1 means that key has changed state.

    if (bitRead(key_state, 0)) { // Key "register" bit 0 (Encoder 1)
      serial_print("ENC1_KEY - ");

      if (bitRead(currentKeyState, 0)) {
        serial_println("Released");

        stepSizeEnc1 = stepSizeEnc1 * 10;

        // if (stepSizeEnc1 == 2000) {
        //   stepSizeEnc1 = 2;
        // }

        if (stepSizeEnc1 == 1000) {
          stepSizeEnc1 = 1;
        }

        if (tracking) {
          stepSizeEnc2 = stepSizeEnc1;
          displaySelector2();
        }

        displaySelector1();

        serial_print("Step size 1: ");
        serial_println(stepSizeEnc1);

      } else {
        serial_println("Pressed");
      }

    } // enc1

    if (bitRead(key_state, 1)) { // Key "register" bit 0 (Encoder 2)
      serial_print("ENC2_KEY - ");

      if (railSetting == true) {
        serial_println("err - rail, return");
        key_state = 0;
        return;
      }

      if (bitRead(currentKeyState, 1)) {
        serial_println("Released");

        stepSizeEnc2 = stepSizeEnc2 * 10;

        // if (stepSizeEnc2 == 2000) {
        //   stepSizeEnc2 = 2;
        // }

        if (stepSizeEnc2 == 1000) {
          stepSizeEnc2 = 1;
        }

        if (tracking) {
          stepSizeEnc1 = stepSizeEnc2;
          displaySelector1();
        }

        displaySelector2();
        serial_print("Step size 2: ");
        serial_println(stepSizeEnc2);

      } else {
        serial_println("Pressed");
      }

    } // enc2

    // all keys have been checked, reset key_state
    key_state = 0;

  } // End key change detect
}
