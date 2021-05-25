#include <Arduino.h>
#include <stdio.h>

#include "ET8861.h"
#include "TinyWireS.h"

#define I2C_ADDRESS 0x14
#define SHOW_GREETING 1

/**
 * No data, just the command
 */
#define COMMAND_CLEAR 0x00
/**
 * Followed by 7 bytes character data
 */
#define COMMAND_SET_STRING 0x01
/**
 * Followed by 1 byte position and 1 byte character data
 */
#define COMMAND_SET_CHARACTER 0x02
/**
 * Followed by 1 byte symbol data
 */
#define COMMAND_SET_SYMBOLS 0x03

/**
 * Followed by 1 byte position and 2 bytes segment data,
 * MSB first.
 */
#define COMMAND_SET_SEGMENTS 0x04

static uint8_t receiveByte = 0;
char displayString[8] = "       ";
static bool isDirty = false;

void receiveEvent(uint8_t bytesInBuffer) {
  register uint8_t command;
  if (bytesInBuffer < 1 || bytesInBuffer > 8) {
    return;
  }

  command = TinyWireS.receive();

  if (command == COMMAND_CLEAR && bytesInBuffer == 1) {
    sprintf(displayString, "       ");
    ET8861.unsetAdditionalSymbol(0xFF);
    isDirty = true;
  }

  if (command == COMMAND_SET_STRING && bytesInBuffer == 8) {
    receiveCommandSetString();
    isDirty = true;
  }

  if (command == COMMAND_SET_CHARACTER && bytesInBuffer == 3) {
    receiveCommandSetCharacter();
    isDirty = true;
  }

  if (command == COMMAND_SET_SYMBOLS && bytesInBuffer == 2) {
    receiveCommandSetSymbols();
    isDirty = true;
  }

  if (command == COMMAND_SET_SEGMENTS && bytesInBuffer == 4) {
    receiveCommandSetSegments();
    isDirty = true;
  }

  for (register uint8_t bytesToDrain = TinyWireS.available(); bytesToDrain > 0; bytesToDrain--) {
    TinyWireS.receive();
  }
}

void receiveCommandSetString() {
  for (uint8_t displayPosition = 0; displayPosition < 7; displayPosition++) {
    displayString[displayPosition] = TinyWireS.receive();
  }
}

void receiveCommandSetCharacter() {
  uint8_t position = TinyWireS.receive();
  if (position >= 7) {
    return;
  }
  displayString[position] = TinyWireS.receive();
}

void receiveCommandSetSymbols() {
  uint8_t symbols = TinyWireS.receive();
  ET8861.unsetAdditionalSymbol(0xFF);
  ET8861.setAdditionalSymbol(symbols);
}

void receiveCommandSetSegments() {
  uint8_t position = TinyWireS.receive();
  if (position >= 7) {
    return;
  }
  uint16_t segments = TinyWireS.receive();
  segments <<= 8;
  segments |= TinyWireS.receive();
  ET8861.setSegments(position, segments);
}

void showGreeting() {
  ET8861.setAllSegmensOn();
  delay(500);
  for(register uint8_t position = 0; position < 7; position++) {
    ET8861.setCharacter(position, ' ');
    delay(100);
  }
  ET8861.clear();
  sprintf(displayString, "I2C $%X", I2C_ADDRESS);
  ET8861.setString(displayString);
  ET8861.setSegments(2, 0b0000110001001000);
  delay(3000);
}

void setup() {
  ET8861.enable();
#if SHOW_GREETING
  showGreeting();
#endif
  ET8861.setString("       ");

  TinyWireS.begin(I2C_ADDRESS);
  TinyWireS.onReceive(receiveEvent);
}

void loop() {
  if (isDirty) {
    ET8861.setString(displayString);
    isDirty = false;
  }
  delay(10);
}
