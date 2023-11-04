
// I sometimes get a V+ encoder signal at boot, investigate....
// Maybe the cap across it doesn't have time to charge or something?
// Would a delay help?

#include "analog.h"
#include "control.h"
#include "defines.h"
#include "display.h"
#include "encoders.h"
#include "expander.h"
#include <Arduino.h>

void setup() {

  //  TODO: retest with OTA and bluetooth to be sure it's still fine
  //  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  //  esp_task_wdt_add(NULL); //add current thread to WDT watch

  Serial.begin(115200);
  Serial.setTimeout(100); // Timeout value for parseInt and stuff

  Wire.begin(I2C_SDA, I2C_SCL); // SDA, SCL
  Wire.setClock(400000);        // has to be after begin().

  // scanI2C();

  initDAC();
  initADC();

  // Serial.print("setup() running on core ");
  // Serial.println(xPortGetCoreID());

  pinMode(DEBUG_LED, OUTPUT);
  digitalWrite(DEBUG_LED, HIGH);

  lcdInit0(); // Just initialize, don't show anything yet

#ifdef LCD_TEST

  railSetting = SINGLE;

  while (true) {
    drawBorders(false);
    delay(3000);
    drawBorders(true);
    delay(3000);
  };

#endif

  initKeys();

  serial_println("Sketch starting... ");
  serial_println();

  pinMode(OUT_VPLUS, OUTPUT);
  pinMode(OUT_VMINUS, OUTPUT);
  pinMode(OUT_RANGE, OUTPUT);

  // runSelfTest();

  // initBT();
  // startBT();

  // initOTA();

  initEncoders();
  initExpander();
  initTimer();

  setAllLEDs(HIGH);

  lcdInit(); // has a 2sec delay for boot screen

  setAllLEDs(LOW);
  digitalWrite(DEBUG_LED, LOW);

  serial_println("Ready");
  serial_println();

  while(1);
}

void loop() {

  // handleOTA();

  handleKeys();
  handleExpanderInputs();
  handleEncoders();
  handleAnalog();
  handleDisplay();
  handleSerial();

  // debug
  blinking();
  // testAnalog();
}

void blinking() {

  uint32_t currentMillis = millis();
  static uint32_t previousMillis = 0;
  static bool ledState = false;

  if ((uint32_t)(currentMillis - previousMillis) >= FLASH_INTERVAL) {

    esp_task_wdt_reset();

    // Serial.print("loop() running on core ");
    // Serial.println(xPortGetCoreID());

    ledState = !ledState;
    digitalWrite(DEBUG_LED, ledState);

    setExpanderDebugLED(!ledState);

    if (getChannel1ErrorFlag()) {
      setChannel1StateLED(ledState);
    }
    if (getChannel2ErrorFlag()) {
      setChannel2StateLED(ledState);
    }

    previousMillis = currentMillis;
  }
} // blinking

void runSelfTest() {

  int8_t error = 0;

  // Make sure this doesn't come back and bite me in the ass (like -1 returns on
  // error and stuff)
  int8_t anyError = 0;

  serial_println();
  serial_println("Running self test...");
  serial_println();

  displaySelfTest();

  // Look for MCP23017
  Wire.beginTransmission(MCP_ADDR);
  error = Wire.endTransmission();

  anyError = error;

  if (error == 0) {
    serial_println("MCP23017 Present");
    displayExpanderTest(true);
  } else {
    serial_println("MCP23017 Not found");
    displayExpanderTest(false);
  }
  delay(1000);

  // Look for DAC
  Wire.beginTransmission(DAC_ADDR);
  error = Wire.endTransmission();
  anyError = anyError + error;

  if (error == 0) {
    serial_println("DAC Present");
    displayDACTest(true);
  } else {
    serial_println("DAC Not found");
    displayDACTest(false);
  }
  delay(1000);

  displaySelfTestResult(anyError);

  serial_println(anyError);
  serial_println();

  // TODO: Halt on self test error?
  while (anyError != 0) {
  }

  delay(3000);

  displayTitle();
}

void scanI2C() {

  while (1) {

    byte error, address;
    int nDevices;

    Serial.println("Scanning...");

    nDevices = 0;
    for (address = 1; address < 127; address++) {

      // Uses the return value of the Wiree.endTransmisstion to see if
      // a device acknowledge the address.
      Wire.beginTransmission(address);
      error = Wire.endTransmission();

      if (error == 0) {
        Serial.print("I2C device found at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.print(address, HEX);
        Serial.println("  !");

        nDevices++;
      } else if (error == 4) {
        Serial.print("Unknow error at address 0x");
        if (address < 16)
          Serial.print("0");
        Serial.println(address, HEX);
      }
    }
    if (nDevices == 0)
      Serial.println("No I2C devices found\n");
    else
      Serial.println("done\n");

    delay(5000); // wait 5 seconds for next scan
  }
}
