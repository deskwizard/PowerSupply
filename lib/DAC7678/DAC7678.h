
/*
TI DAC7678 12bits I²C DAC Library
Datasheet: https://www.ti.com/lit/ds/symlink/dac7678.pdf

Charles Dorval (deskwizard)

------------------------------------------------------------------------------
 
Current Library version: 1.0 - November 3rd, 2023
Tested working on IDE version 1.8.19

------------------------------------------------------------------------------
*/

#ifndef DAC7678_H
#define DAC7678_H

#include "Arduino.h"
#include "defines.h"
#include <Wire.h>

class DAC7678 {

public:
  DAC7678(uint8_t address);
  void begin();
  void begin(bool vref_config);
  void reset();
  void setVREF(bool refstate);
  void LDAC(bool state);
  void LDAC(uint8_t channel, bool state);
  void offMode(uint8_t channel, uint8_t mode);
  void offMode(uint8_t mode);
  void enable();
  void disable();
  void setChannelState(uint8_t channel, bool state);
  void enableChannel(uint8_t channel);
  void disableChannel(uint8_t channel);
  void set(int16_t value);
  void set(uint8_t channel, int16_t _value);
  void select(uint8_t channel);
  void update(uint8_t channel, int16_t value);
  void clrMode(int16_t value);
  uint8_t DAC;
  uint16_t readChan(uint8_t command);
  uint16_t readDAC(uint8_t command);

  // Deprecated, kept for backwards compatibility
  void init();
  void enable(uint8_t state);
  void enable(uint8_t channel, uint8_t state);

private:
  int16_t dac7678_address;
  uint8_t off_mode[8];
  uint8_t LDAC_reg = 0xFF;
  void transmit(uint8_t command, uint8_t msdb, uint8_t lsdb);
};

#endif
