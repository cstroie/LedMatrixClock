/**
  DotMatrix.h

  Copyright (c) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrixClock.
*/

#ifndef DOTMATRIX_H
#define DOTMATRIX_H

#define MAXMATRICES  8
#define MAXSCANLIMIT 8

#include "Arduino.h"

enum DotMatrixOps {OP_NOOP, OP_DIGIT0, OP_DIGIT1, OP_DIGIT2, OP_DIGIT3, OP_DIGIT4,
                   OP_DIGIT5, OP_DIGIT6, OP_DIGIT7, OP_DECODEMODE, OP_INTENSITY,
                   OP_SCANLIMIT, OP_SHUTDOWN, OP_DISPLAYTEST = 0x0F
                  };

class DotMatrix {
  public:
    DotMatrix();
    void init(int csPin, int devices);

    void decodemode(uint8_t value);
    void intensity(uint8_t value);
    void scanlimit(uint8_t value);
    void shutdown(bool yesno);
    void displaytest(bool yesno);
    void clear();
    void clearFrameBuffer();

    void display();

    void sendSPI(uint8_t matrix, uint8_t reg, uint8_t data);
    void sendAllSPI(uint8_t reg, uint8_t data);
    void sendAllSPI(uint8_t reg, uint8_t* data, uint8_t size);


    uint8_t matrices  = MAXMATRICES;
    uint8_t scanlines = MAXSCANLIMIT;

    uint8_t frameBuffer[MAXMATRICES * MAXSCANLIMIT] = {0};

  private:
    uint8_t cmdBuffer[MAXMATRICES * 2] = {0};
    int SPI_CS;   /* Chip Select pin */
};

#endif /* DOTMATRIX_H */
