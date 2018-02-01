/**
  ntp.cpp - Network Time Protocol

  Copyright (c) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrixClock.
*/

#include <Arduino.h>
#include <Wire.h>
#include "DS3231.h"

// Convert to binary coded decimal
uint8_t bin2bcd(uint8_t x) {
  return (x / 10 * 16) + (x % 10);
}

// Convert from binary coded decimal
uint8_t bcd2bin(uint8_t x) {
  return (x / 16 * 10) + (x % 16);
}


DS3231::DS3231() {
}

bool DS3231::init(uint8_t twRTC, bool twInit) {
  // Init i2c
  if (twInit) Wire.begin();
  // Keep the address
  rtcAddr = twRTC;
  // Check the device is present
  Wire.beginTransmission(rtcAddr);
  rtcOk = Wire.endTransmission() == 0;
  return rtcOk;
  // Set century
  cc = CENTURY;
}

/**
  Read the current time from the RTC and unpack it into the object variables

  @return true if the function succeeded
*/
bool DS3231::readTime(bool readDate) {
  // Set DS3231 register pointer to 0x00
  Wire.beginTransmission(rtcAddr);
  Wire.write(0);
  if (Wire.endTransmission() != 0)
    return false;
  // Request 3 or 7 bytes of data starting from 0x00
  if (readDate)
    Wire.requestFrom(rtcAddr, 7);
  else
    Wire.requestFrom(rtcAddr, 3);

  SS = Wire.read();
  SS = bcd2bin(SS);
  MM = Wire.read();
  MM = bcd2bin(MM);
  HH = Wire.read() & 0x3F;
  HH = bcd2bin(HH);
  if (readDate) {
    dw = Wire.read() & 0x07;
    dw = bcd2bin(dw);
    dd = Wire.read();
    dd = bcd2bin(dd);
    mm = Wire.read();
    if (mm & 0x80) cy = 1;
    mm &= 0x1F;
    mm = bcd2bin(mm);
    yy = Wire.read();
    yy = bcd2bin(yy);
    yyyy = 100 * (cc + cy) + yy;
  }
  return true;
}

/**
  Read the current time from the RTC as unpacked BCD in
  preallocated buffer

  @return true if the function succeeded
*/
bool DS3231::readTimeBCD() {
  // Set DS3231 register pointer to 0x01
  Wire.beginTransmission(rtcAddr);
  Wire.write(1);
  if (Wire.endTransmission() != 0)
    return false;
  // Request 3 bytes of data starting from 0x01
  Wire.requestFrom(rtcAddr, 3);

  uint8_t x;
  x = Wire.read();
  HHMM[2] = x / 16; (x & 0xF0) >> 4;
  HHMM[3] = x % 16; (x & 0x0F);
  x = Wire.read() & 0x3F;
  HHMM[0] = x / 16; (x & 0xF0) >> 4;
  HHMM[1] = x % 16; (x & 0x0F);
  return true;
}

/**
   Sets RTC datetime data

   @param uint8_t second second to set to HW RTC
   @param uint8_t minute minute to set to HW RTC
   @param uint8_t hour hour to set to HW RTC
   @param uint8_t dayOfWeek day of week to set to HW RTC
   @param uint8_t dayOfMonth day of month to set to HW RTC
   @param uint8_t month month to set to HW RTC
   @param uint8_t year year to set to HW RTC
*/
bool DS3231::writeDateTime(const uint8_t s, const uint8_t m, const uint8_t h,
                           const uint8_t w, const uint8_t d, const uint8_t b, const uint8_t y) {
  // Set DS3231 register pointer to 0x00
  Wire.beginTransmission(rtcAddr);
  Wire.write(0);
  Wire.write(bin2bcd(s)); // set seconds
  Wire.write(bin2bcd(m)); // set minutes
  Wire.write(bin2bcd(h)); // set hours
  Wire.write(bin2bcd(w)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(bin2bcd(d)); // set day (1 to 31)
  Wire.write(bin2bcd(b)); // set month
  Wire.write(bin2bcd(y)); // set year (0 to 99)
  return (Wire.endTransmission() == 0);
}

