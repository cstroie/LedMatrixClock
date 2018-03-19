/**
  DS3231.h - Simple DS3231 library

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

// DS3232 Register Addresses
#define RTC_SECONDS 0x00
#define RTC_MINUTES 0x01
#define RTC_HOURS   0x02
#define RTC_DAY     0x03
#define RTC_DATE    0x04
#define RTC_MONTH   0x05
#define RTC_YEAR    0x06
#define AL1_SECONDS 0x07
#define AL1_MINUTES 0x08
#define AL1_HOURS   0x09
#define AL1_DAYDATE 0x0A
#define AL2_MINUTES 0x0B
#define AL2_HOURS   0x0C
#define AL2_DAYDATE 0x0D
#define RTC_CONTROL 0x0E
#define RTC_STATUS  0x0F
#define RTC_AGING   0x10
#define RTC_TMP_MSB 0x11
#define RTC_TMP_LSB 0x12


class DS3231 {
  public:
    DS3231();
    bool      init(uint8_t rtcAddr = I2C_RTC, bool twInit = true);
    bool      readTime(bool readDate = false);
    bool      readTimeBCD();
    int8_t    readTemperature(bool metric = true);
    bool      lostPower();
    uint8_t   checkAlarms();
    bool      writeDateTime(uint8_t S, uint8_t M, uint8_t H, uint8_t d, uint8_t m, uint16_t Y);
    bool      resetSeconds();
    bool      setMinutes(int8_t dir = 1, bool readRTC = true);
    bool      setHours(int8_t dir = 1, bool readRTC = true);

    uint8_t   getDOW(uint16_t year, uint8_t month, uint8_t day);

    bool      dstCheck(uint16_t year, uint8_t month, uint8_t day, uint8_t hour);
    int8_t    dstAdjust(uint16_t year, uint8_t month, uint8_t day, uint8_t hour, bool dstFlag);
    int8_t    dstSelfAdjust(bool dstFlag);

    // The names of the variables are inspired by man date(1)
    uint8_t   S; // second 00..59
    uint8_t   M; // minute 00..59
    uint8_t   H; // hour   00..24
    uint8_t   I; // hour    1..12
    bool      P; // AM/PM, true if PM
    uint8_t   z; // time zone (hours only)
    uint8_t   u; // DoW     1..7, 1 is Monday
    uint8_t   d; // day    00..31
    uint8_t   m; // month  00..12
    uint8_t   y; // year   00..99
    uint16_t  C; // century, e.g. 1900 or 2000
    uint16_t  Y; // year, including millenium and century

    uint8_t   R[4]; // 24-hour hour and minute, unpacked BCD

    // Flags
    bool      rtcOk = false;

  private:
    uint8_t   rtcAddr = I2C_RTC;
};

#endif /* DS3231_H */
