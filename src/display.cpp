#include "display.h"
#include "control.h"

Adafruit_ILI9341 lcd = Adafruit_ILI9341(LCD_CS, LCD_DC, LCD_RST);

GFXcanvas1 canvasChan1V(115, 40); // 115x40 pixel canvas
GFXcanvas1 canvasChan1I(115, 40); // 115x40 pixel canvas
GFXcanvas1 canvasChan2V(132, 40); // 115x40 pixel canvas
GFXcanvas1 canvasChan2I(132, 40); // 115x40 pixel canvas

// I'm going to have to rework this because colors are a mess now...
// const uint32_t colors[3][3] = {
//  {ILI9341_BLUE, ILI9341_PURPLE, ILI9341_BLUE},   // Unused at the moment
//  {COLOR_CHAN1, COLOR_TRACK, ILI9341_WHITE},       // Channel 1 colors
//  (tracking/not tracking) {COLOR_CHAN2, COLOR_TRACK, ILI9341_DARKGREY} //
//  Channel 1 colors (tracking/not tracking)
//};

// lcd debug
float fakeReading1I = 0.0;
float fakeReading2I = 0.0;

void lcdInit0() {

  // printDebugPrefixLCD("lcdInit0()");

  lcd.begin(80000000);

  lcd.setRotation(0);

  displayTitle();
}

void lcdInit() {

  // printDebugPrefixLCD("lcdInit()");

  lcd.setTextColor(ILI9341_GREEN);
  lcd.setFont(&FreeSansBold18pt7b);
  lcd.setCursor(40, 270);
  lcd.print("Starting!");
  delay(2000);

  drawBorders();

  displaySelector1();
  displaySelector2();

  displayMode();

  displayCh1State();
  displayCh2State();
}

void displayCh1State() {

  lcd.fillRect(100, 10, 50, 20, ZONE_COLOR);

  lcd.setCursor(100, 28);
  lcd.setFont(&FreeSansBold12pt7b);
  lcd.setTextSize(0);

  /******************************************************************************/

  if (getChannel1State()) {
    if (getTrackingMode()) {
      lcd.setTextColor(COLOR_TRACK_ON);
    } else if (getRailSetting()) {
      lcd.setTextColor(COLOR_HR_ON);
    } else {
      lcd.setTextColor(COLOR_CHAN1_ON);
    }
    lcd.print("ON");
  } else {
    if (getTrackingMode()) {
      lcd.setTextColor(COLOR_TRACK_OFF);
    } else if (getRailSetting()) {
      lcd.setTextColor(COLOR_HR_OFF);
    } else {
      lcd.setTextColor(COLOR_CHAN1_OFF);
    }
    lcd.print("OFF");
  }

  displayHeaders1();
  displayUnits1();
  displayUpdateChan1V();
  displayUpdateChan1I();
}

void displayCh2State() {

  // maybe not the best place for this check, works for now
  // stops the rail switch from triggering printing the numbers
  // before drawing the borders (and again over it)
  if (getRailSetting() == SINGLE) {
    return;
  }

  lcd.fillRect(100, ORIGIN + 10, 50, 20, ZONE_COLOR);

  lcd.setCursor(100, ORIGIN + 28);
  lcd.setFont(&FreeSansBold12pt7b);
  lcd.setTextSize(0);

  if (getChannel2State()) {
    if (getTrackingMode()) {
      lcd.setTextColor(COLOR_TRACK_ON);
    } else if (!getRailSetting()) {
      lcd.setTextColor(COLOR_CHAN2_ON);
    }
    lcd.print("ON");
  } else {
    if (getTrackingMode()) {
      lcd.setTextColor(COLOR_TRACK_OFF);
    } else if (!getRailSetting()) {
      lcd.setTextColor(COLOR_CHAN2_OFF);
    }
    lcd.print("OFF");
  }

  displayHeaders2();
  displayUnits2();
  displayUpdateChan2V();
  displayUpdateChan2I();
}

