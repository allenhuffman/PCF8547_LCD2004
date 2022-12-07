/*---------------------------------------------------------------------------*/
/*
  LCD2004/LCD1602 via PCF8574 I2C Interface
  By Allen C. Huffman
  www.subethasoftware.com

  Code to do most all of the functions of the LCD2004 via the PCF8574 I2C
  interface board. (It can easily be modified to work with LCD1602 and any
  other size display that uses the same interface.)

  REFERENCES:
  
  CONFIGURATION:
  
  VERSION HISTORY:
  2022-08-31 0.0 allenh - In the beginning...
  2022-09-06 0.1 allenh - Updated Sub-Etha font ;-)
  2022-12-07 0.2 allenh - Initial support for 4-bit GPIO mode.
  
  TODO:
  
  TOFIX:

*/
/*---------------------------------------------------------------------------*/
#ifndef LCD2004_PCF8547_C
#define LCD2004_PCF8547_C


/*--------------------------------------------------------------------------*/
// Include Files
/*--------------------------------------------------------------------------*/
#include <stdint.h>

#include <Wire.h>

#include "LCD2004_PCF8547.h"


/*--------------------------------------------------------------------------*/
// STATIC GLOBALS
/*--------------------------------------------------------------------------*/
static bool     S_IsLCDEnabled = false;
static uint8_t  S_DisplayControlBits = 0;
static uint8_t  S_BacklightBit = 0;


/*--------------------------------------------------------------------------*/
// CONSTANTS
/*--------------------------------------------------------------------------*/
const uint8_t defaultCGRAMData[] =
{
    // S
  0b01111,
  0b11111,
  0b11000,
  0b11110,
  0b01111,
  0b00011,
  0b11111,
  0b11110,

  // U
  0b11011,
  0b11011,
  0b11011,
  0b11011,
  0b11011,
  0b11011,
  0b11111,
  0b01110,

  // B
  0b11110,
  0b11111,
  0b11011,
  0b11110,
  0b11111,
  0b11011,
  0b11111,
  0b11110,

  // -
  0b00000,
  0b00000,
  0b00000,
  0b01110,
  0b01110,
  0b00000,
  0b00000,
  0b00000,

  // E
  0b11111,
  0b11111,
  0b11000,
  0b11111,
  0b11111,
  0b11000,
  0b11111,
  0b11111,

  // T
  0b11111,
  0b11111,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b01110,

  // H
  0b11011,
  0b11011,
  0b11011,
  0b11111,
  0b11111,
  0b11011,
  0b11011,
  0b11011,

  // A
  0b01110,
  0b11111,
  0b11011,
  0b11111,
  0b11111,
  0b11011,
  0b11011,
  0b11011,
};


/*--------------------------------------------------------------------------*/
// FUNCTIONS
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
// Return status of LCD (true=enabled, false=not enabled)
/*--------------------------------------------------------------------------*/
bool IsLCDEnabled(void)
{
    return S_IsLCDEnabled;
}


