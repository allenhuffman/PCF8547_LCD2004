# PCF8547_LCD2004 (DO NOT USE YET!)
LCD2004/LCD1602 via PCF8574 I2C interface board (full functionality support, w.i.p.)

See my PCF8547_LCD2004_Minimal repository for a much much smaller implementation that supports only the bare essentials to initialize, clear and write text to the screen. That version still has ways to manually send Instructions/Data to do most any feature, but does not have simple API functions for them. This version builds on that one, and adds a bunch of extra routines to make code easier to write.

WORK-IN-PROGRESS

Supports:
* Initialize
* Clear Display
* Return Home (home cursor)
* Entry Mode Set - cursor direction and screen shift
* Display On/Off Control - display, cursor, and cursor blink
* Cursor or Display Shift 
* Function Set - configure 8-bit/4-bit, number of display lines, display font type
* Set CGRAM Address - for custom character font data
* Set DDRAM Address - for setting screen cursor position
* Read Busy Flag and Address Counter
* Write Data to RAM
* Read Data from RAM
* Terminate
* Backlight - toggle on or off, via PCF8574 pin

More to come...
