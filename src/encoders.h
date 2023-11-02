#include "LCD_defs.h"
#include "control.h"
#include "defines.h"
#include "externs.h"

void encoder_init();
void handleEncoders();
uint8_t IRAM_ATTR getEnc1Pos();
uint8_t IRAM_ATTR getEnc2Pos();

void keys_init();
void IRAM_ATTR pinRead();
void handleKeys();