/*--------------------------------------------------------------------------*/
// Initialize the LCD2004.
/*--------------------------------------------------------------------------*/
//    4-Bit data mode
//    2 Line display
//    Display On
//    Display Clear
//    Left-to-Right display mode.
bool LCDInit(void)
{
    // Set all PCF8547 I/O pins LOW.
#if defined(PCF8574_ADDRESS)
    int ack = 0;

    Wire.begin();

    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(0x0);
    ack = Wire.endTransmission();

    if (ack != 0)
    {
        return false;
    }
#else // GPIO
    pinMode (PIN_RS, OUTPUT);
    pinMode (PIN_E, OUTPUT);
    pinMode (PIN_D4, OUTPUT);
    pinMode (PIN_D5, OUTPUT);
    pinMode (PIN_D6, OUTPUT);
    pinMode (PIN_D7, OUTPUT);

    digitalWrite (PIN_RS, LOW);
    digitalWrite (PIN_E, LOW);
    digitalWrite (PIN_D4, LOW);
    digitalWrite (PIN_D5, LOW);
    digitalWrite (PIN_D6, LOW);
    digitalWrite (PIN_D7, LOW);
#endif

    delayMicroseconds(50000);

    // We have an LCD.
    S_IsLCDEnabled = true;

    // [7  6  5  4  3  2  1  0 ]
    // [D7 D6 D5 D4 BL -E RW RS]
    LCDWriteInstructionNibble(0b0011);
    delay(5); // min 4.1 ms

    LCDWriteInstructionNibble(0b0011);
    delayMicroseconds(110); // min 100 us

    LCDWriteInstructionNibble(0b0011);
    delayMicroseconds(110); // min 100 us

    // Set interface to 4-bit mode.
    LCDWriteInstructionNibble(0b0010);

    // Now we can use the 8-bit as two 4-bit write functions:

    // Function Set
    // [ 0  0  1  DL N  F  0  0 ]
    // DL: 1=8-Bit, 0=4-Bit
    //  N: 1=2 Line, 0=1 Line
    //  F: 1=5x10, 0=5x8
    //                     (--001DNF00)
    //LCDWriteInstructionByte(0b00101000);
    LCDFunctionSet (N_BIT); // 2-Lines

    // Entry Mode Set
    // [ 0  0  0  0  0  1  ID S ]
    // ID: 1=Increment, 0=Decrement
    //  S: 1=Shift based on ID (1=Left, 0=Right)
    //                     (--000001IS)
    //LCDWriteInstructionByte(0b00000110);
    LCDEntryModeSet (ID_BIT); // Increment

    // Default settings:

    LCDClear ();
    LCDSetDisplay (true);
    LCDSetBacklight (true);

    // Load 8 custom characters and display Splash Screen.
    LCDSetCGRAMAddress (0);
    LCDWriteDataCGRAM (defaultCGRAMData, sizeof(defaultCGRAMData));
    LCDSetXY (6, 1);
    LCDWriteData ("\0\1\2\3\4\5\6\7", 8);
    LCDWriteDataStringXY (6, 2, "Software");
    delay (2000);

    return true;
}


/*--------------------------------------------------------------------------*/
// Disable LCD screen.
/*--------------------------------------------------------------------------*/
void LCDTerm(void)
{
    if (S_IsLCDEnabled == true)
    {
        LCDClear ();
        LCDSetBacklight (false);

        // Disable all I/O pins.
#if defined(PCF8574_ADDRESS)
        Wire.beginTransmission (PCF8574_ADDRESS);
        Wire.write (0);
        Wire.endTransmission ();
#else // GPIO
        digitalWrite (PIN_RS, LOW);
        digitalWrite (PIN_E, LOW);
        digitalWrite (PIN_D4, LOW);
        digitalWrite (PIN_D5, LOW);
        digitalWrite (PIN_D6, LOW);
        digitalWrite (PIN_D7, LOW);
#endif

        S_IsLCDEnabled = false;
    }
}


/*--------------------------------------------------------------------------*/
// Write out a 4-bit value.
/*--------------------------------------------------------------------------*/
// [7  6  5  4  3  2  1  0 ]
// [D7 D6 D5 D4 BL -E RW RS]
void LCDWriteInstructionNibble(uint8_t nibble)
{
#if defined(PCF8574_ADDRESS)
    uint8_t dataByte = S_BacklightBit | (nibble << 4);

    Wire.beginTransmission(PCF8574_ADDRESS);

    Wire.write(E_BIT | dataByte);
#else // GPIO
    digitalWrite (PIN_D7, nibble & BIT(4));
    digitalWrite (PIN_D6, nibble & BIT(3));
    digitalWrite (PIN_D5, nibble & BIT(2));
    digitalWrite (PIN_D4, nibble & BIT(1));
    digitalWrite (PIN_E, HIGH);
#endif
    delayMicroseconds(1);

#if defined(PCF8574_ADDRESS)
    Wire.write(dataByte);
#else // GPIO
    digitalWrite (PIN_E, LOW);
#endif
    delayMicroseconds(1);

#if defined(PCF8574_ADDRESS)
    Wire.endTransmission();
#endif

    delayMicroseconds(37);
}


