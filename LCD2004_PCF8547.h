#ifndef LCD2004_PCF8547_H
#define LCD2004_PCF8547_H

/*--------------------------------------------------------------------------*/
// Include Files
/*--------------------------------------------------------------------------*/
#include <stdint.h>


extern const uint8_t defaultCGRAMData[64];

/*--------------------------------------------------------------------------*/
// Constants
/*--------------------------------------------------------------------------*/
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

// INSTRUCTIONS/COMMANDS
typedef enum
{
    LCD_CLEAR_DISPLAY   = BIT(0),   // 00000001
    LCD_RETURN_HOME     = BIT(1),   // 0000001-
    LCD_ENTRY_MODE_SET  = BIT(2),   // 000001xx
    LCD_DISPLAY_CONTROL = BIT(3),   // 00001xxx
    LCD_SHIFT           = BIT(4),   // 0001xx--
    LCD_FUNCTION_SET    = BIT(5),   // 001xxx--
    LCD_SET_CGRAM_ADDR  = BIT(6),   // 01xxxxxx
    LCD_SET_DDRAM_ADDR  = BIT(7),   // 1xxxxxxx
} InstructionCodeEnum;

// To ensure bad values are not passed in:
#define LCD_ENTRY_MODE_SET_MASK     0b00000011
#define LCD_DISPLAY_CONTROL_MASK    0b00000111
#define LCD_SHIFT_MASK              0b00001100
#define LCD_FUNCTION_SET_MASK       0b00011100
#define LCD_CGRAM_ADDR_MASK         0b00111111
#define LCD_DDRAM_ADDR_MASK         0b01111111


// ENTRY MODE SET BITS
typedef enum
{
    S_BIT               = BIT(0),   // -------1
    ID_BIT              = BIT(1),   // ------1-

} EntryModeSetBitsEnum;

// DISPLAY ON/OFF CONTROL BITS
typedef enum
{
    B_BIT               = BIT(0),   // -------1
    C_BIT               = BIT(1),   // ------1-
    D_BIT               = BIT(2)    // -----1--
} DisplayOnOfControlBitsEnum;

// CURSOR OR DISPLAY SHIFT BITS
typedef enum
{
    RL_BIT              = BIT(2),   // -----1--
    SC_BIT              = BIT(3)    // ----1---
} CursorOrDisplayShiftBitsEnum;

// FUNCTION SET BITS
typedef enum
{
    F_BIT               = BIT(2),   // -----1--
    N_BIT               = BIT(3),   // ----1---
    DL_BIT              = BIT(4)    // ---1----
} FunctionSetBitsEnum;


/*--------------------------------------------------------------------------*/
// Prototypes
/*--------------------------------------------------------------------------*/
bool IsLCDEnabled (void);

bool LCDInit (void);
void LCDTerm (void);

void LCDWriteInstructionNibble (uint8_t nibble);

void LCDWriteByte (uint8_t rsBit, uint8_t value);
void LCDWriteInstructionByte (uint8_t instruction);
void LCDWriteDataByte (uint8_t data);

void LCDWriteData (uint8_t *dataPtr, uint8_t dataSize);
void LCDWriteDataString (char *message);
void LCDWriteDataXYString (uint8_t x, uint8_t y, char *message);

void LCDWriteDataCGRAM (uint8_t *dataPtr, uint8_t dataSize);
void LCDWriteDataCGRAMCharacter (uint8_t characterNumber, uint8_t *dataPtr, uint8_t dataSize);

void LCDSetXY (uint8_t x, uint8_t y);

// LCD COMMANDS/INSTRUCTIONS
void LCDClear (void);
void LCDHome (void);
void LCDEntryModeSet (uint8_t entryModeBits);
void LCDDisplayOnOffControl (uint8_t displayControlBits);
void LCDCursorOrDisplayShift (uint8_t cursorOrDisplayShiftBits);
void LCDFunctionSet (uint8_t functionSetBits);
void LCDSetCGRAMAddress (uint8_t cgramAddress);
void LCDSetDDRAMAddress (uint8_t ddramAddress);

// READ
uint8_t LCDReadByte(LCDBitEnum rsBit);
uint8_t LCDReadDataByte (void);
void LCDReadData (uint8_t *bufferPtr, uint8_t bufferSize);

void LCDReadDataDDRAM (uint8_t DDRAMAddress, uint8_t *bufferPtr, uint8_t bufferSize);
void LCDReadDataCGRAM (uint8_t *bufferPtr, uint8_t bufferSize);
void LCDReadDataCGRAMCharacter (uint8_t characterNumber, uint8_t *bufferPtr, uint8_t bufferSize);

uint8_t LCDReadBusyFlagAndAddressCounter (void);
uint8_t LCDReadAddressCounter (void);

void LCDWaitForBusyFlag (void);

// Convienience functions:

// Display
void LCDSetDisplay(bool displayOn);
void LCDDisplayOn(void);
void LCDDisplayOff(void);

// Cursor (underline)
void LCDSetCursor(bool cursorOn);
void LCDCursorOn(void);
void LCDCursorOff(void);

// Cursor (blinking block)
void LCDSetCursorBlink(bool blinkingOn);
void LCDCursorBlinkOn(void);
void LCDCursorBlinkOff(void);

// PCF8574-specific:

// Backlight
void LCDSetBacklight(bool backlightOn);
void LCDSetBacklightOn(void);
void LCDSetBacklightOff(void);

#endif   /* LCD2004_PCF8547_H */

// End of LCD2004_PCF8547.h
