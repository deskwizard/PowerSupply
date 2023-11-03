#ifndef _LCD_DEFS_
#define _LCD_DEFS_

// Color picker
// https://www.google.com/search?q=html+color+picker&oq=html+color+picker&aqs=chrome..69i57j0l9.2350j0j7&sourceid=chrome&ie=UTF-8
// Color calculator http://www.rinkydinkelectronics.com/calc_rgb565.php

#include "defines.h"
#include "externs.h"

#define AOFF

//#define LCD_TEST

#include "Adafruit_GFX.h"
#include "Adafruit_ILI9341.h"
#include <Fonts/FreeSans12pt7b.h>
#include <Fonts/FreeSans18pt7b.h>
#include <Fonts/FreeSans24pt7b.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Fonts/FreeSansBold18pt7b.h>
#include <Fonts/FreeSansBold24pt7b.h>
#include <Fonts/FreeSansBold9pt7b.h>

#define X_RES 240 // pixels
#define Y_RES 320

#define ORIGIN                                                                 \
  (Y_RES - BORDERS_H) - 1 // Origin point for drawing channel 2 stuff

#define IND_LOC_X 180 // X position of the Vs and As

#define V1_ROW 76
#define I1_ROW V1_ROW + 49
#define V2_ROW ORIGIN + V1_ROW
#define I2_ROW ORIGIN + I1_ROW

//#define ZONE_COLOR ILI9341_BLUE
#define ZONE_COLOR ILI9341_BLACK

#define BORDERS_H 140
#define BORDER_SIZE 4
#define ROUNDYNESS 12

#define BIG_FONT 1

#define ILI9341_LIGHTBLUE 0x04DF
#define ILI9341_LIGHTORANGE 0xFEE7
#define ILI9341_DARKYELLOW 0x7BE0
#define ILI9341_ORANGE2 0xBB80

//#define COLOR_TRACK   ILI9341_LIGHTORANGE
//#define COLOR_CHAN1   ILI9341_DARKCYAN
//#define COLOR_CHAN2   ILI9341_DARKYELLOW

#define COLOR_TRACK_OFF ILI9341_ORANGE
#define COLOR_TRACK_ON ILI9341_LIGHTORANGE
#define COLOR_CHAN1_OFF ILI9341_DARKCYAN
#define COLOR_CHAN1_ON ILI9341_CYAN
#define COLOR_CHAN2_OFF ILI9341_DARKYELLOW
#define COLOR_CHAN2_ON ILI9341_YELLOW
#define COLOR_HR_OFF ILI9341_PURPLE
#define COLOR_HR_ON ILI9341_MAGENTA

void drawBorders(bool bright); //

void lcdInit0();
void lcdInit();
void displayCh1State();
void displayCh2State();
void displayTitle();
void drawBorders();
void displayRange();
void displaySelector1();
void displaySelector2();
void displayMode();
void displayTracking();
void displayHeaders1();
void displayHeaders2();
void displayUnits1();
void displayUnits2();
void displayUpdateChan1V();
void displayUpdateChan1I();
void displayUpdateChan2V();
void displayUpdateChan2I();

void displaySelfTest();
void displaySelfTestResult(bool result);
void displayExpanderTest(bool result);
void displayDACTest(bool result);

void displayWaitForBT();
void displayConnectedBT();

void displayOTASetup();
void displayOTAFail();
void displayOTASuccess(String ip);
void displayOTAProgress(uint8_t progress);
void displayOTADone();
void displayOTAStarting(String type);
#endif
