#ifndef _DEFINES_H_
#define _DEFINES_H_

#include "BluetoothSerial.h"
#include "pins.h"
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <esp_task_wdt.h>

#define ERR_DISABLE
#define WAIT_FOR_BT
// #define LCD_TEST

#define WDT_TIMEOUT 10 // 10 seconds WDT

#define ADC_ADDR 0x4B
#define DAC_ADDR 0x48
#define MCP_ADDR 0x20

// Directions (encoders/control)
#define DOWN 0
#define UP 1

// LED blinking
#define FLASH_INTERVAL 1000

// Inputs timer interrupt frequency
#define TIMER_FREQ 1000 // In hertz

#define VREF_MV 2489.0f
// #define OUT_GAIN 5.0
#define OUT_GAIN 10.0
#define MAX_V_CODE 3000
#define MAX_I_CODE 4095 // 1000

// In case we need some in the future
#define CHAN1V_OFFSET 0
#define CHAN2V_OFFSET 0

// Output mode (unipolar/bipolar)
#define SINGLE 1
#define DUAL 0

#define MODE_CV 0
#define MODE_CC 1

// From timer.cpp
void initTimer();

// from bluetooth.cpp
void initBT();
void startBT();

// From OTA.cpp
void initOTA();
void handleOTA();

// From serial_helpers.cpp
void handleSerial();
void handleBT_Serial();

void serial_print(const char *data);
void serial_println(const char *data);
void serial_print(const int16_t data);
void serial_println(const int16_t data);
void serial_print(const float data, uint8_t digits);
void serial_println(const float data, uint8_t digits);
void serial_println();
void printChan1V();
void printChan1I();
void printChan2V();
void printChan2I();
void printDebugPrefixLCD(const char *data);

// **** TESTING ( in main.cpp) ****
void runSelfTest();
void blinking();
void scanI2C();

#endif
