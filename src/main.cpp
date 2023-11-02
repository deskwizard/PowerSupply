#include <Arduino.h>

// I sometimes get a v+ encoder signal at boot, investigate....
// the cap across doesn't have time to charge or something? would a delay help?

#include "LCD_defs.h"
#include "control.h"
#include "defines.h"
#include "encoders.h"
#include <ADS7828.h>

MCP23017 expander(MCP_ADDR);

#define ADC_READ_AVG 10
ADS7828 adc(ADC_ADDR);
int16_t chan1VRead;
int16_t chan1IRead;

// fuck this...
// uint16_t readings[ADC_READ_AVG] = {0}; // the readings from the analog input
// uint8_t readIndex = 0;                 // the index of the current reading
// uint32_t total = 0;   // the running total
// uint16_t average = 0; // the average

void testAnalog();

void setup() {

  //  TODO: retest with OTA and bluetooth to be sure it's still fine
  //  esp_task_wdt_init(WDT_TIMEOUT, true); //enable panic so ESP32 restarts
  //  esp_task_wdt_add(NULL); //add current thread to WDT watch

  Serial.begin(115200);
  Serial.setTimeout(100); // Timeout value for parseInt and stuff

  Wire.begin(I2C_SDA, I2C_SCL); // SDA, SCL
  Wire.setClock(400000);        // has to be after begin().

  // scanI2C();

  dac_init();
  adc.begin(INT);

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

  keys_init();

  serial_println("Sketch starting... ");
  serial_println();

  pinMode(OUT_VPLUS, OUTPUT);
  pinMode(OUT_VMINUS, OUTPUT);
  pinMode(OUT_RANGE, OUTPUT);

  // runSelfTest();

  bt_init();
  // startBT();

  // ota_init();

  encoder_init();
  expander_init();
  timer_init();

  expander.digitalWrite(PORT_B, LED_VPLUS_EN, HIGH);
  expander.digitalWrite(PORT_B, LED_VMINUS_EN, HIGH);
  expander.digitalWrite(PORT_B, LED_TRACKING_EN, HIGH);

  lcdInit(); // has a 2sec delay

  expander.digitalWrite(PORT_B, LED_VPLUS_EN, LOW);
  expander.digitalWrite(PORT_B, LED_VMINUS_EN, LOW);
  expander.digitalWrite(PORT_B, LED_TRACKING_EN, LOW);

  digitalWrite(DEBUG_LED, LOW);

  serial_println("Ready");
  serial_println();
}

void loop() {

  // handleOTA();
  handleKeys();
  handleExpanderInputs();
  handleEncoders();
  handleSerial();
  blinking();

  // debug
  // testAnalog();

  uint32_t currentMillis = millis();
  static uint32_t previousMillis = 0;

  if (((uint32_t)(currentMillis - previousMillis) >= 1) && chan1_enabled) {

    static uint8_t readIndex = 0;

    static uint16_t readingsV[ADC_READ_AVG] = {0};
    static uint32_t totalV = 0;

    static uint16_t readingsI[ADC_READ_AVG] = {0};
    static uint32_t totalI = 0;

    // subtract the last reading:
    totalV = totalV - readingsV[readIndex];
    totalI = totalI - readingsI[readIndex];
    // read from the sensor:
    //    readings[readIndex] = adc.read(0, SD);
    readingsV[readIndex] = adc.read(0, DF);
    readingsI[readIndex] = adc.read(2, DF);
    // add the reading to the total:
    totalV = totalV + readingsV[readIndex];
    totalI = totalI + readingsI[readIndex];
    // advance to the next position in the array:
    readIndex = readIndex + 1;

    // End of the array, wrap around
    if (readIndex >= ADC_READ_AVG) {
      readIndex = 0;
    }

    // calculate the average:
    chan1VRead = totalV / ADC_READ_AVG;
    chan1IRead = totalI / ADC_READ_AVG;

    previousMillis = currentMillis;
  }
}

void blinking() {

  uint32_t currentMillis = millis();
  static uint32_t previousMillis = 0;
  static bool ledState = false;

  if ((uint32_t)(currentMillis - previousMillis) >= FLASH_INTERVAL) {

    esp_task_wdt_reset();

    // Serial.print("loop() running on core ");
    // Serial.println(xPortGetCoreID());

    displayUpdateChan1V();
    displayUpdateChan1I();

    ledState = !ledState;
    digitalWrite(DEBUG_LED, ledState);

    expander.digitalWrite(PORT_B, 2, !ledState);

    if (errorFlagChan1) {
      expander.digitalWrite(PORT_B, LED_VPLUS_EN, ledState);
    }
    if (errorFlagChan2) {
      expander.digitalWrite(PORT_B, LED_VMINUS_EN, ledState);
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
      // The i2c_scanner uses the return value of
      // the Write.endTransmisstion to see if
      // a device did acknowledge to the address.
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

void readADC() {

  uint16_t read_value = 0;
  float voltage = 0;

  // for (int x = 0; x <= 7; x++) {
  for (int x = 0; x <= 1; x++) {

    read_value = adc.read(x, SD);
    voltage = read_value * (2500.0 / 4096.0);

    Serial.print("Channel ");
    Serial.print(x);
    Serial.print(": ");
    Serial.print(read_value);
    Serial.print(" (");
    Serial.print(voltage, 0);
    Serial.println(" mV)");
  }

  Serial.println();
}

void testAnalog() {

  uint32_t currentMillis = millis();  // Get snapshot of time
  static uint32_t previousMillis = 0; // Tracks the time since last event fired
  static bool ledState;

  if ((uint32_t)(currentMillis - previousMillis) >= 500) {
    writeDAC();
    readADC();
    previousMillis = currentMillis;
  }
}