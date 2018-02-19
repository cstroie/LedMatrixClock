/**
  DotMatrix.h

  Copyright (c) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrixClock.
*/

#ifndef DOTMATRIX_H
#define DOTMATRIX_H

#define MAXMATRICES 8

#include "Arduino.h"

enum DotMatrixOps {OP_NOOP, OP_DIGIT0, OP_DIGIT1, OP_DIGIT2, OP_DIGIT3, OP_DIGIT4,
                   OP_DIGIT5, OP_DIGIT6, OP_DIGIT7, OP_DECODEMODE, OP_INTENSITY,
                   OP_SCANLIMIT, OP_SHUTDOWN, OP_DISPLAYTEST, OP_ALL};

class DotMatrix {
  public:
    DotMatrix();
    void init(int dataPin, int clkPin, int csPin, int devices);
    void init(int csPin, int devices);

    void decodemode(uint8_t value);
    void intensity(uint8_t value);
    void scanlimit(uint8_t value);
    void shutdown(bool yesno);
    void displaytest(bool yesno);
    void clear();

    void sendSWSPI(uint8_t matrix, uint8_t reg, uint8_t data);
    void sendHWSPI(uint8_t matrix, uint8_t reg, uint8_t data);
    //void (*sendSPI)(uint8_t,uint8_t,uint8_t);
    void sendAllSWSPI(uint8_t reg, uint8_t data);
    void sendAllHWSPI(uint8_t reg, uint8_t data);
    //void (*sendAllSPI)(uint8_t,uint8_t);
    void sendAllSWSPI(uint8_t reg, uint8_t data[], uint8_t size);
    void sendAllHWSPI(uint8_t reg, uint8_t data[], uint8_t size);

    int SPI_MOSI; /* The clock is signaled on this pin */
    int SPI_CLK;  /* This one is driven LOW for chip selectzion */
    int SPI_CS;   /* The maximum number of devices we use */


    uint8_t matrices;
  private:

    uint8_t buffer[MAXMATRICES * 2] = {0};
};

#endif /* DOTMATRIX_H */
