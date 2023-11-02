#include "defines.h"
#include "encoders.h"
#include "externs.h"

hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR onTimer() { // Needs to be before setup()

  portENTER_CRITICAL_ISR(&timerMux);

  // Rotary encoder(s) read functions
  currentEnc1Pos = getEnc1Pos();
  currentEnc2Pos = getEnc2Pos();

  // Key(s) debouncing routine
  pinRead();

  // Check for interrupt signal from MCP23017
  ExpanderIntFlag = !digitalRead(IN_MCP_INT);

  portEXIT_CRITICAL_ISR(&timerMux);
}



// void input_init() {
//
//  encoder_init();
//  expander_init();
//
//  delay(100);     // Wait a bit for things to stabilize
//
//  // Now that all the IO is taken care of, configure and start the timer
//  timer_init();
//
//}

void timer_init() {

  // Setup and start timer interrupt
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000, true);
  timerAlarmEnable(timer);
}
