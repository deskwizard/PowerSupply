#ifndef _ENCODERS_H_
#define _ENCODERS_H_

#include "defines.h"
#include "externs.h"

void initEncoders();
void handleEncoders();

void IRAM_ATTR getEnc1Pos();
void IRAM_ATTR getEnc2Pos();


void initKeys();
void IRAM_ATTR getEncSwitches();
void handleKeys();

#endif