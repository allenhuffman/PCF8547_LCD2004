// LCDTest.ino

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

  // Load custom font data.

  LCDClear ();
  LCDWriteDataString ("Programming chars.");

  //LCDSetCGRAMAddress (0);
  //LCDWriteDataCGRAM (&data[0], sizeof(data));

  LCDWriteDataXYString (0, 1, "01234567");
  LCDSetXY (0,2);
  LCDWriteData ("\0\1\2\3\4\5\6\7\8", 8);

  // delay (2000);
  // LCDSetCGRAMAddress (0);
  // LCDWriteDataCGRAM (&data[0], sizeof(data));

  for (int ch=0; ch<8; ch++)
  {
    LCDWriteDataCGRAMCharacter (ch, &pacmanGGRAMData[8*ch], 8);
    delay(1000);
  }

  LCDSetXY (0,3);
  LCDWriteData ("\0\1\2\3\4\5\6\7\8", 8);

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

  // LCDWriteDataXYString(0,0,"Hello");
  // Serial.println ("Clear");
  // LCDWriteInstructionByte(0b00000001);
  // LCDWaitForBusyFlag();
  // Serial.println("Done");

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

  LCDClear ();
  LCDWriteDataXYString (0,0, "12345678901234567890");
  LCDWriteDataXYString (0,1, "ABCDEFGHIJKLMNOPQRST");
  LCDWriteDataXYString (0,2, "abcdefghjiklmnopqrst");
  LCDWriteDataXYString (0,3, "09876543210987654321");

  // char buffer[128];
  // memset (buffer, 0x0, sizeof(buffer));
  // LCDReadDataDDRAM (0, buffer, sizeof(buffer));

  // Serial.print ("[ ");
  // for (int idx=0; idx<sizeof(buffer); idx++)
  // {
  //   Serial.print(buffer[idx]);
  //   Serial.print (" ");
  // }
  // Serial.println (" ]");

  char buffer[80];
  for (int line=0; line<4; line++)
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

  // for (int idx=0; idx<4; idx++)
  // {
  //   LCDSetBacklight(true);
  //   delay(250);
  //   LCDSetBacklight(false);
  //   delay(250);
  // }
  // delay(1000);

  // LCDClear();
  // LCDWriteDataString ("Cursor On/Off:");
  // for (int idx=0; idx<5; idx++)
  // {
  //   LCDSetCursor(false);
  //   delay(1000);
  //   LCDSetCursor(true);
  //   delay(1000);
  // }
  // LCDSetCursor(false);

  // LCDClear();
  // LCDWriteDataString ("Cursor Blinking:");
  // for (int idx=0; idx<5; idx++)
  // {
  //   LCDSetCursorBlink(false);
  //   delay(1000);
  //   LCDSetCursorBlink(true);
  //   delay(1000);
  // }
  // LCDSetCursorBlink(false);

  // LCDClear();
  // LCDWriteDataString ("Display:");
  // for (int idx=0; idx<5; idx++)
  // {
  //   LCDSetDisplay(false);
  //   delay(1000);
  //   LCDSetDisplay(true);
  //   delay(1000);
  // }

  // LCDWriteDataString ("Term");
  // delay (1000);
  // LCDTerm ();

  // delay(4000);
  // LCDInit ();
  // LCDSetBacklight(false);
}


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
  LCDWriteDataXYString (0, 0, "8 Programmable Chars");

  LCDWriteDataXYString (1, 2, "0:");
  LCDWriteDataByte (0);
  LCDWriteDataXYString (6, 2, "1:\1  2:\2  3:\3");

  LCDWriteDataXYString (1, 3, "4:\4  5:\5  6:\6  7:\7");

  LCDSetCGRAMAddress (0);
  LCDWriteDataCGRAM (&pacmanGGRAMData[0], sizeof(pacmanGGRAMData));

  delay (4000);

  /*------------------------------------------------------------------------*/
  // Pac-Man Demo
  /*------------------------------------------------------------------------*/
  LCDWriteDataXYString (0, 1, "\5\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\7");
  LCDWriteDataXYString (0, 2, "\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4\4");
  LCDWriteDataXYString (0, 3, "\5\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\6\7");

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
    //LCDWriteDataString (x, y, "Sub-Etha!");
    //LCDWriteDataXYString (x, y, "Cellencor");
    LCDSetXY (x, y);
    LCDWriteData ("\0\1\2\3\4\5\6\7", 8);
    delay(250);
    LCDWriteDataXYString (x, y, "         ");

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


// End of LCDTest.ino