void displayTracking() {

  drawBorders();

  // Smallest possible rectangle to erase
  lcd.fillRect(60, 150, 105, 25, ZONE_COLOR);

  if (getTrackingMode()) {

    // Just show the tracking indicator if we are tracking
    lcd.setTextColor(ILI9341_WHITE);

    lcd.setFont(&FreeSansBold12pt7b);
    lcd.setTextSize(0);

    lcd.setCursor(70, 168);
    lcd.print("Tracking");

    // Restore the font so we don't have to do it everytime we update the values
    lcd.setFont(&FreeSansBold24pt7b);
    lcd.setTextSize(BIG_FONT);
  }

  displayCh1State();
  displayCh2State();

  displaySelector1();
  displaySelector2();

  displayMode();
}

void displayRange() {

  drawBorders();

  // Smallest possible rectangle to erase
  lcd.fillRect(60, 150, 105, 25, ZONE_COLOR);

  if (getRailSetting() == SINGLE) {
    /*
        lcd.fillTriangle(20, 170, 35, 148, 50, 170, ILI9341_YELLOW);
        lcd.fillTriangle(195, 170, 210, 148, 225, 170, ILI9341_YELLOW);
    */
    // Just show the high range indicator
    lcd.setTextColor(ILI9341_WHITE);

    lcd.setFont(&FreeSansBold12pt7b);
    lcd.setTextSize(0);

    lcd.setCursor(60, 168);
    lcd.print("High range");
    /*
        lcd.setTextColor(ILI9341_RED);
        lcd.setCursor(30, 168);
        lcd.print("!");
        lcd.setCursor(205, 168);
        lcd.print("!");

    */
    // Restore the font so we don't have to do it everytime we update the values
    lcd.setFont(&FreeSansBold24pt7b);
    lcd.setTextSize(BIG_FONT);
  }

  displaySelector1();
  displayCh1State();

  if (getRailSetting() == DUAL) {
    displaySelector2();
    displayCh2State();
  }

  displayMode();
}
void redrawChan1Borders() {}

void drawBorders() {

  if (getRailSetting()) {
    lcd.fillScreen(ILI9341_BLACK);

    lcd.fillRoundRect(0, 0, X_RES, BORDERS_H, ROUNDYNESS, COLOR_HR_OFF);
    lcd.fillRoundRect(BORDER_SIZE, BORDER_SIZE, X_RES - (2 * BORDER_SIZE),
                      BORDERS_H - (2 * BORDER_SIZE), ROUNDYNESS, ILI9341_BLACK);

    lcd.fillRoundRect(0, ORIGIN, X_RES, BORDERS_H, ROUNDYNESS,
                      ILI9341_DARKGREY);
    lcd.fillRoundRect(BORDER_SIZE, ORIGIN + BORDER_SIZE,
                      X_RES - (2 * BORDER_SIZE), BORDERS_H - (2 * BORDER_SIZE),
                      ROUNDYNESS, ILI9341_BLACK);

  } else if (getTrackingMode()) {
    uint16_t color = COLOR_TRACK_OFF;

    // One color fill + 1 smaller black fill  = border
    lcd.fillRoundRect(0, 0, X_RES, Y_RES, ROUNDYNESS, color);
    lcd.fillRoundRect(BORDER_SIZE, BORDER_SIZE, X_RES - (2 * BORDER_SIZE),
                      Y_RES - (2 * BORDER_SIZE), ROUNDYNESS, ILI9341_BLACK);

    // the fancy bar that's displays between CH1 and CH2
    lcd.fillRoundRect(12, 15, 30, 185, 15, color);
    lcd.fillRoundRect(17, 20, 40, 175, 15, ILI9341_BLACK);
    lcd.fillRect(27, 10, 20, 200, ZONE_COLOR); // clip it before text

  }

  else {

    // Lets do something to remove the 2 pink bars both sides
    lcd.fillScreen(ILI9341_BLACK);

    lcd.fillRoundRect(0, 0, X_RES, BORDERS_H, ROUNDYNESS, COLOR_CHAN1_OFF);
    lcd.fillRoundRect(BORDER_SIZE, BORDER_SIZE, X_RES - (2 * BORDER_SIZE),
                      BORDERS_H - (2 * BORDER_SIZE), ROUNDYNESS, ILI9341_BLACK);

    lcd.fillRoundRect(0, ORIGIN, X_RES, BORDERS_H, ROUNDYNESS, COLOR_CHAN2_OFF);
    lcd.fillRoundRect(BORDER_SIZE, ORIGIN + BORDER_SIZE,
                      X_RES - (2 * BORDER_SIZE), BORDERS_H - (2 * BORDER_SIZE),
                      ROUNDYNESS, ILI9341_BLACK);
  }

  displayHeaders1();
  displayHeaders2();
}

