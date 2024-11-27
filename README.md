# PCF8547_LCD2004 (for Arduino)
This version intends to have full support of all LCD2004/LCD1602 features, but still has some cleanup to do.

2022-12-07 - Added initial support for GPIO mode. This was coded and tested at the online WOKWI.com Arduino simulator. The one I was using had a virtual LCD1602 display, so the output of this demo doesn't show up properly (two 2 of 16 characters, while my demo was written for 4 lines of 20 characters), but the write communication seems to work. The WOKWI simulated LCD1602 does not have the RW line wired up, or the Backlight, so those options do not work in the simulator and are not implemented in my code, for now. My project: https://wokwi.com/projects/350400522467934802

Rest:

I was writing code for a PIC24 processor using the CCS IDE, and used an Arduino to figure out how to talk to this LCD and I2C controller. I thought I'd share the Arduino code in case anyone else is needing to do the same thing.

The LCD2004 (where '20' is the number of characters across and '04' is the number of lines) is interfaced through 11 I/O pins. 8 for Data, and three for Register Select, Read/Write and Enable. It can also be configured to operate in a 4-bit mode where it only uses 4 Data pins, reducing the needed I/O down to 7 I/O pins.

The PCF8547 chip is an I2C to 8 I/O Pin interface. This chip is used on many low-cost LCD interface boards, and is often soldered directly to an LCD making it an I2C LCD display.

There are many (MANY!) Arduino libraries that support this chip and the LCDxxxx modules, but I was out of Flash storage and could not use any of them. Instead, I created just the bare minimum support needed to Initialize the LCD in 4-Bit Mode, Clear the Screen, and Write Text at location X/Y. Since I wrote a routine to write Instruction Bytes, you can also manually send data to load custom characters in to the LCD. It supports 8 custom characters, and I demonstrate how to do this in a simple LCDTest.ino sketch.

This is very simple code, and hopefully documented well enouhg that it helps someone not spend the hours I spent trying to figure this out. I'll have a full tutorial up on SubEthaSoftare.com in the future, mostly because almost all the "tutorials" I found on how to use this LCD screen were not that at all - they were just tutorials on how to install a library and use a function in that library with no details on the PCF8574 or LCD2004 module at all.

This code can also be modified to work with LCD1602 and other screens. The main change is in the Locate X/Y routine. The LCD data is not organized as multiple lines of 20 charaters (or 16). Instead, it is just a buffer of screen memory that is mapped to the display. In the case of the LCD2004, the screen is basically 128 bytes of memory, with the FIRST line being bytes 0-19, the SECOND line being bytes 64-83, the THIRD line being bytes 20-39, and the FOURTH line being bytes 84-103.

Consider this diagram:
<pre>
+--------------------+
|ABCDEFGHIJKLMNOPQRST| (bytes 0-19)
|abcdefghijklmnopqrst| (bytes 64-83)
|UVWXYZ              | (bytes 20-39)
|uvwxyz              | (bytes 84-103)
+--------------------+
</pre>

If you were to start at memory offset 0 (top left of the display) and write 80 bytes of data (thinking you'd get 20, 20, 20 and 20 on the display), that wouldn't happen ;-)  You'd see some of your data did not show up since it was writing out in the memory that is not mapped in to the display. (You can also use that memory for data storage, but I did not implement any READ routines in this code -- yet.)

From the main .ino:

<pre>
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
</pre>

You should be able to easily modify this code to work on an LCD1602 or other configurations of compatible LCD screens just by changing the LCDSetXY() routine offsets. I have LCD1602s so when I get time I will update this code to support them, as well.

Good luck! Drop me a note if you have any questions.
