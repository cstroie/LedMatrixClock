/**
  ntp.cpp - Network Time Protocol

  Copyright (c) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrixClock.
*/

#include <Arduino.h>
#include <Wire.h>
#include "DS3231.h"

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
  SS = BCD2DEC(Wire.read());
  MM = BCD2DEC(Wire.read());
  HH = BCD2DEC(Wire.read() & 0x3F);
  if (readDate) {
    dw = BCD2DEC(Wire.read() & 0x07);
    dd = BCD2DEC(Wire.read());
    mm = Wire.read();
    if (mm & 0x80) cy = 1;
    mm = BCD2DEC(mm & 0x1F);
    yy = BCD2DEC(Wire.read());
    yyyy = 100 * (cc + cy) + yy;
  }
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
bool DS3231::writeDateTime(const uint8_t s, const uint8_t m, const uint8_t h, const uint8_t w, const uint8_t d, const uint8_t b, const uint8_t y) {
  // Set DS3231 register pointer to 0x00
  Wire.beginTransmission(rtcAddr);
  Wire.write(0);
  Wire.write(DEC2BCD(s)); // set seconds
  Wire.write(DEC2BCD(m)); // set minutes
  Wire.write(DEC2BCD(h)); // set hours
  Wire.write(DEC2BCD(w)); // set day of week (1=Sunday, 7=Saturday)
  Wire.write(DEC2BCD(d)); // set day (1 to 31)
  Wire.write(DEC2BCD(b)); // set month
  Wire.write(DEC2BCD(y)); // set year (0 to 99)
  return (Wire.endTransmission() == 0);
}

