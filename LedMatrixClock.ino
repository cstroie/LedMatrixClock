/**
  LedMatrixClock - A desk clock using 3 8x8 led matrix modules

  Copyright 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrix Clock.

  LedMatrixClock is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by the Free
  Software Foundation, either version 3 of the License, or (at your option) any
  later version.

  LedMatrixClock is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along with
  LedMatrixClock.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <LedControl.h>
#include "DS3231.h"

// Pin definitions
const int DIN_PIN = 11;
const int CS_PIN  = 13;
const int CLK_PIN = 12;

// Load the fonts
#include "fonts.h"

// The RTC
DS3231 rtc;

// The matrix object
#define MATRICES 3
LedControl mtx = LedControl(DIN_PIN, CLK_PIN, CS_PIN, MATRICES);

// The currently selected font
uint8_t DIGIT[16][8] = {0};
// Display brightness
uint8_t mtxBrightness = 1;

/**
  Load the specified font into RAM

  @param font the font id
*/
void loadFont(uint8_t font) {
  /*
    uint8_t *buf = new uint8_t[16];
    if (buf) {
        memcpy_P(buf, FONTS[font], len_xyz);
       }
    uint8_t *p = &FONTS[font];
  */
  //int fontSize = sizeof(FONTS[font]) / 8;
  for (int i = 0; i < fontSize; i++)
    memcpy_P(&DIGIT[i], &FONTS[font][i], 8);
}

/**
  Get the day of the week using the Tomohiko Sakamoto's method

  @param y year  >1752
  @param m month 1..12
  @param d day   1..31
  @return day of the week, 0..6 (Sun..Sat)
*/
uint8_t getDOW(uint16_t y, uint8_t m, uint8_t d) {
  uint8_t t[] = {0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4};
  y -= m < 3;
  return (y + y / 4 - y / 100 + y / 400 + t[m - 1] + d) % 7;
}

/**
  Check if a specified date observes DST, according to
  the time changing rules in Europe:

    start: last Sunday in March
    end:   last Sunday in October

  @param year  year  >1752
  @param month month 1..12
  @param day   day   1..31
  @return bool DST yes or no
*/
bool isDST(uint16_t year, uint8_t month, uint8_t day) {
  // Get the last Sunday in March
  uint8_t dayBegin = 31 - getDOW(year, 3, 31);
  // Get the last Sunday on October
  uint8_t dayEnd = 31 - getDOW(year, 10, 31);
  // Compute DST
  return ((month > 3) and (month < 10)) or
         ((month == 3) and (day >= dayBegin)) or
         ((month == 10) and (day < dayEnd));
}


/**
  Show the time specfied in unpacked BCD (4 bytes)
*/
void showTimeBCD(uint8_t* HHMM) {
  // Keep previous values for hours and minutes
  static uint8_t _hh = 255, _mm = 255;

  // Skip if not changed
  if ((HHMM[1] == _hh) and (HHMM[3] == _mm)) return;

  // Display the columns (the matrices are transposed)
  for (int i = 0; i < 8; i++) {
    uint8_t row;
    // First matrix
    if (HHMM[1] != _hh) {
      row = DIGIT[HHMM[0]][i] >> 3 | DIGIT[HHMM[1]][i] << 3;
      mtx.setColumn(0, i, row);
    }
    // Second matrix
    if ((HHMM[1] != _hh) or (HHMM[3] != _mm)) {
      row = DIGIT[HHMM[1]][i] >> 5 | DIGIT[HHMM[2]][i] << 2;
      mtx.setColumn(1, i, row);
    }
    // Third matrix
    if (HHMM[3] != _mm) {
      row = DIGIT[HHMM[2]][i] >> 6 | DIGIT[HHMM[3]][i];
      mtx.setColumn(2, i, row);
    }
  }

  // Keep the current values
  _hh = HHMM[1];
  _mm = HHMM[3];
}

/**
  Show the time

  @param hh hours   0..23
  @param mm minutes 0..59
*/
void showTime(uint8_t hh, uint8_t mm) {
  // Convert hhmm to unpacked BCD, 4 digits
  uint8_t HHMM[4] = {hh / 10, hh % 10, mm / 10, mm % 10};

  // Display the time in unpacked BCD
  showTimeBCD(HHMM);
}

/**
  Main Arduino setup function
*/
void setup() {
  // Init the serial com
  Serial.begin(9600);

  // Init all led matrices in a loop
  for (int address = 0; address < mtx.getDeviceCount(); address++) {
    // The MAX72XX is in power-saving mode on startup
    mtx.shutdown(address, false);
    // Set the brightness
    mtx.setIntensity(address, mtxBrightness);
    // Clear the display
    mtx.clearDisplay(address);
  }
  // Load the font
  loadFont(8);

  // Init and configure RTC
  if (! rtc.init()) {
    Serial.println(F("DS3231 RTC missing"));
  }

  /*
    if (rtc.lostPower()) {
      Serial.println(F("RTC lost power, lets set the time!"));
      // The next line sets the RTC to the date & time this sketch was compiled
      //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
      // This line sets the RTC with an explicit date & time, for example to set
      // January 21, 2014 at 3am you would call:
      // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
    }
  */
}

/**
  Main Arduino loop
*/
void loop() {
  //rtc.readTime();
  //showTime(rtc.HH, rtc.MM);

  rtc.readTimeBCD();
  showTimeBCD(rtc.HHMM);
}
