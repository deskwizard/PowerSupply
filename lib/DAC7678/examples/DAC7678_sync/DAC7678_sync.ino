/*
Example sketch for the TI DAC7678 12bits I²C DAC Library
Datasheet: https://www.ti.com/lit/ds/symlink/dac7678.pdf

Demonstrate selective batch updating of DAC channels

Charles Dorval (deskwizard)

------------------------------------------------------------------------------
 
Current Library version: 1.0 - November 3rd, 2023
Tested working on IDE version 1.8.19

------------------------------------------------------------------------------

Chip Wiring

Connect GND pin to Ground
Connect AVDD pin to 5v
Connect ADDR0 to ground
Connect LDAC pin to 5v
Connect SDA pin to Arduino SDA (Analog 4 on Uno)
Connect SCL pin to Arduino SCL (Analog 5 on Uno)
Connect CLR pin to 5v

*/

#include <DAC7678.h>                  // Include DAC7678.h for TI DAC7678 functions

DAC7678 dac(0x48);                    // Set the DAC address to 0x48 (72 in decimal)

void setup() {

  Serial.begin(9600);
  Serial.println("Sketch started");
  Serial.println();

  Wire.begin();

  dac.begin();              // Initialize the DAC
  dac.setVREF(INT);         // Configure for internal voltage reference

  // Or alternatively, you can do it like so:
  // dac.begin(INT); // Initialize the DAC with internal reference

  dac.LDAC(0, ENABLE);      // Disable LDAC for pin 2

  delay(1000);

  dac.enable();
  
  dac.set(2, 4095);     
  delay(1000);
  
  dac.update(0, 4095);      // Won't write since LDAC is enabled on that pin
  dac.update(1, 4095);      // Will write since LDAC is disabled on that pin
  delay(1000);
  
  dac.set(2, 0);            // Writes channel 0 and channel 2

}


void loop() {}
