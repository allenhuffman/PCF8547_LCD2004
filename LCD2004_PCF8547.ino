/*---------------------------------------------------------------------------*/
/*
  Test program for LCD2004/LCD1602 via PCF8574 I2C Interface
  By Allen C. Huffman
  www.subethasoftware.com

  REFERENCES:
  
  CONFIGURATION:
  
  VERSION HISTORY:
  2022-08-31 0.0 allenh - In the beginning...
  2022-12-07 0.1 allenh - Initial support for 4-bit GPIO mode.
  
  TODO:
  
  TOFIX:

*/
/*---------------------------------------------------------------------------*/
// TI PCF8574 (or compatible)

#include <stdint.h>

#include <Wire.h>

#include "LCD2004_PCF8547.h"

#define LCD_WIDTH   20
#define LCD_HEIGHT  4

const uint8_t pacmanGGRAMData[] =
{
  // Pac Close
  0b01110,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b11111,
  0b01110,

  // Pack Right
  0b01110,
  0b11111,
  0b11110,
  0b11100,
  0b11100,
  0b11110,
  0b11111,
  0b01110,

  // Ghost 1
  0b01110,
  0b11111,
  0b11111,
  0b10101,
  0b11111,
  0b11111,
  0b11111,
  0b10101,

  // Ghost 2
  0b01110,
  0b11111,
  0b11111,
  0b10101,
  0b11111,
  0b11111,
  0b11111,
  0b01010,

  // Dot
  0b00000,
  0b00000,
  0b00000,
  0b01100,
  0b01100,
  0b00000,
  0b00000,
  0b00000,

  // Maze Left
  0b00000,
  0b01111,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b01111,
  0b00000,

  // Maze Center
  0b00000,
  0b11111,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b11111,
  0b00000,

  // Maze Right
  0b00000,
  0b11110,
  0b00001,
  0b00001,
  0b00001,
  0b00001,
  0b11110,
  0b00000,
};


/*--------------------------------------------------------------------------*/
// Setup
/*--------------------------------------------------------------------------*/
void setup()
{
  Serial.begin(9600);
  Wire.begin(); // I2C Master

  LCDInit ();

  for (int idx=0; idx<3; idx++)
  {
    LCDSetBacklightOn();
    delay(100);
    LCDSetBacklightOff();
    delay(100);
  }
  LCDSetBacklightOn();

  // Wait for Busy Flag test:

  LCDWriteDataStringXY(0,0,"Hello");
  Serial.println ("Clear");
  LCDWriteInstructionByte(LCD_CLEAR_DISPLAY);
  LCDWaitForBusyFlag();
  Serial.println("Done");

  // Load custom font data.

  LCDClear ();
  LCDWriteDataString (" Programming chars:");

  //LCDSetCGRAMAddress (0);
  //LCDWriteDataCGRAM (&data[0], sizeof(data));

  LCDWriteDataStringXY (6, 1, "01234567");
  LCDSetXY (6,2);
  LCDWriteData ("\0\1\2\3\4\5\6\7\8", 8);

  // Load all custom characters:

  // delay (2000);
  // LCDSetCGRAMAddress (0);
  // LCDWriteDataCGRAM (&data[0], sizeof(data));

  // Load custom characters one at a time:

  for (int ch=0; ch<8; ch++)
  {
    LCDWriteDataCGRAMCharacter (ch, &pacmanGGRAMData[8*ch], 8);
    delay(1000);
  }

  // Read back character RAM and compare to what we sent:

  Serial.print ("DATA : ");
  for (int idx=0; idx<sizeof(pacmanGGRAMData); idx++)
  {
    Serial.print (pacmanGGRAMData[idx], HEX);
    Serial.print (" ");
  }
  Serial.println();

  Serial.print ("CGRAM: ");
  uint8_t CGRAMBuffer[sizeof(pacmanGGRAMData)];
  LCDReadDataCGRAM (CGRAMBuffer, sizeof(CGRAMBuffer));
  for (int idx=0; idx<sizeof(CGRAMBuffer); idx++)
  {
    Serial.print (CGRAMBuffer[idx], HEX);
    Serial.print (" ");
  }
  Serial.println();

  delay (2000);

  // Read Data from Cursor Position Test:

  LCDClear ();
  LCDWriteDataByte (255);
  LCDWriteData ("ABCDEFGH", 8);
  for (int idx=0; idx<20; idx++)
  {
    //LCDSetDDRAMAddress (idx);
    LCDSetXY (idx, 0);
    uint8_t data = LCDReadDataByte();

    //LCDSetDDRAMAddress (64+idx);
    LCDSetXY (idx, 2);
    LCDWriteDataByte (data);
  }
  delay (2000);

  // Read Display RAM test:

  LCDClear ();
  LCDWriteDataStringXY (0,0, "12345678901234567890");
  LCDWriteDataStringXY (0,1, "ABCDEFGHIJKLMNOPQRST");
  LCDWriteDataStringXY (0,2, "abcdefghjiklmnopqrst");
  LCDWriteDataStringXY (0,3, "09876543210987654321");

  char buffer[80];
  LCDReadDataDDRAM (0, buffer, sizeof(buffer));
  for (int line=0; line<2; line++)
  {
    Serial.print ("Line ");
    Serial.print (line);
    Serial.print (": [ ");
    
    for (int pos=(line*20); pos<(line*20)+20; pos++)
    {
      Serial.print(buffer[pos]);
    }
    Serial.println (" ]");
  }

  delay (4000);

  // Backlight test:

  LCDClear ();
  LCDWriteDataString ("Backlight Test:");
  for (int idx=0; idx<2; idx++)
  {
    LCDSetBacklight(false);
    delay(250);
    LCDSetBacklight(true);
    delay(250);
  }
  delay(1000);

  // Cursor test:

  LCDClear();
  for (int idx=0; idx<2; idx++)
  {
    LCDWriteDataStringXY (0, 0, "Cursor On :");
    LCDSetCursor(true);
    delay(1000);
    LCDWriteDataStringXY (0, 0, "Cursor Off:");
    LCDSetCursor(false);
    delay(1000);
  }

  LCDClear ();
  for (int idx=0; idx<2; idx++)
  {
    LCDWriteDataStringXY (0, 0, "Blinking On :");
    LCDSetCursorBlink(true);
    delay(1000);
    LCDWriteDataStringXY (0, 0, "Blinking Off:");
    LCDSetCursorBlink(false);
    delay(1000);
  }

  // You can also have Cursor On and Blink On, and you will see the cursor
  // when the blinking square is off.

  LCDClear();
  LCDWriteDataStringXY (0, 0, "Display On/Off:");
  for (int idx=0; idx<5; idx++)
  {
    LCDSetDisplay(false);
    delay(1000);
    LCDSetDisplay(true);
    delay(1000);
  }

  // Term test

//   LCDWriteDataString ("Term");
//   delay (1000);
//   LCDTerm ();
//   delay(4000);
//   LCDInit ();
}


