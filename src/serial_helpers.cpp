#include "analog.h"
#include "control.h"
#include "defines.h"
#include "externs.h"
/*

  TODO:
  - Make it so we can use the serial commands from either serials (nicely...).

    ******** Warning:
    To print floats, you have to specify the number of digits,
    otherwise it default to calling the int version and won't print as a
  float... I have no clue what I'm doing...
*/

float testFloat = 0.0;
uint16_t test_code = 0;

void handleSerial() {

  if (Serial.available()) {

    unsigned char c = Serial.read();

    if (c == 'h') {

      serial_println();
      serial_println("Help");
      serial_println("----------------------------------------------");
      serial_println("Command       Function");
      serial_println();
      serial_println("  a           Disable channel 1 output");
      serial_println("  A           Enable channel 1 output");
      serial_println("  b           Disable channel 2 output");
      serial_println("  B           Enable channel 2 output");
      serial_println("  t           Toggle tracking");
      serial_println("  m           Toggle mode (cc/cv)");
      serial_println("  r           Toggle rail");
      serial_println();
      serial_println("  f           Disable both channels");
      serial_println("  F           Enable both channels");
      serial_println();
      serial_println("  s<code>     Set channel 1 V to <code>");
      serial_println("  n<code>     Set channel 2 V to <code>");
      serial_println("  o<code>     Set channel 1 I to <code>");
      serial_println("  l<code>     Set channel 2 I to <code>");
      serial_println();

      return;
    }

    switch (c) {

    case 'A':

      setChan1State(true);

      if (getTrackingMode()) {
        setChan2State(true);
        displayCh2State();
      }

      displayCh1State();
      break;

    case 'a':
      setChan1State(false);

      if (getTrackingMode()) {
        setChan2State(false);
        displayCh2State();
      }

      displayCh1State();
      break;

    case 'b':
      if (!getTrackingMode() && getRailSetting() == DUAL) {
        setChan2State(false);
        displayCh2State();
      } else {
        serial_println("no.");
      }
      break;

    case 'B':
      if (!getTrackingMode() && getRailSetting() == DUAL) {
        setChan2State(true);
        displayCh2State();
      } else {
        serial_println("no.");
      }
      break;

    case 'F':
      setChan1State(true);
      setChan2State(true);
      displayCh2State();
      displayCh1State();
      break;

    case 'f':
      setChan1State(false);
      setChan2State(false);
      displayCh2State();
      displayCh1State();
      break;

    case 't':
      toggleTracking();
      break;

    case 'r':
      // toggleRail();
      break;

    case 'm':
      toggleOutputMode();
      break;

    case 's':
      test_code = Serial.parseInt();

      if (test_code > MAX_V_CODE) {
        test_code = MAX_V_CODE;
      }

      setChan1V(test_code);

      if (getTrackingMode()) {
        setChan2V(test_code);
        displayUpdateChan2V();
      }

      displayUpdateChan1V();
      break;

    case 'n':
      if (getTrackingMode() || (getRailSetting() == SINGLE)) {
        break;
      }

      test_code = Serial.parseInt();

      if (test_code > MAX_V_CODE) {
        test_code = MAX_V_CODE;
      }

      setChan2V(test_code);

      displayUpdateChan2V();

      break;

    case 'o':
      test_code = Serial.parseInt();

      if (test_code > MAX_I_CODE) {
        test_code = MAX_I_CODE;
      }

      setChan1I(test_code);

      displayUpdateChan1I();

      break;

    case 'l':
      test_code = Serial.parseInt();

      if (test_code > MAX_I_CODE) {
        test_code = MAX_I_CODE;
      }

      setChan2I(test_code);

      displayUpdateChan2I();

      break;

    } // switch end
  }
}
/*
void handleSerial() {

  uint8_t c = 0;

  if (Serial.available()) {
    c = Serial.read();
    reactSerial(c);
  }

  if (BT_Serial.available()) {
    c = BT_Serial.read();
    reactSerial(c);
  }
}
*/
/*
void handleSerial() {

  if (Serial.available()) {

    unsigned char c = Serial.read();

    switch (c) {

    case '+':
      chan1Vcode = Serial.parseInt();

      if (chan1Vcode > MAX_V_CODE) {
        chan1Vcode = MAX_V_CODE;
      }

      serial_println();
      serial_print("V+: ");
      serial_println(chan1Vcode);

      setChan1V();

      if (tracking) {
        chan2Vcode = chan1Vcode;

        serial_print("--- >>>> V-: ");
        serial_println(chan2Vcode);
        setChan2V();
      }
      break;

    case 'A':
      setChan1State(true);
      break;
    case 'a':
      setChan1State(false);
      break;

    case '-':

      if (!tracking && railSetting == DUAL) {
        chan2Vcode = Serial.parseInt();

        if (chan2Vcode > MAX_V_CODE) {
          chan2Vcode = MAX_V_CODE;
        }

        serial_println();
        serial_print("V-: ");
        serial_println(chan2Vcode);
        setChan2V();
      }

      break;

    case 'b':
      if (!tracking && railSetting == DUAL) {
        setChan2State(false);
      } else {
        serial_println("no.");
      }
      break;

    case 'B':
      if (!tracking && railSetting == DUAL) {
        setChan2State(true);
      } else {
        serial_println("no.");
      }
      break;

    case 't':
      toggleTracking();
      break;

    case 'p':
      testFloat = Serial.parseFloat();

      test_code =
          (((testFloat / OUT_GAIN) / float(VREF_MV / 4095.0)) * 1000) + 0.5;

      serial_print(testFloat, 2);
      serial_print("  -  ");
      serial_println(test_code);

      chan1Vcode = test_code;

      setChan1V();

      break;

    case '=':
      ESP.restart();
      break;

    case 'i':
      runSelfTest();
      break;
    }
  }
}
*/
/*
void handleBT_Serial() {

  if (BT_Serial.available()) {

    unsigned char c = BT_Serial.read();

    switch (c) {

    case '+':
      chan1Vcode = BT_Serial.parseInt();

      if (chan1Vcode > MAX_V_CODE) {
        chan1Vcode = MAX_V_CODE;
      }

      serial_println();
      serial_print("V+: ");
      serial_println(chan1Vcode);
      setChan1V();

      if (tracking) {
        chan2Vcode = chan1Vcode;
        setChan2V();
        serial_print("--- >>>> V-: ");
        serial_println(chan2Vcode);
      }
      break;

    case 'A':
      setChan1State(true);
      break;
    case 'a':
      setChan1State(false);
      break;

    case '-':

      if (!tracking && railSetting == DUAL) {
        chan2Vcode = BT_Serial.parseInt();

        if (chan2Vcode > MAX_V_CODE) {
          chan2Vcode = MAX_V_CODE;
        }

        serial_println();
        serial_print("V-: ");
        serial_println(chan2Vcode);
        setChan2V();
      }

      break;

    case 'b':
      if (!tracking && railSetting == DUAL) {
        setChan2State(false);
      } else {
        serial_println("no.");
      }
      break;

    case 'B':
      if (!tracking && railSetting == DUAL) {
        setChan2State(true);
      } else {
        serial_println("no.");
      }
      break;

    case 't':
      toggleTracking();
      break;

    case 'p':
      testFloat = BT_Serial.parseFloat();

      test_code =
          (((testFloat / OUT_GAIN) / float(VREF_MV / 4095.0)) * 1000) + 0.5;

      serial_print(testFloat, 2);
      serial_print("  -  ");
      serial_println(test_code);

      chan1Vcode = test_code;

      setChan1V();
      break;

    case 'i':
      runSelfTest();
      break;
    }
  }
}
*/
void serial_print(const char *data) {
  Serial.print(data);
  BT_Serial.print(data);
}

