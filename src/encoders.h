#ifndef _ENCODERS_H_
#define _ENCODERS_H_

#include "LCD_defs.h"
#include "control.h"
#include "defines.h"
#include "externs.h"

void initEncoders();
void handleEncoders();
uint8_t IRAM_ATTR getEnc1Pos();
uint8_t IRAM_ATTR getEnc2Pos();

void initKeys();
void IRAM_ATTR pinRead();
void handleKeys();

#endif