/*--------------------------------------------------------------------------*/
// Loop
/*--------------------------------------------------------------------------*/
void loop()
{
  /*------------------------------------------------------------------------*/
  // Full character set
  /*------------------------------------------------------------------------*/
  LCDClear();

  int ch = 0;
  int col = 0;
  while (ch < 256)
  {
    for (int row=0; row<4; row++)
    {
      LCDSetXY (0, row);
      LCDWriteDataByte (getHexDigit (ch >> 4));
      LCDWriteDataByte (getHexDigit (ch & 0xf));
      LCDWriteDataString (": ");
      for (int col=0; col<16; col++)
      {
        LCDWriteDataByte (ch);
        ch++;
      }
    }
    delay(2000);
  }

  /*------------------------------------------------------------------------*/
  // 8 Programmable Characters
  /*------------------------------------------------------------------------*/
  LCDClear ();
  LCDWriteDataStringXY (0, 0, "8 Programmable Chars");

  LCDWriteDataStringXY (1, 2, "0:");
  LCDWriteDataByte (0);
  LCDWriteDataStringXY (6, 2, "1:\1  2:\2  3:\3");

  LCDWriteDataStringXY (1, 3, "4:\4  5:\5  6:\6  7:\7");

  LCDSetCGRAMAddress (0);
  LCDWriteDataCGRAM (&pacmanGGRAMData[0], sizeof(pacmanGGRAMData));

  delay (4000);

  /*------------------------------------------------------------------------*/
  // Pac-Man Demo
  /*------------------------------------------------------------------------*/
  LCDWriteDataStringXY (0, 1, "\5\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\7");
  LCDWriteDataStringXY (0, 2, "\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4");
  LCDWriteDataStringXY (0, 3, "\5\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\7");

  delay (2000);

  for (int x=0; x<24; x++)
  {
    if (x < 20)
    {
      LCDSetXY (x , 2);
      LCDWriteDataByte (x & 1);
    }

    if (x > 4)
    {
      LCDSetXY (x-4, 2);
      LCDWriteDataByte (2 + (x & 1));
    }

    delay(250);
    
    if (x < 20)
    {
      LCDSetXY (x, 2);
      LCDWriteDataByte (32);
    }

    if (x > 4)
    {
      LCDSetXY (x-4, 2);
      LCDWriteDataByte (32);
    }
  }

  delay(4000);

  /*------------------------------------------------------------------------*/
  // Screen Saver
  /*------------------------------------------------------------------------*/
  LCDClear ();

  LCDSetCGRAMAddress (0);
  LCDWriteDataCGRAM (&defaultCGRAMData[0], sizeof(defaultCGRAMData));

  int x = 0;
  int y = 0;
  int xm = 1;
  int ym = 1;

  for (int idx=0; idx<20; idx++)
  {
    LCDSetXY (x, y);
    // Text:
    //LCDWriteDataString (x, y, "Sub-Etha!");
    // Custom characters:
    LCDWriteData ("\0\1\2\3\4\5\6\7", 8);
    delay(250);
    LCDWriteDataStringXY (x, y, "         ");

    x = x + xm;
    if ((x < 1) || (x >= LCD_WIDTH-8-1))
    {
      xm = -xm;
    }

    y = y + ym;
    if ((y < 1) || (y >= LCD_HEIGHT-1))
    {
      ym = -ym;
    }
  }

  //delay(3000);
}


// Utility function.
char getHexDigit(uint8_t nibble)
{
  char hexChar;

  nibble = (nibble & 0x0f);

  if (nibble <= 9)
  {
    hexChar = '0';
  }
  else
  {
    hexChar = 'A'-10;
  }

  return (hexChar + nibble);
}

/*---------------------------------------------------------------------------*/
// End of LCD2004PCF8547.ino
