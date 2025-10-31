# PCF8547_LCD2004 Minimal (for Arduino)
Bare minimum Arduino code for **LCD2004** (LCD1602 with changes, etc.) and **PCF8547** I2C interface chip. For when size matters.

**2022-12-07 TODO:** I just updated the "full" version of this code to support GPIO mode, based on using the WOKWI.com Arduino simulator. I need to split out the extra code from the full version in a separate file, so it can all be one project. Include some, or all, to get the levels of functionality. If you want GPIO support, you can see the changes I made to the Write functions in that project (simple changes) plus some config stuff for I/O pins in the header file. That code now conditionally compiles -- if you define the I2C address, it uses the Wire library, else GPIO.

Check my other repositories for a larger, fuller version of this code that supports all of features of the LCD.

This code supports the following:
* Initialize the LCD2004 (or LCD1602).
* Clear Screen
* Set Cursor Position and Write String
* Send Instruction Byte
* Send Data Byte (single)
* Send Data Bytes (buffer)

Using `LCDInit()`, `LCDClear()` and `LCDWriteDataString(0, 0, "Hello ,world!")` you can do basic stuff. You can also use the support functions to manually send any custom Instruction or Data, such as loading character RAM for custom characters.

---
I was writing code for a PIC24 processor using the CCS IDE, and used an Arduino to figure out how to talk to this LCD and I2C controller. I thought I'd share the Arduino code in case anyone else is needing to do the same thing.

The **LCD2004** (where '20' is the number of characters across and '04' is the number of lines) is interfaced through 11 I/O pins. 8 for Data, and three for Register Select, Read/Write and Enable. It can also be configured to operate in a 4-bit mode where it only uses 4 Data pins, reducing the needed I/O down to 7 I/O pins.

The **PCF8547** chip is an I2C to 8 I/O Pin interface. This chip is used on many low-cost LCD interface boards, and is often soldered directly to an LCD making it an I2C LCD display.

There are many (MANY!) Arduino libraries that support this chip and the LCDxxxx modules, but I was out of Flash storage and could not use any of them. Instead, I created just the bare minimum support needed to Initialize the LCD in 4-Bit Mode, Clear the Screen, and Write Text at location X/Y. Since I wrote a routine to write Instruction Bytes, you can also manually send data to load custom characters in to the LCD. It supports 8 custom characters, and I demonstrate how to do this in a simple LCDTest.ino sketch.

This is very simple code, and hopefully documented well enouhg that it helps someone not spend the hours I spent trying to figure this out. I'll have a full tutorial up on SubEthaSoftare.com in the future, mostly because almost all the "tutorials" I found on how to use this LCD screen were not that at all - they were just tutorials on how to install a library and use a function in that library with no details on the PCF8574 or LCD2004 module at all.

This code can also be modified to work with LCD1602 and other screens. The main change is in the Locate X/Y routine. The LCD data is not organized as multiple lines of 20 charaters (or 16). Instead, it is just a buffer of screen memory that is mapped to the display. In the case of the LCD2004, the screen is basically 128 bytes of memory, with the FIRST line being bytes 0-19, the SECOND line being bytes 64-83, the THIRD line being bytes 20-39, and the FOURTH line being bytes 84-103.

Consider this diagram:
```
+--------------------+
|ABCDEFGHIJKLMNOPQRST| (bytes 0-19)
|abcdefghijklmnopqrst| (bytes 64-83)
|UVWXYZ              | (bytes 20-39)
|uvwxyz              | (bytes 84-103)
+--------------------+
```
If you were to start at memory offset 0 (top left of the display) and write 80 bytes of data (thinking you'd get 20, 20, 20 and 20 on the display), that wouldn't happen ;-)  You'd see some of your data did not show up since it was writing out in the memory that is not mapped in to the display. (You can also use that memory for data storage, but I did not implement any READ routines in this code -- yet.)

From the main .ino:
```
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
```
You should be able to easily modify this code to work on an **LCD1602** or other configurations of compatible LCD screens just by changing the `LCDSetXY()` routine offsets. I now have some **LCD1602**s so when I get time I will update this code to support them, as well.

Good luck! Drop me a note if you have any questions.