/*--------------------------------------------------------------------------*/
// Write a byte out, 4-bits at a time. The rsBit will determine
// if it is an Instruction (rsBit=0) or Data byte (rsBit=1).
/*--------------------------------------------------------------------------*/
// [7  6  5  4  3  2  1  0 ]
// [D7 D6 D5 D4 BL -E RW RS]
void LCDWriteByte(uint8_t rsBit, uint8_t dataByte)
{
#if defined(PCF8574_ADDRESS)
    uint8_t newByte;

    Wire.beginTransmission(PCF8574_ADDRESS);

    newByte = S_BacklightBit | rsBit | (dataByte & 0xf0);
    Wire.write(E_BIT | newByte);
#else // GPIO
    digitalWrite (PIN_RS, rsBit & RS_BIT);
    digitalWrite (PIN_D7, dataByte & BIT(7));
    digitalWrite (PIN_D6, dataByte & BIT(6));
    digitalWrite (PIN_D5, dataByte & BIT(5));
    digitalWrite (PIN_D4, dataByte & BIT(4));
    digitalWrite (PIN_E, HIGH);
#endif    
    delayMicroseconds(1);

#if defined(PCF8574_ADDRESS)
    Wire.write(newByte);
#else // GPIO
    digitalWrite (PIN_E, LOW);
#endif

    delayMicroseconds(37);

#if defined(PCF8574_ADDRESS)
    newByte = S_BacklightBit | rsBit | (dataByte << 4);
    Wire.write(E_BIT | newByte);
#else // GPIO
    digitalWrite (PIN_D7, dataByte & BIT(3));
    digitalWrite (PIN_D6, dataByte & BIT(2));
    digitalWrite (PIN_D5, dataByte & BIT(1));
    digitalWrite (PIN_D4, dataByte & BIT(0));
    digitalWrite (PIN_E, HIGH);
#endif
    delayMicroseconds(1);

#if defined(PCF8574_ADDRESS)
    Wire.write(newByte);
#else // GPIO
    digitalWrite (PIN_E, LOW);
#endif    
    delayMicroseconds(37);

    Wire.endTransmission();
}


/*--------------------------------------------------------------------------*/
// Write with RS bit 0.
/*--------------------------------------------------------------------------*/
void LCDWriteInstructionByte(uint8_t instruction)
{
    LCDWriteByte(0, instruction);
}


/*--------------------------------------------------------------------------*/
// Write with RS bit 1.
/*--------------------------------------------------------------------------*/
void LCDWriteDataByte(uint8_t data)
{
    LCDWriteByte(RS_BIT, data);
    delayMicroseconds(53);
}


/*--------------------------------------------------------------------------*/
// Write out one or more data bytes.
/*--------------------------------------------------------------------------*/
void LCDWriteData(uint8_t *dataPtr, uint8_t dataSize)
{
    for (int idx = 0; idx < dataSize; idx++)
    {
        LCDWriteDataByte(dataPtr[idx]);
    }
}


/*--------------------------------------------------------------------------*/
// Write out a NIL terminated C string.
/*--------------------------------------------------------------------------*/
void LCDWriteDataString(char *message)
{
    LCDWriteData((uint8_t *)message, strlen(message));
}

/*--------------------------------------------------------------------------*/
// Write out a NIL terminated C string.
/*--------------------------------------------------------------------------*/
void LCDWriteDataStringXY(uint8_t x, uint8_t y, char *message)
{
    LCDSetXY(x, y);
    LCDWriteDataString(message);
}


/*--------------------------------------------------------------------------*/
// Character Generator RAM
/*--------------------------------------------------------------------------*/
void LCDWriteDataCGRAM (uint8_t *dataPtr, uint8_t dataSize)
{
    if ((dataPtr != NULL) && (dataSize != 0))
    {
        if (dataSize > 64)
        {
            dataSize = 64;
        }

        LCDWriteData (dataPtr, dataSize);
    }
}


/*--------------------------------------------------------------------------*/
// Character Generator RAM
/*--------------------------------------------------------------------------*/
void LCDWriteDataCGRAMCharacter (uint8_t characterNumber, uint8_t *dataPtr, uint8_t dataSize)
{
    if (characterNumber > 7)
    {
        characterNumber = 7;
    }

    LCDSetCGRAMAddress (characterNumber * 8);
    LCDWriteDataCGRAM (dataPtr, dataSize);
}


/*--------------------------------------------------------------------------*/
// Set LCD cursor position.
/*--------------------------------------------------------------------------*/
// LCD2004 is internally treated like a two line display of 64 characters
// each. The first internal line is bytes 0-63 and the second internal
// line is bytes 64-127.
//
// For the physical LCD2004 20x4 four-line display, the first 20 bytes of
// internal line 1 (0-19) is the display line 1. The second 20 bytes of
// internal line 1 (20-39) is the display line 3. The first 20 bytes
// of internal line 2 (64-83) is display line 2. The second 20 bytes
// of internal line 2 (84-103) is display line 4.
//
// Super easy and not confusing at all.
//
//                          +--------------------+
// Internal Line 1 (0-19)   |aaaaaaaaaaaaaaaaaaaa| Display line 1
// Internal Line 2 (64-83)  |bbbbbbbbbbbbbbbbbbbb| Display line 2
// Internal Line 1 (20-39)  |aaaaaaaaaaaaaaaaaaaa| Display line 3
// Internal Line 2 (84-103) |bbbbbbbbbbbbbbbbbbbb| Display line 4
//                          +--------------------+
//
// Because of this, we will use a simple translation to get between
// column (x) and row (y) to the actual offset of these two internal
// 64-byte lines.
//
void LCDSetXY(uint8_t x, uint8_t y)
{
    uint8_t offset = 0;

    if (y == 1)
    {
        offset = 64; // 0x40
    }
    else if (y == 2)
    {
        offset = 20; // 0x14
    }
    else if (y == 3)
    {
        offset = 84; // 0x54
    }
    else
    {
        // Offset will be 0.
    }

    offset = offset + x;

    LCDSetDDRAMAddress(offset);
}


