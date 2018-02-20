/**
  DotMatrix.cpp

  Basic code for using Maxim MAX7219/MAX7221 with Arduino.
  Wire the Arduino and the MAX7219/MAX7221 together as follows:
  | Arduino   | MAX7219/MAX7221 |
  | --------- | --------------- |
  | MOSI (11) | DIN (1)         |
  | SCK (13)  | CLK (13)        |
  | I/O (7)*  | LOAD/CS (12)    |

  Copyright (c) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrixClock.
*/

#include "Arduino.h"
#include <SPI.h>

#include "DotMatrix.h"

DotMatrix::DotMatrix() {
}

void DotMatrix::init(int csPin, int devices) {
  /* Pin configuration */
  SPI_CS = csPin;
  /* The number of matrices */
  if (devices <= 0 || devices > MAXMATRICES) devices = MAXMATRICES;
  matrices = devices;
  /* Configure the pins and SPI */
  pinMode(MOSI, OUTPUT);
  pinMode(SCK, OUTPUT);
  pinMode(SPI_CS, OUTPUT);
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  SPI.begin();
  digitalWrite(SPI_CS, HIGH);
}

void DotMatrix::decodemode(uint8_t value) {
  sendAllSPI(OP_DECODEMODE, value & 0x0F);
}

void DotMatrix::intensity(uint8_t value) {
  sendAllSPI(OP_INTENSITY, value & 0x0F);
}

void DotMatrix::scanlimit(uint8_t value) {
  scanlines = ((value - 1) & 0x07) + 1;
  sendAllSPI(OP_SCANLIMIT, scanlines - 1);
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
  for (uint8_t l = 0; l < scanlines; l++)
    sendAllSPI(l + 1, 0x00);
}

void DotMatrix::clearFrameBuffer() {
  memset(frameBuffer, 0, matrices * scanlines);
}

/**
  Display the framebuffer
*/
void DotMatrix::display() {
  /* Repeat for each line in matrix */
  for (uint8_t i = 0; i < scanlines; i++) {
    /* Compose an array containing the same line in all matrices */
    uint8_t data[matrices] = {0};
    /* Fill the array from the frambuffer */
    for (uint8_t m = 0; m < matrices; m++)
      data[m] = frameBuffer[m * scanlines + i];
    /* Send the array */
    sendAllSPI(i + 1, data, matrices);
  }
}

/**
  Send data to one device
*/
void DotMatrix::sendSPI(uint8_t matrix, uint8_t reg, uint8_t data) {
  uint8_t offset = matrix * 2;

  /* Clear the command buffer */
  memset(cmdBuffer, 0, matrices * 2);
  /* Write the data into command buffer */
  cmdBuffer[offset] = data;
  cmdBuffer[offset + 1] = reg;

  /* Chip select */
  digitalWrite(SPI_CS, LOW);
  /* Send the data */
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0));
  for (int i = matrices * 2; i > 0; i--)
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
  for (int i = matrices; i > 0; i--) {
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

