/**
  DS3231.h - Simple DS3231 library

  Copyright (c) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrixClock.
*/

#ifndef DS3231_H
#define DS3231_H

#include <Arduino.h>

/* I2C Addresses
  DS3231 ROM 0x57
  DS3231 RTC 0x68
*/
#define I2C_RTC   0x68
#define I2C_EEP   0x57

// Century
#define CENTURY   20


class DS3231 {
  public:
    DS3231();
    bool          init(uint8_t rtcAddr = I2C_RTC, bool twInit = true);
    bool          readTime(bool readDate = false);
    bool          readTimeBCD();
    bool          writeDateTime(const uint8_t s, const uint8_t m, const uint8_t h, const uint8_t w, const uint8_t d, const uint8_t b, const uint8_t y);
    bool rtcOk = false;
    uint8_t SS; // second 0..59
    uint8_t MM; // minute 0..59
    uint8_t HH; // hour   0..24
    uint8_t dw; // dow    0..6
    uint8_t dd; // day    0..31
    uint8_t mm; // month  0..12
    uint8_t yy; // year   0..99
    uint16_t yyyy;  // year, including millenium and century
    uint8_t HHMM[4];

  private:
    uint8_t rtcAddr = I2C_RTC;
    uint8_t cc; // century
    uint8_t cy; // +1
};

#endif /* DS3231_H */