// WRITE INSTRUCTIONS/COMMANDS

/*--------------------------------------------------------------------------*/
// Clear the LCD (and home the cursor position).
/*--------------------------------------------------------------------------*/
void LCDClear(void)
{
    // Avoid sending I2C data if no LCD is enabled.
    //if (S_IsLCDEnabled == true)
    {
        // Clear Display
        // [ 0  0  0  0  0  0  0  1 ]
        LCDWriteInstructionByte(LCD_CLEAR_DISPLAY);        
        delay(3); // 1.18ms - 2.16ms
        //LCDWaitForBusyFlag ();
    }
}


/*--------------------------------------------------------------------------*/
// Home the cursor.
/*--------------------------------------------------------------------------*/
void LCDHome(void)
{
    // Avoid sending I2C data if no LCD is enabled.
    //if (S_IsLCDEnabled == true)
    {
        // Return Home
        // [ 0  0  0  0  0  0  1  0 ]
        LCDWriteInstructionByte(LCD_RETURN_HOME);        
        delay(3); // 1.18ms - 2.16ms
        //LCDWaitForBusyFlag ();
    }
}


/*--------------------------------------------------------------------------*/
//
/*--------------------------------------------------------------------------*/
void LCDEntryModeSet (uint8_t entryModeBits)
{
    entryModeBits = (entryModeBits & LCD_ENTRY_MODE_SET_MASK);

    // Entry Mode Set
    // [ 0  0  0  0  0  1  ID S ]
    // ID: 1=Increment, 0=Decrement
    //  S: 1=Shift based on ID (1=Left, 0=Right)
    LCDWriteInstructionByte (LCD_ENTRY_MODE_SET | entryModeBits);

    delayMicroseconds (53);
}


/*--------------------------------------------------------------------------*/
//
/*--------------------------------------------------------------------------*/
void LCDDisplayOnOffControl (uint8_t displayControlBits)
{
    displayControlBits = (displayControlBits & LCD_DISPLAY_CONTROL_MASK);

    // Display On/Off Control
    // [ 0  0  0  0  1  D  C  B ]
    // D: Display
    // C: Cursor
    // B: Blink
    LCDWriteInstructionByte (LCD_DISPLAY_CONTROL | displayControlBits);

    delayMicroseconds (53);

    // Remember last bits that were set.
    S_DisplayControlBits = displayControlBits;
}


/*--------------------------------------------------------------------------*/
//
/*--------------------------------------------------------------------------*/
void LCDCursorOrDisplayShift (uint8_t cursorOrDisplayShiftBits)
{
    cursorOrDisplayShiftBits = (cursorOrDisplayShiftBits & LCD_SHIFT_MASK);

    // Cursor or Display Shift
    // [ 0  0  0  0  SC RL -  - ]
    // SC:
    // RL:
    LCDWriteInstructionByte (LCD_SHIFT | cursorOrDisplayShiftBits);

    delayMicroseconds (53);
}


/*--------------------------------------------------------------------------*/
//
/*--------------------------------------------------------------------------*/
void LCDFunctionSet (uint8_t functionSetBits)
{
    functionSetBits = (functionSetBits & LCD_FUNCTION_SET_MASK);

    // Function Set
    // [ 0  0  1  DL N  F  0  0 ]
    // DL: 1=8-Bit, 0=4-Bit
    //  N: 1=2 Line, 0=1 Line
    //  F: 1=5x10, 0=5x8
    LCDWriteInstructionByte (LCD_FUNCTION_SET | functionSetBits);

    delayMicroseconds (53);
}


