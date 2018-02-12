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
  // Set century
  C = CENTURY * 100;
  return rtcOk;
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

  // Seconds
  S = Wire.read();
  S = bcd2bin(S);
  // Minutes
  M = Wire.read();
  M = bcd2bin(M);
  // Hours
  H = Wire.read();
  // Check if 12 hours and PM and add 0x12 (BCD)
  if ((H & (1 << 6)) and (H & (1 << 5)))
    H = (H & 0x1F) + 0x12;
  H = bcd2bin(H & 0x3F);
  I = H;
  P = H >= 12;
  if (P) I -= 12;
  // Date
  if (readDate) {
    // Century
    uint16_t c = C;
    // Day of week, 1 is Monday
    u = Wire.read();
    u = bcd2bin(u & 0x07);
    // Day
    d = Wire.read();
    d = bcd2bin(d);
    // Month and century
    m = Wire.read();
    if (m & (1 << 7)) c += 100;
    m = bcd2bin(m & 0x1F);
    // Year, short and long format
    y = Wire.read();
    y = bcd2bin(y);
    Y = c + y;
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
  R[2] = x / 16; (x & 0xF0) >> 4;
  R[3] = x % 16; (x & 0x0F);
  x = Wire.read() & 0x3F;
  R[0] = x / 16; (x & 0xF0) >> 4;
  R[1] = x % 16; (x & 0x0F);
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

