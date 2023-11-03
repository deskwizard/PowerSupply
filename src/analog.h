#ifndef _ANALOG_H_
#define _ANALOG_H_

#include "defines.h"

// DAC channels 
// (Even #s since the outputs pins are on the same side of the chip)
#define DAC_CHAN1_V 0
#define DAC_CHAN1_I 2
#define DAC_CHAN2_V 4
#define DAC_CHAN2_I 6

#define ADC_READ_AVG 10  // Number of readings to take for averaging

void testAnalog();

/****************************************************************/
/*                             DAC                              */
/****************************************************************/
void initDAC();
void setChannelState(uint8_t channel, bool state);
void setChan1V();
void setChan1V(uint16_t code);
void setChan1I();
void setChan1I(uint16_t code);
void setChan2V();
void setChan2V(uint16_t code);
void setChan2I();
void setChan2I(uint16_t code);


/****************************************************************/
/*                             ADC                              */
/****************************************************************/

#define ADC_READ_AVG 10

void initADC();

//debug
void handleAnalog();

#endif