void serial_println(const char *data) {
  serial_print(data);
  serial_println();
}

void serial_print(const int16_t data) {
  Serial.print(data);
  BT_Serial.print(data);
}

void serial_println(const int16_t data) {
  serial_print(data);
  serial_println();
}

void serial_print(const float data, uint8_t digits) {
  Serial.print(data, digits);
  BT_Serial.print(data, digits);
}

void serial_println(const float data, uint8_t digits) {
  serial_print(data, digits);
  serial_println();
}

void serial_println() {
  Serial.println();
  BT_Serial.println();
}

void printChan1V() {

  uint16_t outputValue =
      (VREF_MV / 4096) * (chan1Vcode - CHAN1V_OFFSET) * OUT_GAIN;

  serial_print(" Chan1V - code: ");
  serial_print(chan1Vcode);

  /*
    if (railSetting == SINGLE) { // Our gain is 10x on single rail instead of
                                 // 5x, so double the value
      outputValue = outputValue * 2;
    }
  */

  serial_print(" value: ");
  serial_print(float(outputValue / 1000.0), 2);
  serial_println("V");
}

void printChan2V() {

  uint16_t outputValue =
      (VREF_MV / 4096) * (chan2Vcode - CHAN2V_OFFSET) * OUT_GAIN;

  serial_print("Chan2V - code: ");
  serial_print(chan2Vcode);

  serial_print(" value: -");
  serial_print(float(outputValue / 1000.0), 2);
  serial_println("V");
}

void printChan2I() {
  serial_print("Chan2I - code: ");
  serial_print(chan2Icode);

  serial_print(" value: ");
  serial_print(float(chan2Icode / 1000.0), 3);
  serial_println("A");
}

void printChan1I() {
  serial_print("Chan1I - code: ");
  serial_print(chan1Icode);

  serial_print(" value: -");
  serial_print(float(chan1Icode / 1000.0), 3);
  serial_println("A");
}

void printDebugPrefixLCD(const char *data) {
  serial_print("-----------LCD ------->");
  serial_println(data);
  serial_println();
}