void displayHeaders1() {

  lcd.setFont(&FreeSansBold12pt7b);
  lcd.setTextSize(0);

  if (getChannel1State()) {
    if (getTrackingMode()) {
      lcd.setTextColor(COLOR_TRACK_ON);
    } else if (getRailSetting()) {
      lcd.setTextColor(COLOR_HR_ON);
    } else {
      lcd.setTextColor(COLOR_CHAN1_ON);
    }
  } else {
    if (getTrackingMode()) {
      lcd.setTextColor(COLOR_TRACK_OFF);
    } else if (getRailSetting()) {
      lcd.setTextColor(COLOR_HR_OFF);
    } else {
      lcd.setTextColor(COLOR_CHAN1_OFF);
    }
  }

  lcd.setCursor(30, 28);
  lcd.print("CH1");

  displayUnits1();
}

void displayHeaders2() {

  if (getRailSetting()) {
    lcd.setTextColor(ILI9341_DARKGREY);

    lcd.setFont(&FreeSansBold18pt7b);
    lcd.setTextSize(1);

    lcd.setCursor(20, V2_ROW);
    lcd.print("Unavailable");
  } else {

    displayUnits2();

    if (getChannel2State()) {
      if (getTrackingMode()) {
        lcd.setTextColor(COLOR_TRACK_ON);
      } else if (getRailSetting()) {
        lcd.setTextColor(COLOR_HR_ON);
      } else {
        lcd.setTextColor(COLOR_CHAN2_ON);
      }
    } else {
      if (getTrackingMode()) {
        lcd.setTextColor(COLOR_TRACK_OFF);
      } else if (getRailSetting()) {
        lcd.setTextColor(COLOR_HR_OFF);
      } else {
        lcd.setTextColor(COLOR_CHAN2_OFF);
      }
    }
  }

  lcd.setFont(&FreeSansBold12pt7b);
  lcd.setTextSize(0);
  lcd.setCursor(30, ORIGIN + 28);
  lcd.print("CH2");
}

void displayUnits1() {

  lcd.setFont(&FreeSansBold24pt7b);
  lcd.setTextSize(BIG_FONT);

  if (getChannel1State()) {
    lcd.setTextColor(ILI9341_GREEN);
  } else {
    lcd.setTextColor(ILI9341_DARKGREEN);
  }

  lcd.setCursor(IND_LOC_X, V1_ROW);
  lcd.print('V');

  lcd.setCursor(IND_LOC_X, I1_ROW);
  lcd.print('A');
}

void displayUnits2() {

  lcd.setFont(&FreeSansBold24pt7b);
  lcd.setTextSize(BIG_FONT);

  if (getChannel2State()) {
    lcd.setTextColor(ILI9341_GREEN);
  } else {
    lcd.setTextColor(ILI9341_DARKGREEN);
  }

  lcd.setCursor(IND_LOC_X, V2_ROW);
  lcd.print('V');

  lcd.setCursor(IND_LOC_X, I2_ROW);
  lcd.print('A');
}

void displayUpdateChan1V() {

  uint32_t foreground = ILI9341_DARKGREEN;

  uint16_t outputValue;

  // uint16_t outputValue =
  //     (VREF_MV / 4096) * (chan1Vcode - CHAN1V_OFFSET) * OUT_GAIN;

  /*
    if (getRailSetting() == SINGLE) { // Our gain is 10x on single rail instead
    of 5x,
                                 // so double the value
      outputValue = outputValue * 2;
    }
  */
  if (getOutputMode() == MODE_CV) {
    canvasChan1V.setFont(&FreeSansBold24pt7b);
  } else {
    canvasChan1V.setFont(&FreeSans24pt7b);
  }

  canvasChan1V.fillScreen(ILI9341_BLACK);
  canvasChan1V.setCursor(0, 38);

  if (getChannel1State()) {
    foreground = ILI9341_GREEN;
    // outputValue = chan1VRead;
    outputValue = (VREF_MV / 4096) * chan1VRead * OUT_GAIN;
    // canvasChan1V.print(float(outputValue / 1000.0), 2);
  } else {
    outputValue = (VREF_MV / 4096) * (chan1Vcode - CHAN1V_OFFSET) * OUT_GAIN;
  }

  if (outputValue < 10000) { // in mV, so 10000 = 10V
    canvasChan1V.print("  ");
  }

  canvasChan1V.print(float(outputValue / 1000.0), 2);

  lcd.drawBitmap(60, 37, canvasChan1V.getBuffer(), 115, 40, foreground,
                 ILI9341_BLACK); // Copy to screen
}

