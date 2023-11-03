#ifndef _PINS_H_
#define _PINS_H_

/**********************************************
                    MCU PINS
**********************************************/
#define DEBUG_LED 2
#define BT_LED 15

#define I2C_SDA 25
#define I2C_SCL 26

#define IN_MCP_INT 27

#define LCD_DC 16
#define LCD_CS 5
#define LCD_RST 17

#define OUT_VPLUS 12
#define OUT_VMINUS 13
#define OUT_RANGE 14

// Rotary Encoders and keys pins
// ** WARNING PINS 34-39 INPUT ONLY, NO BUILT_IN PULLUPS/PULLDOWNS **

#define ENC1_A_PIN 33
#define ENC1_B_PIN 32
#define ENC1_BTN_PIN 35
#define ENC2_A_PIN 34
#define ENC2_B_PIN 39
#define ENC2_BTN_PIN 36

/**********************************************/
//              MCP23017 Pins
//     Port A = inputs, Port B = outputs
/**********************************************/

// Inputs (Port A)
#define SW_VPLUS_EN 7
#define SW_VMINUS_EN 5
#define SW_TRACKING 6

#define IN_CHAN1_ERR 4
#define IN_CHAN2_ERR 3

#define SW_RANGE 2
#define SW_MODE 1

// Outputs (Port B)
#define LED_VPLUS_EN 7
#define LED_VMINUS_EN 5
#define LED_TRACKING_EN 6
#define LED_RANGE 2
#define MCP_DEBUG_LED 3

#endif