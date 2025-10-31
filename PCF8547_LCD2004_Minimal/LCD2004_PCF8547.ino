#ifndef LCD2004_PCF8547_C
#define LCD2004_PCF8547_C

/*--------------------------------------------------------------------------*/
// Include Files
/*--------------------------------------------------------------------------*/
#include <stdint.h>

#include <Wire.h>

#include "LCD2004_PCF8547.h"


/*--------------------------------------------------------------------------*/
// Variables
/*--------------------------------------------------------------------------*/
static bool S_IsLCDEnabled = false;


/*--------------------------------------------------------------------------*/
// Functions
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
    int ack = 0;

    Wire.begin();

    // Set all PCF8547 I/O pins LOW.
    Wire.beginTransmission(PCF8574_ADDRESS);
    Wire.write(0x0);
    ack = Wire.endTransmission();

    if (ack != 0)
    {
        return false;
    }
    delayMicroseconds(50000);

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

    // Function Set
    // [0  0  1  DL N  F  0  0 ]
    // DL: 1=8-Bit, 0=4-Bit
    //  N: 1=2 Line, 0=1 Line
    //  F: 1=5x10, 0=5x8
    //                      [--001DNF00]
    LCDWriteInstructionByte(0b00101000);

    // Display On
    // [0  0  0  0  1  D  C  B ]
    // D: Display
    // C: Cursor
    // B: Blink
    //                      [--00001DCB]
    LCDWriteInstructionByte(0b00001100);

    // Display Clear
    // [0  0  0  0  0  0  0  1 ]
    LCDWriteInstructionByte(0b00000001);
    delayMicroseconds(3);  // 1.18ms - 2.16ms

    // Entry Mode Set
    // [0  0  0  0  0  1  ID S ]
    // ID: 1=Increment, 0=Decrement
    //  S: 1=Shift based on ID (1=Left, 0=Right)
    //                      [--000001IS]
    LCDWriteInstructionByte(0b00000110);

    S_IsLCDEnabled = true;

    return true;
}


/*--------------------------------------------------------------------------*/
// Disable LCD screen.
/*--------------------------------------------------------------------------*/
void LCDTerm(void)
{
    S_IsLCDEnabled = false;
}


/*--------------------------------------------------------------------------*/
// Write out a 4-bit value.
/*--------------------------------------------------------------------------*/
// [7  6  5  4  3  2  1  0 ]
// [D7 D6 D5 D4 BL -E RW RS]
void LCDWriteInstructionNibble(uint8_t nibble)
{
    uint8_t dataByte = BL_BIT | (nibble << 4);

    Wire.beginTransmission(PCF8574_ADDRESS);

    Wire.write(E_BIT | dataByte);
    delayMicroseconds(1);

    Wire.write(dataByte);
    delayMicroseconds(37);

    Wire.endTransmission();
}


/*--------------------------------------------------------------------------*/
// Write a byte out, 4-bits at a time. The rsBit will determine
// if it is an Instruction (rsBit=0) or Data byte (rsBit=1).
/*--------------------------------------------------------------------------*/
void LCDWriteByte(uint8_t rsBit, uint8_t dataByte)
{
    uint8_t newByte;

    Wire.beginTransmission(PCF8574_ADDRESS);

    newByte = BL_BIT | rsBit | (dataByte & 0xf0);
    Wire.write(E_BIT | newByte);
    delayMicroseconds(1);
    Wire.write(newByte);
    delayMicroseconds(37);

    newByte = BL_BIT | rsBit | (dataByte << 4);
    Wire.write(E_BIT | newByte);
    delayMicroseconds(1);
    Wire.write(newByte);
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
void LCDWriteDataString(uint8_t x, uint8_t y, char *message)
{
    if (S_IsLCDEnabled == true)
    {
        LCDSetXY(x, y);
        LCDWriteData((uint8_t *)message, strlen(message));
    }
}


/*--------------------------------------------------------------------------*/
// Set LCD offset in screen memory.
/*--------------------------------------------------------------------------*/
void LCDSetOffset(uint8_t offset)
{
    // DDRAM AD SET
    LCDWriteInstructionByte(0b10000000 | offset);
    delayMicroseconds(40);
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

    LCDSetOffset(offset);
}


/*--------------------------------------------------------------------------*/
// Clear the LCD (and home the cursor position).
/*--------------------------------------------------------------------------*/
void LCDClear(void)
{
    if (S_IsLCDEnabled == true)
    {
        // Display Clear
        // [0  0  0  0  0  0  0  1 ]
        LCDWriteInstructionByte(0b00000001);
        delay(3); // 1.18ms - 2.16ms
    }
}


// NOT WORKING YET! I was experimenting with doing a READ operation, and
// seeing if I could poll the BF (busy flag) bit.
#if 0
void LCDWaitForBusyFlag (void)
{
  while (1)
  {
    // Toggle on READ mode.
    Wire.beginTransmission (PCF8574_ADDRESS);
    Wire.write (E_BIT | RW_BIT | DB7_BIT);
    Wire.endTransmission ();

    // Read I/O pins.
    Wire.requestFrom (PCF8574_ADDRESS, 1);
    if ((Wire.read() & DB7_BIT) == 0)
    {
      break;
    }

    // Else toggle off E, to start another ready.
    Wire.beginTransmission (PCF8574_ADDRESS);
    Wire.write (RW_BIT | DB7_BIT);
    Wire.endTransmission ();
  }
}
#endif // 0

#endif /* LCD2004_PCF8547_C */

// End of LCD2004_PCF8547.c