void displayUpdateChan1I() {

  uint32_t foreground = ILI9341_DARKGREEN;

  canvasChan1I.fillScreen(ILI9341_BLACK);

  if (getOutputMode() == MODE_CC) {
    canvasChan1I.setFont(&FreeSansBold24pt7b);
  } else {
    canvasChan1I.setFont(&FreeSans24pt7b);
  }

  // Needs to be here, else the cursor is wrong the first time
  // the function is called. (wtf...)
  canvasChan1I.setCursor(0, 38);
  /*
    // Always show the code for now (unless it's 0)
    if (chan1Icode >= 0) {
      canvasChan1I.print(float(chan1Icode / 1000.0), 3);
    } else {
      canvasChan1I.print("--------");
      // TODO wipe indicator, show on 0 crossing upwards
    }
  */
  if (getChannel1ErrorFlag() == true) {
    foreground = ILI9341_RED;
    canvasChan1I.setFont(&FreeSansBold18pt7b);
    canvasChan1I.print("LIMIT");
  } else if (getChannel1State()) {
    foreground = ILI9341_GREEN;
    canvasChan1I.print(float(chan1IRead / 1000.0), 3);
  } else {
    if (chan1Icode >= 0) {
      canvasChan1I.print(float(chan1Icode / 1000.0), 3);
    } else {
      canvasChan1I.print("--------");
      // TODO wipe indicator, show on 0 crossing upwards
    }
  }

  /*
    if (chan1_enabled == false) {
      if (modeSetting == MODE_CV) {
        canvasChan1I.print("--------");
      } else {
        canvasChan1I.print(float(chan1Icode / 1000.0), 3);
      }

    } else {
      foreground = ILI9341_GREEN;
      // Print fake number for the actual current reading if enabled
      canvasChan1I.print(fakeReading1I, 3);
    }
  */

  lcd.drawBitmap(60, 86, canvasChan1I.getBuffer(), 115, 40, foreground,
                 ILI9341_BLACK); // Copy to screen
}

void displayUpdateChan2V() {

  uint32_t foreground = ILI9341_DARKGREEN;

  uint16_t outputValue =
      (VREF_MV / 4096) * (chan2Vcode - CHAN2V_OFFSET) * OUT_GAIN;

  if (getChannel2State()) {
    foreground = ILI9341_GREEN;
  }

  if (getOutputMode() == MODE_CV) {
    canvasChan2V.setFont(&FreeSansBold24pt7b);
  } else {
    canvasChan2V.setFont(&FreeSans24pt7b);
  }

  canvasChan2V.fillScreen(ILI9341_BLACK);
  canvasChan2V.setCursor(0, 38);

  canvasChan2V.print('-'); // padding is easier if we hardcode the - instead,
                           // else it "moves around"

  if (outputValue < 10000) { // in mV, so 10000 = 10V
    canvasChan2V.print("0");
  }

  canvasChan2V.print(float(outputValue / 1000.0), 2);

  lcd.drawBitmap(40, V2_ROW - 38, canvasChan2V.getBuffer(), 132, 40, foreground,
                 ILI9341_BLACK); // Copy to screen
}

