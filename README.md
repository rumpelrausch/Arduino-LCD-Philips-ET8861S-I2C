# ET8861S I2C

This adds an I2C interface to a specific LCD harvested from an unknown
Philips CD/Radio system.
The interface is based upon an ATTINY85 and uses the Arduino core.

## LCD capabilities

The LCD is equiped with an ET8861S driver. Though there's no public
documentation available it appears to be compatible with the
well-known HT1621 chip.

The display itself contains:
- Seven 13-segment digits
- Two segments representing a dot "." and a colon "."
  right before the last two digits.
- A number of segments representing system state like "MP3", "AUX",
  "CD", "TRACK", "SLEEP" etc.  
  Those **are not handled** by this I2C interface.

So we basically have seven more or less alphanumeric digits and optional
dot and colon at a fixed position.

## Dev environment

- ATTINY board configuration from  
  https://github.com/SpenceKonde/ATTinyCore
- VSCode with the "official" Arduino extension  
  (vsciot-vscode.vscode-arduino)

## General configuration

The main file, "ET8861S_I2C.ino", provides some options as CPP definitions:

- I2C_ADDRESS  
  Default: 0x14
- SHOW_GREETING  
  1 = Shows some greeting upon power on

I2C is implemented using the default "SCL" and "DI" pins. It uses hardware based serial and is tested with ATTINY85 and ATTINY4313 (ATTINY2313 is too small).

## Flashing the ATTINY

The ET8861S shares pins with I2C and flashing. Disconnecting the ET8861S during flash operation is recommended.

## I2C command set

The implementation only provides write access, no read or write/read.

| command | data bytes | description |
| --- | --- | --- |
| 0x0 | 0 | Clear the display |
| 0x1 | 7 | Shows an ASCII string.<br>Not all characters are supported,<br>see ```ET8861.cpp```.|
| 0x2 | 2 | Shows an ASCII character at a given position.<br> The first data byte represents the position (0-6), the second one the character. |
| 0x3 | 1 | Enables / disables the dot and colon symbols (at their fixed position).<br>The data byte is bit-coded: Bit 0 represents colon, bit 1 dot. |
| 0x4 | 3 | Sets arbitrary segments at a given digit position.<br>The first data byte represents the position, the second and third contain the bit-coded segments (MSB first). See `ET8861.cpp` for segment mapping. |

As long as segments are individually set by command 0x4, the according
digit position will not be used by the commands 0x1 and 0x2. Setting
the segment data to zero enables character access to that digit
position. Example:
- Set a single segment at position 2.
- Send a string (command 0x1) "HELLO".

Result: "`HE LO`" with the enabled segment at position 2.

