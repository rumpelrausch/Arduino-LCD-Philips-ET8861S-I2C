#include <Arduino.h>
#include "ET8861.h"

/**
 * Bit numbers:
 * 
 *          -----0------
 *       9 |  \  |  /   | 5
 *         | 13\ | /1   |
 *        10 --- | --- 6
 *         | 15/ |  \2  | 
 *      11 |  /  |14 \  | 7
 *          -----3------
 */

const uint16_t segmentsForNumbers[] PROGMEM = {
    0b0000000000000000,  // SPC
    0b0100000000001000,  // !
    0b0000001000100000,  // "
    0b0100010011101000,  // #
    0b0100011011001001,  // $
    0b1000001010000010,  // %
    0b0010110000001111,  // &
    0b0000000000000010,  // '
    0b0000000000000110,  // (
    0b1010000000000000,  // )
    0b1110000000000110,  // *
    0b0100010001000000,  // +
    0b1000000000000000,  // ,
    0b0000010001000000,  // -
    0b0000010000000000,  // .
    0b1000000000000010,  // /
    0b1000101010101011,  // 0
    0b0000000010100010,  // 1
#if ET8861_USE_SLANTED_2
    0b0000110001001011,  // 2
#else
    0b0000110001101001,  // 2
#endif
#if ET8861_USE_SLANTED_3
    0b0000010011001011,  // 3
#else
    0b0000010011101001,  // 3
#endif
    0b0000011011100000,  // 4
    0b0000011011001001,  // 5
    0b0000111011001001,  // 6
    0b0000000010100001,  // 7
    0b0000111011101001,  // 8
    0b0000011011101001,  // 9
};

const uint16_t segmentsForLetters[] PROGMEM = {
    0b1000001011101001,  // @
    0b0000111011100001,  // A
    0b0100000011101001,  // B
    0b0000101000001001,  // C
    0b0100000010101001,  // D
    0b0000111000001001,  // E
    0b0000111000000001,  // F
    0b0000101011001001,  // G
    0b0000111011100000,  // H
    0b0100000000001001,  // I
    0b0000100010101000,  // J
    0b0000111000000110,  // K
    0b0000101000001000,  // L
    0b0010101010100010,  // M
    0b0010101010100100,  // N
    0b0000101010101001,  // O
    0b0000111001100001,  // P
    0b0000101010101101,  // Q
    0b0000111001100101,  // R
    0b0010000011001001,  // S
    0b0100000000000001,  // T
    0b0000101010101000,  // U
    0b1000101000000010,  // V
    0b1000101010100100,  // W
    0b1010000000000110,  // X
    0b1000011000000010,  // Y
    0b1000000000001011,  // Z
};

/**
 * Special bit numbers:
 * 
 * Position 5:
 *  4 = "Mute"
 *  8 = ":"
 * 12 = "."
 * 
 * Position 4:
 *  4 = "SLEEP"
 *  8 = "TRACK"
 * 12 = clock symbol
 * 
 * Position 2:
 *  4 = "AUX"
 * 
 * Position 0:
 *  2 = "MP3"
 *  4 = "CD"
 * 
 */

/**
 * Bits:
 * 0 : ":"
 * 1 : "."
 */
static uint8_t additionalSymbols = 0;

void ET8861Master::enable() {
  pinMode(ET8861_PIN_CS, OUTPUT);
  pinMode(ET8861_PIN_WR, OUTPUT);
  pinMode(ET8861_PIN_DATA, OUTPUT);
  digitalWrite(ET8861_PIN_CS, HIGH);
  digitalWrite(ET8861_PIN_WR, HIGH);
  digitalWrite(ET8861_PIN_DATA, HIGH);
  delay(10);
  sendCommand(ET8861_COMMAND_SYS_ENABLE);  // SYS Enable
  sendCommand(ET8861_COMMAND_LCD_ON);      // LCD ON
  sendCommand(0b00101001); // bias & COM
  clear();
}

void ET8861Master::startTransmission() {
  digitalWrite(ET8861_PIN_CS, LOW);
  digitalWrite(ET8861_PIN_WR, HIGH);
  digitalWrite(ET8861_PIN_DATA, HIGH);
  delayMicroseconds(ET8861_BIT_DELAY);
}

void ET8861Master::endTransmission() {
  delayMicroseconds(ET8861_BIT_DELAY);
  digitalWrite(ET8861_PIN_CS, HIGH);
  digitalWrite(ET8861_PIN_DATA, HIGH);
  digitalWrite(ET8861_PIN_CS, HIGH);
}

void ET8861Master::sendBits(uint8_t data, uint8_t length) {
  register uint8_t i;

  for (i = 0; i < length; i++, data <<= 1) {
    digitalWrite(ET8861_PIN_WR, LOW);
    delayMicroseconds(ET8861_BIT_DELAY);
    digitalWrite(ET8861_PIN_DATA, data & 0b10000000 ? HIGH : LOW);
    delayMicroseconds(ET8861_BIT_DELAY);
    digitalWrite(ET8861_PIN_WR, HIGH);
    delayMicroseconds(ET8861_BIT_DELAY);
  }
}

void ET8861Master::setSegments(int8_t position, uint16_t segments) {
  if (position == 5) {
    if (additionalSymbols & 0x01) {
        segments |= 0b0000000100000000;
      }
    if (additionalSymbols & 0x02) {
        segments |= 0b0001000000000000;
      }
  }
  startTransmission();
  sendBits(ET8861_CMDCODE_WRITE, ET8861_LENGTH_CMDCODE);
  sendBits((ET8861_NUM_OF_DIGITS - 1 - position) << 4, ET8861_LENGTH_ADDRESS);
  sendBits(segments, ET8861_LENGTH_WRITEDATA);
  sendBits(segments >> ET8861_LENGTH_WRITEDATA, ET8861_LENGTH_WRITEDATA);
  endTransmission();
}

void ET8861Master::sendCommand(uint8_t command) {
  startTransmission();
  sendBits(ET8861_CMDCODE_COMMAND, ET8861_LENGTH_CMDCODE);
  sendBits(command, ET8861_LENGTH_CMDDATA);
  sendBits(0, 1);
  endTransmission();
}

void ET8861Master::clear() {
  register int8_t i;
  for (i = -1; i < ET8861_NUM_OF_DIGITS; i++) {
    setSegments(i, 0);
  }
}

void ET8861Master::setAllSegmensOn() {
  register int8_t i;
  for (i = -1; i < ET8861_NUM_OF_DIGITS; i++) {
    setSegments(i, 0xffff);
  }
}

void ET8861Master::setCharacter(uint8_t position, char character) {
  if (character >= 'a' && character <= 'z') {
    character -= 32;
  }

  if (character >= ' ' && character <= '9') {
    return ET8861.setSegments(position, pgm_read_word(segmentsForNumbers + character - 0x20));
  }
  if (character >= '@' && character <= 'Z') {
    return ET8861.setSegments(position, pgm_read_word(segmentsForLetters + character - 0x40));
  }
}

void ET8861Master::setString(char* string) {
  register uint8_t i;
  for (i = 0; i < strlen(string) && i < ET8861_NUM_OF_DIGITS; i++) {
    setCharacter(i, string[i]);
  }
}

void ET8861Master::setAdditionalSymbol(uint8_t symbolCode) {
  additionalSymbols |= symbolCode;
}

void ET8861Master::unsetAdditionalSymbol(uint8_t symbolCode) {
  additionalSymbols &= ~ symbolCode;
}