/*--------------------------------------------------------------------------*/
//
/*--------------------------------------------------------------------------*/
void LCDSetCGRAMAddress (uint8_t CGRAMAddress)
{
    CGRAMAddress = (CGRAMAddress & LCD_CGRAM_ADDR_MASK);

    // Set CGRAM Address (0-63, first 8 5x8 characters)
    // [ 0  1  AC5 AC4 AC3 AC2 AC1 AC0 ]
    // ACx: CGRAM Address Counter
    LCDWriteInstructionByte (LCD_SET_CGRAM_ADDR | CGRAMAddress);

    delayMicroseconds(53);
}


/*--------------------------------------------------------------------------*/
//
/*--------------------------------------------------------------------------*/
void LCDSetDDRAMAddress (uint8_t DDRAMAddress)
{
    DDRAMAddress = (DDRAMAddress & LCD_DDRAM_ADDR_MASK);

    // Set CGRAM Address (0-127, screen memory)
    // [ 1 AC6 AC5 AC4 AC3 AC2 AC1 AC0 ]
    // ACx: DDRAM Address Counter
    LCDWriteInstructionByte (LCD_SET_DDRAM_ADDR | DDRAMAddress); 
   
    delayMicroseconds(53);
}

// READ

/*--------------------------------------------------------------------------*/
// Read a byte, 4-bits at a time.
/*--------------------------------------------------------------------------*/
uint8_t LCDReadByte(LCDBitEnum rsBit)
{
#if defined(PCF8574_ADDRESS)
    uint8_t newByte = 0;

    // CGRAM/DDRAM DATA READ:

    // Enable, Register Select (Data), Read/Write (Read), Enable I/O Pins
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(E_BIT | S_BacklightBit | rsBit | RW_BIT | DB7_BIT | DB6_BIT | DB5_BIT | DB4_BIT);
    Wire.endTransmission();
    digitalWrite (E_BIT, HIGH);

    delayMicroseconds (1);

    // Read one byte.
    Wire.requestFrom (PCF8574_ADDRESS, 1);
    //  Store high four bits as high nibble of newBytes. (xxx----)
    newByte = (Wire.read() & 0xf0);

    // Disable, but leave Backlight on.
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(S_BacklightBit);
    Wire.endTransmission();
    delayMicroseconds (1);

    // Enable, Register Select (Data), Read/Write (Read), Enable I/O Pins
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(E_BIT | S_BacklightBit | rsBit | RW_BIT | DB7_BIT | DB6_BIT | DB5_BIT | DB4_BIT);
    Wire.endTransmission();
    delayMicroseconds (1);

    // Read one byte.
    Wire.requestFrom (PCF8574_ADDRESS, 1);
    // Store high four bits as low nibble of newByte. (----xxxx)
    newByte = newByte | (Wire.read() >> 4);

    // Disable, but leave Backlight on.
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(S_BacklightBit);
    Wire.endTransmission();

    delayMicroseconds (53);

    return newByte;
#else
  return 0;
#endif    
}


/*--------------------------------------------------------------------------*/
// 
/*--------------------------------------------------------------------------*/
uint8_t LCDReadDataByte (void)
{
    return LCDReadByte(RS_BIT);
}


/*--------------------------------------------------------------------------*/
// 
/*--------------------------------------------------------------------------*/
void LCDReadData (uint8_t *bufferPtr, uint8_t bufferSize)
{
    for (int idx = 0; idx < bufferSize; idx++)
    {
        bufferPtr[idx] = LCDReadDataByte();
    }
}


/*--------------------------------------------------------------------------*/
// 
/*--------------------------------------------------------------------------*/
void LCDReadDataDDRAM (uint8_t DDRAMAddress, uint8_t *bufferPtr, uint8_t bufferSize)
{
    if ((bufferPtr != NULL) && (bufferSize != 0))
    {
        if (bufferSize > 128)
        {
            bufferSize = 128;
        }

        LCDSetDDRAMAddress (DDRAMAddress);
        LCDReadData (bufferPtr, bufferSize);
    }
}


/*--------------------------------------------------------------------------*/
// 
/*--------------------------------------------------------------------------*/
void LCDReadDataCGRAM (uint8_t *bufferPtr, uint8_t bufferSize)
{
    if ((bufferPtr != NULL) && (bufferSize != 0))
    {
        if (bufferSize > 64)
        {
            bufferSize = 64;
        }

        LCDSetCGRAMAddress (0);
        LCDReadData (bufferPtr, bufferSize);
    }
}


