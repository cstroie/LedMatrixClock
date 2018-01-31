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
  Show the time

  @param hh hours   0..23
  @param mm minutes 0..59
*/
void showTime(uint8_t hh, uint8_t mm) {
  // Keep previous values for hours and minutes
  static uint8_t _hh = 255, _mm = 255;

  // Skip if not changed
  if ((hh == _hh) and (mm == _mm)) return;

  // Decompose hhmm to 4 digits
  uint8_t tm[4] = {hh / 10, hh % 10, mm / 10, mm % 10};

  // Display the columns (the matrices are transposed)
  for (int i = 0; i < 8; i++) {
    uint8_t row;
    // First matrix
    if (hh != _hh) {
      row = DIGIT[tm[0]][i] >> 3 | DIGIT[tm[1]][i] << 3;
      mtx.setColumn(0, i, row);
    }
    // Second matrix
    if ((hh != _hh) or (mm != _mm)) {
      row = DIGIT[tm[1]][i] >> 5 | DIGIT[tm[2]][i] << 2;
      mtx.setColumn(1, i, row);
    }
    // Third matrix
    if (mm != _mm) {
      row = DIGIT[tm[2]][i] >> 6 | DIGIT[tm[3]][i];
      mtx.setColumn(2, i, row);
    }
  }

  // Keep the current values
  _hh = hh;
  _mm = mm;
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
  rtc.readTime();
  showTime(rtc.HH, rtc.MM);
}
