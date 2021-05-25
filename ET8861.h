#ifndef ET8861_h
#define ET8861_h

#include <Arduino.h>
#include <stdint.h>

// #define ET8861_PIN_CS PIN_PA1
// #define ET8861_PIN_WR PIN_PA0
// #define ET8861_PIN_DATA PIN_PD2
#define ET8861_PIN_CS PIN_PB3
#define ET8861_PIN_WR PIN_PB4
#define ET8861_PIN_DATA PIN_PB1

#define ET8861_USE_SLANTED_2 0
#define ET8861_USE_SLANTED_3 1

#define ET8861_BIT_DELAY 0
#define ET8861_NUM_OF_DIGITS 7

#define ET8861_LENGTH_CMDCODE 3
#define ET8861_LENGTH_CMDDATA 8
#define ET8861_LENGTH_ADDRESS 6
#define ET8861_LENGTH_WRITEDATA 8
#define ET8861_CMDCODE_WRITE 0b10100000
#define ET8861_CMDCODE_COMMAND 0b10000000
#define ET8861_COMMAND_SYS_ENABLE 0b00000001
#define ET8861_COMMAND_LCD_ON 0b00000011

#define ET8861_SYMBOL_COLON 0x01
#define ET8861_SYMBOL_DOT 0x02

class ET8861Master {
 public:
  ET8861Master();
  void enable();
  void setSegments(int8_t position, uint16_t data);
  void setCharacter(uint8_t position, char character);
  void setString(char * string);
  void sendCommand(uint8_t command);
  void clear();
  void setAllSegmensOn();
  void setAdditionalSymbol(uint8_t symbolCode);
  void unsetAdditionalSymbol(uint8_t symbolCode);

 private:
  void startTransmission();
  void endTransmission();
  void sendBits(uint8_t data, uint8_t length);
};

extern ET8861Master ET8861;

#endif
