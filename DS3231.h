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

// Default century
#define CENTURY   19


class DS3231 {
  public:
    DS3231();
    bool      init(uint8_t rtcAddr = I2C_RTC, bool twInit = true);
    bool      readTime(bool readDate = false);
    bool      readTimeBCD();
    bool      writeDateTime(const uint8_t s, const uint8_t m, const uint8_t h, const uint8_t w, const uint8_t d, const uint8_t b, const uint8_t y);
    bool      rtcOk = false;

    // The names of the variables are inspired by man date(1)
    uint8_t   S; // second 00..59
    uint8_t   M; // minute 00..59
    uint8_t   H; // hour   00..24
    uint8_t   I; // hour    1..12
    bool      P; // AM/PM
    uint8_t   z; // time zone (hours only)
    uint8_t   u; // DoW     1..7, 1 is Monday
    uint8_t   d; // day    00..31
    uint8_t   m; // month  00..12
    uint8_t   y; // year   00..99
    uint16_t  C; // century, e.g. 1900 or 2000
    uint16_t  Y; // year, including millenium and century
    uint8_t   R[4]; // 24-hour hour and minute, unpacked BCD

  private:
    uint8_t   rtcAddr = I2C_RTC;
};

#endif /* DS3231_H */
