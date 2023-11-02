
/*
TI DAC7678 12bits I²C DAC Library
Datasheet: https://www.ti.com/lit/ds/symlink/dac7678.pdf

Charles Dorval (deskwizard)

------------------------------------------------------------------------------
 
Current Library version: 1.0 - November 3rd, 2023
Tested working on IDE version 1.8.19

------------------------------------------------------------------------------
*/

#include "DAC7678.h"
#include "Arduino.h"
#include "Wire.h"
#include "defines.h"

DAC7678::DAC7678(uint8_t address) {

  dac7678_address = address;

  // Preload HIGH-Z off mode - that way you can change it before begin()
  for (uint8_t x = 0; x <= 7; x++) {
    off_mode[x] = HIGHZ;
  }
}

void DAC7678::begin() {
  reset();   // Sends Power-On Reset command to DAC
  disable(); // Make sure outputs are powered off
}

void DAC7678::begin(bool vref_config) {
  setVREF(vref_config); // Set voltage reference mode
  begin();
}

// Sets voltage reference mode
// vref_config 0: Internal vref turned off
// vref_config 1: Internal vref turned on
void DAC7678::setVREF(bool vref_config) {

  uint8_t lsdb;

  if (vref_config) {
    lsdb = 0x10;
  } else if (!vref_config) {
    lsdb = 0x00;
  }

  transmit(CMD_INTREF_RS, 0x00, lsdb);
}

// Issues Reset command (Same as Power-On reset)
void DAC7678::reset() { transmit(CMD_SOFT_RESET, 0x00, 0x00); }

// Configure off mode for specified channel
void DAC7678::offMode(uint8_t channel, uint8_t mode) {
  off_mode[channel] = mode;
}

// Configure off mode for all channels
void DAC7678::offMode(uint8_t mode) {

  for (uint8_t x = 0; x <= 7; x++) {
    off_mode[x] = mode;
  }
}
// Configure LDAC mode for all channels
void DAC7678::LDAC(bool state) {

  if (state) {
    LDAC_reg = 0x00;
  } else {
    LDAC_reg = 0xFF;
  }

  transmit(CMD_LDAC, LDAC_reg, 0x00);
}

// Configure LDAC mode for specified channel
void DAC7678::LDAC(uint8_t channel, bool state) {

  if (state) {
    LDAC_reg &= ~(1 << channel);
  } else {
    LDAC_reg |= (1 << channel);
  }

  transmit(CMD_LDAC, LDAC_reg, 0x00);
}

// Select channel for update
void DAC7678::select(uint8_t channel) {
  transmit(CMD_SELECT + channel, 0x00, 0x00);
}

// Enables all the channels at once
void DAC7678::enable() { transmit(CMD_POWER_DOWN, 0x1F, 0xE0); }

// Disables all the channels in one go
// Doing it one by one is required in case of different off modes per channel
void DAC7678::disable() {

  for (uint8_t x = 0; x <= 7; x++) {
    disableChannel(x);
  }
}

// Set specific channel state
void DAC7678::setChannelState(uint8_t channel, bool state) {

  if (state) {
    enableChannel(channel);
  } else {
    disableChannel(channel);
  }
}

// Enables the specific DAC output channel
void DAC7678::enableChannel(uint8_t channel) {

  if (channel < 8) {

    uint16_t x = (uint16_t)(0x20 << channel); // Channels are offset +5 bits
    uint8_t msdb = (uint8_t)(x >> 8);
    uint8_t lsdb = x & 0xFF;

    transmit(CMD_POWER_DOWN, msdb, lsdb);
  }
}

// Disables the specific DAC output channel
void DAC7678::disableChannel(uint8_t channel) {

  if (channel < 8) {

    uint16_t x = (uint16_t)(0x20 << channel); // Channels are offset +5 bits
    uint8_t msdb = (uint8_t)(x >> 8);
    msdb |= off_mode[channel];
    uint8_t lsdb = x & 0xFF;

    transmit(CMD_POWER_DOWN, msdb, lsdb);
  }
}

// Sets all DAC channels to specified value
void DAC7678::set(int16_t value) {

  for (uint8_t x = 0; x <= 7; x++) {
    set(x, value);
  }
}

// Set specified channel (0-7) to the specified value
void DAC7678::set(uint8_t channel, int16_t value) {

  //   Check for out of range values
  if (value >= 4096 || value < 0) {
    return;
  }

  if (channel < 8) { // Check for out of range Channel #

    // Sets the variables
    uint8_t command = CMD_IS_LDAC_BASE_ADDR + channel;

    uint16_t x = (uint16_t)(value << 4); // Data is offset +4 bits
    uint8_t msdb = (uint8_t)(x >> 8);
    uint8_t lsdb = x & 0xFF;

    // Send data to DAC
    transmit(command, msdb, lsdb);
  }
}

// Update specified channel (0-7) to the specified value
void DAC7678::update(uint8_t channel, int16_t value) {

  //   Check for out of range values
  if (value >= 4096 || value < 0) {
    return;
  }

  if (channel < 8) { // Check for out of range Channel #
    // Sets the variables
    uint8_t command = CMD_WRITE_BASE_ADDR + channel;

    uint16_t x = (uint16_t)(value << 4); // Data is offset +4 bits
    uint8_t msdb = (uint8_t)(x >> 8);
    uint8_t lsdb = x & 0xFF;

    // Send data to DAC
    transmit(command, msdb, lsdb);
  }
}

// Configures the DAC value to output on all channels when CLR pin is brought
// low Will set the CLR Code register to output either zero, half-range, full
// range or to ignore the CLR pin
void DAC7678::clrMode(int16_t value) {

  uint8_t lsdb = value << 4;

  // Send data to DAC
  transmit(CMD_CLEAR_CODE, 0x00, lsdb);
}

uint16_t DAC7678::readChan(uint8_t command) {

  uint16_t reading = 0;

  Wire.beginTransmission(dac7678_address);
  Wire.write(command); // Then send a command byte for the register to be read.
  Wire.endTransmission();
  Wire.requestFrom(dac7678_address, 2);

  if (Wire.available() <= 2) { // If two bytes were received
    reading = Wire.read();     // Read high byte
    reading = reading << 8;    // Shift high byte to be high 8 bits
    reading |= Wire.read();    // Read low byte
    reading = reading >> 4;    // Shift for to get rid of the 4x 0s
  }

  return reading;
}

uint16_t DAC7678::readDAC(uint8_t command) {

  uint16_t reading = 0;

  Wire.beginTransmission(dac7678_address);
  Wire.write(command); // Then send a command byte for the register to be read.
  Wire.endTransmission();
  Wire.requestFrom(dac7678_address, 2);

  if (Wire.available() <= 2) { // if two bytes were received
    reading = Wire.read();     // receive high byte
    reading = reading << 8;    // shift high byte to be high 8 bits
    reading |= Wire.read();
    reading = reading >> 4;
  }

  return reading;
}

// Transmit the actual command and high and low bytes to the DAC
void DAC7678::transmit(uint8_t _command, uint8_t _msdb, uint8_t _lsdb) {

  Wire.beginTransmission(dac7678_address);
  Wire.write(_command);
  Wire.write(_msdb);
  Wire.write(_lsdb);
  Wire.endTransmission();
}

// Deprecated, kept for backwards compatibility
void DAC7678::init() { begin(); }

void DAC7678::enable(uint8_t state) {

  if (state) {
    enable();
  } else {
    disable();
  }
}

void DAC7678::enable(uint8_t channel, uint8_t state) {

  if (state) {
    enableChannel(channel);
  } else {
    disableChannel(channel);
  }
}
