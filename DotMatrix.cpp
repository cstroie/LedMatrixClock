/**
  DotMatrix.cpp - MAX7219/MAX7221 SPI interface

  Copyright (C) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.


  Wiring

  | Arduino   | MAX7219/MAX7221 |
  | --------- | --------------- |
  | MOSI (11) | DIN      (1)    |
  | SCK  (13) | CLK     (13)    |
  | I/O   (7) | LOAD/CS (12)    |
*/

#include "Arduino.h"
#include <SPI.h>

#include "DotMatrix.h"

DotMatrix::DotMatrix() {
}

void DotMatrix::init(uint8_t csPin, uint8_t devices, uint8_t lines) {
  /* Pin configuration */
  SPI_CS = csPin;
  /* The number of matrices */
  if (devices <= 0 || devices > MAXMATRICES)
    devices = MAXMATRICES;
  _devices = devices;
  /* Configure the pins and SPI */
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(SPI_CS, OUTPUT);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
  digitalWrite(SPI_CS, HIGH);
  /* Set the scan limit */
  this->scanlimit(lines);
}

void DotMatrix::decodemode(uint8_t value) {
  sendAllSPI(OP_DECODEMODE, value & 0x0F);
}

void DotMatrix::intensity(uint8_t value) {
  sendAllSPI(OP_INTENSITY, value & 0x0F);
}

void DotMatrix::scanlimit(uint8_t value) {
  _scanlimit = ((value - 1) & 0x07) + 1;
  sendAllSPI(OP_SCANLIMIT, _scanlimit - 1);
}

void DotMatrix::shutdown(bool yesno) {
  uint8_t data = yesno ? 0 : 1;
  sendAllSPI(OP_SHUTDOWN, data);
}

void DotMatrix::displaytest(bool yesno) {
  uint8_t data = yesno ? 1 : 0;
  sendAllSPI(OP_DISPLAYTEST, data);
}

void DotMatrix::clear() {
  for (uint8_t l = 0; l < _scanlimit; l++)
    sendAllSPI(l + 1, 0x00);
}

/**
  Load the specified font into RAM

  @param font the font id
*/
void DotMatrix::loadFont(uint8_t font) {
  uint8_t chrbuf[8] = {0};
  font %= fontCount;
  // Load each character into RAM
  for (int i = 0; i < fontSize; i++) {
    // Load into temporary buffer
    memcpy_P(&chrbuf, &FONTS[font][i], 8);
    // Rotate
    for (uint8_t j = 0; j < 8; j++) {
      for (uint8_t k = 0; k < 8; k++) {
        FONT[i][7 - k] <<= 1;
        FONT[i][7 - k] |= (chrbuf[j] & 0x01);
        chrbuf[j] >>= 1;
      }
    }
  }
}

/**
  Clear the framebuffer
*/
void DotMatrix::fbClear() {
  memset(fbData, 0, _devices * _scanlimit);
}

/**
  Display the framebuffer
*/
void DotMatrix::fbDisplay() {
  /* Repeat for each line in matrix */
  for (uint8_t i = 0; i < _scanlimit; i++) {
    /* Compose an array containing the same line in all matrices */
    uint8_t data[_devices] = {0};
    /* Fill the array from the frambuffer */
    for (uint8_t m = 0; m < _devices; m++)
      data[m] = fbData[m * _scanlimit + i];
    /* Send the array */
    sendAllSPI(i + 1, data, _devices);
  }
}

/**
  Print a charater at the specified position

  @param pos position (rightmost)
  @param digit the character/digit to print
*/
void DotMatrix::fbPrint(uint8_t pos, uint8_t digit) {
  for (uint8_t l = 0; l < fontWidth; l++)
    fbData[pos + l] |= FONT[digit][l];
}

/**
  Send data to one device
*/
void DotMatrix::sendSPI(uint8_t matrix, uint8_t reg, uint8_t data) {
  uint8_t offset = matrix * 2;

  /* Clear the command buffer */
  memset(cmdBuffer, 0, _devices * 2);
  /* Write the data into command buffer */
  cmdBuffer[offset] = data;
  cmdBuffer[offset + 1] = reg;

  /* Chip select */
  digitalWrite(SPI_CS, LOW);
  /* Send the data */
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  for (int i = _devices * 2; i > 0; i--)
    SPI.transfer(cmdBuffer[i - 1]);
  SPI.endTransaction();
  /* Latch data */
  digitalWrite(SPI_CS, HIGH);
}

/**
  Send the same data to all device, at once
*/
void DotMatrix::sendAllSPI(uint8_t reg, uint8_t data) {
  /* Chip select */
  digitalWrite(SPI_CS, LOW);
  /* Send the data */
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  for (int i = _devices; i > 0; i--) {
    SPI.transfer(reg);
    SPI.transfer(data);
  }
  SPI.endTransaction();
  /* Latch data */
  digitalWrite(SPI_CS, HIGH);
}

/**
  Send the data in array to device, at once
*/
void DotMatrix::sendAllSPI(uint8_t reg, uint8_t* data, uint8_t size) {
  /* Chip select */
  digitalWrite(SPI_CS, LOW);
  /* Send the data */
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  for (int i = size; i > 0; i--) {
    SPI.transfer(reg);
    SPI.transfer(data[i - 1]);
  }
  SPI.endTransaction();
  /* Latch data */
  digitalWrite(SPI_CS, HIGH);
}