void displayUpdateChan2I() {

  uint32_t foreground = ILI9341_DARKGREEN;

  canvasChan2I.fillScreen(ILI9341_BLACK);

  if (getOutputMode() == MODE_CC) {
    canvasChan2I.setFont(&FreeSansBold24pt7b);
  } else {
    canvasChan2I.setFont(&FreeSans24pt7b);
  }

  // Needs to be here, else the cursor is wrong the first time
  // the function is called. (wtf...)
  canvasChan2I.setCursor(0, 38);

  canvasChan2I.print('-');

  // Always show the code for now
  canvasChan2I.print(float(chan2Icode / 1000.0), 3);

  if (getChannel2State()) {
    foreground = ILI9341_GREEN;
  }

  /*
    if (chan2_enabled == false) {
      if (modeSetting == MODE_CV) {
        canvasChan2I.print("--------");
      } else {
        canvasChan2I.print(float(chan2Icode / 1000.0), 3);
      }

    } else {
      foreground = ILI9341_GREEN;
      // Print fake number for the actual current reading if enabled
      canvasChan2I.print(fakeReading2I, 3);
    }

  */
  lcd.drawBitmap(40, I2_ROW - 38, canvasChan2I.getBuffer(), 132, 40, foreground,
                 ILI9341_BLACK); // Copy to screen
}

void displaySelector1() {

  static uint16_t v_origin = V1_ROW + 5;
  uint16_t h_origin = 152;
  uint8_t length = 24;

  if (getOutputMode() == MODE_CC) {
    if (v_origin == V1_ROW + 5) { // Wipe the previous indicator
      lcd.fillRect(60, v_origin, 120, 5, ZONE_COLOR);
    }
    v_origin = I1_ROW + 5;
  } else {                        // We're in CV mode
    if (v_origin == I1_ROW + 5) { // Wipe the previous indicator
      lcd.fillRect(60, v_origin, 120, 5, ZONE_COLOR);
    }
    v_origin = V1_ROW + 5;
  }

  // Wipe current indicator
  lcd.fillRect(60, v_origin, 120, 5, ZONE_COLOR);

  if (stepSizeEnc1 == 100) {
    h_origin = 62;
    length = 50;
    if (getOutputMode() == MODE_CC) {
      length = 60;
    }

  } else if (stepSizeEnc1 == 10) {
    h_origin = 126;
  }

  for (uint8_t x = 0; x <= 3; x++) {
    lcd.drawFastHLine(h_origin, v_origin + x, length, ILI9341_WHITE);
  }
}

void displaySelector2() {

  static uint16_t v_origin = V2_ROW + 5;
  uint16_t h_origin = 148;
  uint8_t length = 24;

  if (getOutputMode() == MODE_CC) {
    if (v_origin == V2_ROW + 5) { // Wipe the previous indicator
      lcd.fillRect(56, v_origin, 120, 5, ZONE_COLOR);
    }
    v_origin = I2_ROW + 5;
  } else {                        // We're in CV mode
    if (v_origin == I2_ROW + 5) { // Wipe the previous indicator
      lcd.fillRect(56, v_origin, 120, 5, ZONE_COLOR);
    }
    v_origin = V2_ROW + 5;
  }

  // Wipe current indicator
  lcd.fillRect(56, v_origin, 120, 5, ZONE_COLOR);

  if (stepSizeEnc2 == 100) {
    h_origin = 58;
    length = 50;
    if (getOutputMode() == MODE_CC) {
      length = 60;
    }

  } else if (stepSizeEnc2 == 10) {
    h_origin = 122;
  }

  for (uint8_t x = 0; x <= 3; x++) {
    lcd.drawFastHLine(h_origin, v_origin + x, length, ILI9341_WHITE);
  }
}

void displayMode() {

  // lcd.fillRect(190, ORIGIN + 10, 35, 20, ZONE_COLOR);
  // lcd.fillRect(190, 10, 35, 20, ZONE_COLOR);
  // lcd.fillRect(60, 150, 105, 25, ZONE_COLOR);
  lcd.fillRect(190, 150, 35, 20, ILI9341_BLACK);

  lcd.setFont(&FreeSansBold12pt7b);
  lcd.setTextSize(0);
  // lcd.setCursor(190, 28);
  // lcd.setCursor(70, 168);
  lcd.setCursor(190, 168);

  if (getOutputMode() == MODE_CV) {
    lcd.setTextColor(ILI9341_PINK);
    lcd.print(" V");
  } else {
    lcd.setTextColor(ILI9341_RED);
    lcd.print(" C");
  }
}

/************************ "Printing" ************************/