/*--------------------------------------------------------------------------*/
// 
/*--------------------------------------------------------------------------*/
void LCDReadDataCGRAMCharacter (uint8_t characterNumber, uint8_t *bufferPtr, uint8_t bufferSize)
{

}



/*--------------------------------------------------------------------------*/
// 
/*--------------------------------------------------------------------------*/
uint8_t LCDReadBusyFlagAndAddressCounter (void)
{
    return LCDReadByte(0);
}

uint8_t LCDReadAddressCounter (void)
{
    return (LCDReadByte(0) & LCD_DDRAM_ADDR_MASK);
}


/*--------------------------------------------------------------------------*/
//
// This one is simpler since our BF bit is in the high nibble so we do not
// need to do two reads to retrieve the full 8-bits.
/*--------------------------------------------------------------------------*/
void LCDWaitForBusyFlag(void)
{
#if defined(PCF8574_ADDRESS)
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(S_BacklightBit | RW_BIT | DB7_BIT | DB6_BIT | DB5_BIT | DB4_BIT);
    Wire.endTransmission();
    delayMicroseconds(1);

    // Enable, Read, Backlight On, Pins as Input
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(E_BIT | S_BacklightBit | RW_BIT | DB7_BIT | DB6_BIT | DB5_BIT | DB4_BIT);
    Wire.endTransmission();
    delayMicroseconds(1);

    // TODO: Timeout to avoid a deadlock due to I2C issues.
    uint16_t timeout = 65535;
    while (timeout-- != 0)
    {
        // Read I/O pins.
        Wire.requestFrom(PCF8574_ADDRESS, 1);
        if ((Wire.read() & DB7_BIT) == 0)
        {
            break;
        }
    };

    // Disable, leaving Backlight on.
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(S_BacklightBit);
    Wire.endTransmission();
#else // GPIO
    // TODO: Poll for ... D7?
#endif    
}

/*--------------------------------------------------------------------------*/
// Clear the LCD (and home the cursor position).
/*--------------------------------------------------------------------------*/
void LCDSetDisplay(bool displayOn)
{
    if (displayOn == true)
    {
        S_DisplayControlBits |= D_BIT;
    }
    else
    {
        S_DisplayControlBits &= ~D_BIT;
    }

    LCDDisplayOnOffControl (S_DisplayControlBits);
}

void LCDDisplayOn(void)
{
    LCDSetDisplay(true);
}

void LCDDisplayOff(void)
{
    LCDSetDisplay(false);
}


/*--------------------------------------------------------------------------*/
// Clear the LCD (and home the cursor position).
/*--------------------------------------------------------------------------*/
void LCDSetCursor(bool cursorOn)
{
    if (cursorOn == true)
    {
        S_DisplayControlBits |= C_BIT;
    }
    else
    {
        S_DisplayControlBits &= ~C_BIT;
    }

    LCDDisplayOnOffControl (S_DisplayControlBits);
}

void LCDCursorOn(void)
{
    LCDSetCursor(true);
}

void LCDCursorOff(void)
{
    LCDSetCursor(false);
}


/*--------------------------------------------------------------------------*/
// Clear the LCD (and home the cursor position).
/*--------------------------------------------------------------------------*/
void LCDSetCursorBlink(bool cursorBlinkOn)
{
    if (cursorBlinkOn == true)
    {
        S_DisplayControlBits |= B_BIT;
    }
    else
    {
        S_DisplayControlBits &= ~B_BIT;
    }

    LCDDisplayOnOffControl (S_DisplayControlBits);
}

void LCDCursorBlinkOn(void)
{
    LCDSetCursorBlink(true);
}

void LCDCursorBlinkOff(void)
{
    LCDSetCursorBlink(false);
}


/*--------------------------------------------------------------------------*/
// Set the backlight on or off.
/*--------------------------------------------------------------------------*/
void LCDSetBacklight(bool backlightOn)
{
    if (backlightOn == true)
    {
        S_BacklightBit |= BL_BIT;
    }
    else
    {
        S_BacklightBit &= ~BL_BIT;
    }

    // Manually set backlight bit.
#if defined(PCF8574_ADDRESS) 
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(S_BacklightBit);
    Wire.endTransmission();
#else // GPIO
    // TODO: If backlight is wired to an I/O pin.
#endif    
}

void LCDSetBacklightOn(void)
{
    LCDSetBacklight(true);
}

void LCDSetBacklightOff(void)
{
    LCDSetBacklight(false);
}

#endif /* LCD2004_PCF8547_C */

/*---------------------------------------------------------------------------*/
// End of LCD2004_PCF8547.c
