/*
 TI ADS7828 12bits i2C ADC Library
 Datasheet: http://www.ti.com/lit/ds/symlink/ads7828.pdf

 Charles Dorval (deskwizard)

 ------------------------------------------------------------------------------
 
 Current Library version: 1.0 - November 3rd, 2023
 Tested working on IDE version 1.8.19

  ------------------------------------------------------------------------------

 This is a basic library to use the ADS7828 ADC.
 
 Both single ended and differential modes are currently available.
 Please read the information below if you plan to use differential mode.
 
 Both Internal and external voltage reference mode are available.
 Please read the datasheet if you are to use the internal reference!

 ------------------------------------------------------------------------------ 

 Chip Wiring

 All connections necessary unless specifically mentionned.
 
 Connect GND pin to Ground
 Connect +VDD pin to 5v
 Connect SCL pin to Arduino SCL (Analog 5)
 Connect SDA pin to Arduino SDA (Analog 4)
 Connect A0 pin to either ground or 5v (set device address accordingly)
 Connect A1 pin to either ground or 5v (set device address accordingly) 
 Connect COM pin to ground (Single-ended mode common channel)
 
 CH0 - CH7 are obviously the ADC input channels (Optional?)
 
 ------------------------------------------------------------------------------    

 Function list
 
  begin(INT)*                 Initialize the ADC and the I2C bus with internal voltage reference. (* Use one or the other)
  begin()*                    Initialize the ADC and the I2C bus with external voltage reference. (* Use one or the other)
  read(channel, SD|DF)        Read the specified ADC channel (0-7) in either single ended (SD) or differential (DF) mode.
                              Reading a channel will return a value between 0-4095
                              
 ------------------------------------------------------------------------------    

 Using differential mode
 
 When using differential mode, it is important to pay attention to the channel naming.
 The channel configuration is contained in Table II on page 11 of the datasheet.
 
 It is important to respect the polarity of the channels when reading in differential mode
 or your readings will be 0.
 
 For example:
 
  Differential Channel 0:      Positive side on Channel 0, Negative side on Channel 1
  Differential Channel 1:      Positive side on Channel 1, Negative side on Channel 0
  Differential Channel 2:      Positive side on Channel 2, Negative side on Channel 3
  Differential Channel 3:      Positive side on Channel 3, Negative side on Channel 2
  etc...
  
*/

#include "ADS7828.h"
#include "Arduino.h"
#include "Wire.h"

bool vref_config = EXT; // Default voltage reference is external

// Command address for the channels, allows 0-7 channel 
// mapping in the correct order
uint8_t channels[8] = {0x00, 0x40, 0x10, 0x50, 0x20, 0x60, 0x30, 0x70};

ADS7828::ADS7828(uint8_t address) {
  ads_address = address; // Set ADC i2c address
}

void ADS7828::begin() {}

void ADS7828::begin(boolean value) {
  vref_config = value; // Set voltage reference (Internal or external)
}

uint16_t ADS7828::read(uint8_t channel, bool mode) {

  uint8_t command = 0;  // Initialize command variable
  uint16_t reading = 0; // Initialize reading variable

  command = channels[channel];

  if (mode == SD) {
    command = command ^ 0x80; // Set Single-ended mode
  }

  if (vref_config == INT) {
    command = command ^ 0x08; // Set voltage reference to internal
  }

  Wire.beginTransmission(ads_address); // Send a start or repeated start command
  Wire.write(command); // Then send a command byte for the register to be read.
  Wire.endTransmission(); // Send stop command

  Wire.requestFrom(ads_address, 2); // Request 2 bytes from the ADC

  if (2 <= Wire.available()) {// If two bytes were received
    reading = Wire.read();  // Read high byte
    reading = reading << 8; // Shift high byte to be high 8 bits
    reading |= Wire.read(); // Read low byte into lower 8 bits
  }

  return reading;
}
