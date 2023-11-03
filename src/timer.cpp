#include "defines.h"
#include "encoders.h"
#include "externs.h"

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() { // Needs to be before setup()

  portENTER_CRITICAL_ISR(&timerMux);

  // Rotary encoders rotation
  getEnc1Pos();
  getEnc2Pos();

  // Rotary encoders switches
  getEncSwitches();

  // Check for interrupt signal from MCP23017
  ExpanderIntFlag = !digitalRead(IN_MCP_INT);

  portEXIT_CRITICAL_ISR(&timerMux);
}

void initTimer() {

  // Setup and start timer interrupt
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
}
