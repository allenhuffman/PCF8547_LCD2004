#ifndef LCD2004_PCF8547_H
#define LCD2004_PCF8547_H

#include <stdint.h>

#define PCF8574_ADDRESS   0x27 // 39 (7-bit address)
//#define PCF8574_ADDRESS   (0x27*2) // 39 (8-bit address)

#if !defined(BIT)
#define BIT(b)      (1<<(b))
#endif

// PCF8574 8-bit pins map to the following LCD2004 pins:
//
// [7  6  5  4  3  2  1  0 ]
// |D7 D6 D5 D4 BL -E RW RS
//
// NOTE: This is hard-coded to assume the upper four bits are
// the 4 data lines to the LCD, in that order.

typedef enum
{
    DB7_BIT = BIT(7), // High nibble for 4-bit mode. 
    DB6_BIT = BIT(6), //
    DB5_BIT = BIT(5), //
    DB4_BIT = BIT(4), //
    BL_BIT  = BIT(3), // Backlight (0=Off, 1=On)
    E_BIT   = BIT(2), // Enable (0=Disable, 1=Enable)
    RW_BIT  = BIT(1), // Read/Write (0=Write, 1=Read)
    RS_BIT  = BIT(0)  // Register Select (0=Instruction, 1=Data)
} LCDBitEnum;

// #define DB7_BIT     BIT(7) // High nibble for 4-bit mode. 
// #define DB6_BIT     BIT(6) //
// #define DB5_BIT     BIT(5) //
// #define DB4_BIT     BIT(4) //
// #define BL_BIT      BIT(3) // Backlight (0=Off, 1=On)
// #define E_BIT       BIT(2) // Enable (0=Disable, 1=Enable)
// #define RW_BIT      BIT(1) // Read/Write (0=Write, 1=Read)
// #define RS_BIT      BIT(0) // Register Select (0=Instruction, 1=Data)

// Function Prototypes
bool IsLCDEnabled (void);

bool LCDInit (void);
void LCDTerm (void);

void LCDWriteInstructionNibble (uint8_t nibble);

void LCDWriteByte (uint8_t rsBit, uint8_t value);
void LCDWriteInstructionByte (uint8_t instruction);
void LCDWriteDataByte (uint8_t data);

void LCDWriteData (uint8_t *dataPtr, uint8_t dataSize);
void LCDWriteDataString (uint8_t x, uint8_t y, char *message);

void LCDSetOffset (uint8_t offset);
void LCDSetXY (uint8_t x, uint8_t y);

void LCDClear (void);

void LCDWaitForBusyFlag (void);

#endif   /* LCD2004_PCF8547_H */

// End of LCD2004_PCF8547.h