void displayTitle() {

  lcd.fillScreen(ILI9341_BLACK);

  lcd.setFont(&FreeSansBold24pt7b);

  lcd.setTextColor(ILI9341_CYAN);
  lcd.setCursor(20, V1_ROW);
  lcd.print("P");
  lcd.setTextColor(ILI9341_RED);
  lcd.print("O");
  lcd.setTextColor(ILI9341_MAGENTA);
  lcd.print("A");
  lcd.setTextColor(ILI9341_ORANGE2);
  lcd.print("PSU");

  // Restore font
  lcd.setFont(&FreeSansBold12pt7b);
  lcd.setTextSize(0);
}

void displayPrintOK() {
  lcd.setTextColor(ILI9341_GREEN);
  lcd.println("OK!");
}

void displayPrintFail() {
  lcd.setTextColor(ILI9341_RED);
  lcd.println("Fail!");
}

/************************ Self Test ************************/

void displaySelfTest() {
  lcd.setTextColor(ILI9341_WHITE);
  lcd.setCursor(10, 130);
  lcd.print("Starting self test...");
}

void displaySelfTestResult(bool result) {

  if (result != 0) {
    lcd.setTextColor(ILI9341_RED);
    lcd.setCursor(33, 248);
    lcd.print("Self test failed!");
  } else {
    lcd.setTextColor(ILI9341_GREEN);
    lcd.setCursor(23, 248);
    lcd.print("Self test passed!");
  }
}

void displayExpanderTest(bool result) {

  lcd.setCursor(25, 165);
  lcd.print("Expander... ");

  if (result == false) {
    lcd.setCursor(165, 165);
    displayPrintFail();
  } else {
    lcd.setCursor(170, 165);
    displayPrintOK();
  }
}

void displayDACTest(bool result) {

  // reset color this time
  lcd.setTextColor(ILI9341_WHITE);

  lcd.setCursor(25, 195);
  lcd.print("DAC...");

  if (result == false) {
    lcd.setCursor(165, 195);
    displayPrintFail();
  } else {
    lcd.setCursor(170, 195);
    displayPrintOK();
  }
}

/************************ BT ************************/

void displayWaitForBT() {
  lcd.fillRect(10, 128, 200, 55, ILI9341_BLACK);
  lcd.setTextColor(ILI9341_LIGHTBLUE);
  lcd.setCursor(10, 148);
  lcd.print("Waiting for BT...");
}

void displayConnectedBT() {
  lcd.setTextColor(ILI9341_GREEN);
  lcd.setCursor(30, 178);
  lcd.print("BT Connected!");
}

/************************ OTA ************************/

void displayOTASetup() {
  lcd.setTextColor(ILI9341_GREENYELLOW);
  lcd.setCursor(10, 130);
  lcd.print("Setting up OTA... ");

  lcd.setCursor(25, 165);
  lcd.print("OTA: ");
}

void displayOTAFail() {
  lcd.setTextColor(ILI9341_RED);
  lcd.print("Failed!");
}

void displayOTASuccess(String ip) {

  lcd.setCursor(150, 165);
  displayPrintOK();

  lcd.setCursor(25, 195);
  lcd.setTextColor(ILI9341_GREENYELLOW);
  lcd.print("IP: ");

  lcd.setCursor(85, 195);
  lcd.setTextColor(ILI9341_GREEN);
  lcd.println(ip);
}

void displayOTAProgress(uint8_t progress) {
  lcd.fillRect(100, 180, 48, 22, ILI9341_BLACK);
  lcd.setCursor(100, 198);
  lcd.print(progress);
  lcd.print("%");
}

void displayOTADone() {
  lcd.setTextColor(ILI9341_GREEN);
  lcd.fillRect(100, 180, 65, 22, ILI9341_BLACK);
  lcd.setCursor(75, 198);
  lcd.print("Done!");
}

void displayOTAStarting(String type) {
  lcd.fillScreen(ILI9341_BLACK);
  displayTitle();

  lcd.setFont(&FreeSansBold12pt7b);
  lcd.setTextSize(0);
  lcd.setTextColor(ILI9341_YELLOW);
  lcd.setCursor(10, 168);
  lcd.print("Updating ");
  lcd.print(type);
  lcd.println("...");
  // lcd.setTextSize(1);
  lcd.setTextColor(ILI9341_RED);
}