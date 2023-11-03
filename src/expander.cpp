#include "MCP23017.h"
#include "control.h"
#include "display.h"

MCP23017 expander(MCP_ADDR);

volatile bool ExpanderIntFlag = false;

void initExpander() {

  pinMode(IN_MCP_INT, INPUT_PULLUP);

  expander.begin();

  expander.portMode(PORT_A, INPUT_PULLUP);
  expander.invertInputPort(PORT_A, true);
  // expander.invertInput(PORT_A, IN_CHAN1_ERR, false);
  /*
    expander.pinMode(PORT_A, IN_VPLUS_EN, INPUT);
    expander.pinMode(PORT_A, IN_VMINUS_EN, INPUT);
    expander.pinMode(PORT_A, IN_TRACKING, INPUT);
    expander.pinMode(PORT_A, IN_RANGE, INPUT);
    expander.pinMode(PORT_A, IN_MODE, INPUT);

    expander.pinMode(PORT_A, IN_VPLUS_ERR, INPUT);
    expander.pinMode(PORT_A, IN_VMINUS_ERR, INPUT);
  */
  // expander.invertInput(PORT_A, IN_MODE, true);

  expander.enableInterrupt(SW_VPLUS_EN, IF_CHANGED);
  expander.enableInterrupt(SW_VMINUS_EN, IF_CHANGED);
  expander.enableInterrupt(SW_TRACKING, IF_CHANGED);
  expander.enableInterrupt(SW_RANGE, IF_CHANGED);
  expander.enableInterrupt(SW_MODE, IF_CHANGED);

  // For testing
  expander.enableInterrupt(IN_CHAN1_ERR, IF_CHANGED);

#ifndef ERR_DISABLE
  expander.enableInterrupt(IN_CHAN1_ERR, IF_CHANGED);
  expander.enableInterrupt(IN_CHAN2_ERR, IF_CHANGED);
#endif

  expander.portMode(PORT_B, OUTPUT);

  expander.digitalWrite(PORT_B, LED_RANGE, getRailSetting());
}

/************************************************************
 *                         Outputs                          *
 ************************************************************/

void setTrackingLED(bool state) {
  expander.digitalWrite(PORT_B, LED_TRACKING_EN, state);
}

void setChannel1StateLED(bool state) {
  expander.digitalWrite(PORT_B, LED_VPLUS_EN, state);
}

void setChannel2StateLED(bool state) {
  expander.digitalWrite(PORT_B, LED_VMINUS_EN, state);
}

void setAllLEDs(bool state) {
  expander.digitalWrite(PORT_B, LED_TRACKING_EN, state);
  expander.digitalWrite(PORT_B, LED_VPLUS_EN, state);
  expander.digitalWrite(PORT_B, LED_VMINUS_EN, state);
}

void setExpanderDebugLED(bool state) {
  expander.digitalWrite(PORT_B, MCP_DEBUG_LED, state);
}

/************************************************************
 *                          Inputs                          *
 ************************************************************/

void handleExpanderInputs() {

  if (ExpanderIntFlag) {

    uint8_t intPin = expander.getInterruptPin();
    bool key_state = expander.getInterruptPinValue(intPin);

    if (intPin == SW_VPLUS_EN) {
      /*
            if (errorFlagVPlus && !chan1_enabled) {
              errorFlagVPlus = false;
              expander.digitalWrite(PORT_B, LED_VPLUS_EN, !chan1_enabled);
              serial_println("V+ err reset");
            } else {
              */
      if (key_state == UP) {
        if (getChannel1ErrorFlag()) {
          clearChannel1ErrorFlag();
          expander.digitalWrite(PORT_B, LED_VPLUS_EN, getChannel1State());
          serial_println("V+ err reset");
          displayCh1State();
        } else {
          toggleChan1State();
        }
      }
      serial_println("V+ carry on");
      //     }

    } // IN_VPLUS_EN

    else if (intPin == SW_VMINUS_EN) {

      if (key_state == UP) {
        if (getRailSetting() == SINGLE) {
          serial_println("Single rail mode, V- not available");
          ExpanderIntFlag = false;
          return;
        }
        /*
              if (errorFlagVMinus && !chan2_enabled) { // error reset?
                errorFlagVMinus = false;
                expander.digitalWrite(PORT_B, LED_VMINUS_EN, !chan2_enabled);
                serial_println("V- err reset");
              } else { // else toggle
        */

        toggleChan2State();

        serial_println("V- carry on");
        //}
      }
    } // IN_VMINUS_EN

    /******************** ERROR INPUTS **************************/

    else if (intPin == IN_CHAN1_ERR) {
      if (key_state == HIGH && getChannel1State() == true && chan1Icode > 0) {
        serial_println("V+ Error!");
        setChannel1ErrorFlag();
        setChan1State(false);
        displayCh1State();
      }
    }
    /*
    else if (intPin == IN_VMINUS_ERR) {
  if (key_state != chan2_enabled) {
    serial_println("V- Error!");
    errorFlagVMinus = true;
    setChan2State(false);
  }
}

        setChan1State(!chan1_enabled);
    setChanState(!chan1_enabled);
*/
    // Reuse for testing
    else if (intPin == IN_CHAN2_ERR) {
      if (key_state == UP) {
        setChan1State(!getChannel1State());
        setChan2State(getChannel1State());
        displayCh2State();
        displayCh1State();
      }
    } else if (intPin == SW_TRACKING) {
      if (key_state == UP) {
        toggleTracking();
      }
    }
    /*
        else if (intPin == IN_RANGE) {
          // serial_println("mode");

          if (key_state == UP) { // key state, pressed or released basically
            toggleRail();
          }
        }
        */
    else if (intPin == SW_MODE) {
      // serial_println("mode key");

      if (key_state == UP) {
        toggleOutputMode();
      }
    }

    else {
      serial_print("   Pin: ");
      serial_print(intPin);

      serial_print("   val: ");
      serial_println(key_state);
    }

    ExpanderIntFlag = false;
  }
}
