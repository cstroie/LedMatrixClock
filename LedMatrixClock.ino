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
#include <Wire.h>
#include <RTClib.h>

// Pin definitions
const int DIN_PIN = 11;
const int CS_PIN  = 13;
const int CLK_PIN = 12;

// Define the digits
//#include "fntrct.h"
//#include "fntsqr.h"
//#include "fntstd.h"
#include "fntbld.h"

// The RTC
RTC_DS3231 rtc;
//RTC_Millis rtc;


// The matrix object
LedControl mtx = LedControl(DIN_PIN, CLK_PIN, CS_PIN, 3);

uint8_t mtxBrightness = 2;

/**
  Show the time

  @param hh hours   0..23
  @param mm minutes 0..59
*/
void mtxShowTime(uint8_t hh, uint8_t mm) {
  // Decompose to 4 digits
  uint8_t tm[4];
  tm[0] = hh / 10; tm[1] = hh % 10;
  tm[2] = mm / 10; tm[3] = mm % 10;

  // Create a RAM copy of the font (only the used digits)
  uint8_t digit[4][8];
  for (int i = 0; i < 4; i++)
    memcpy_P(&digit[i], &IMAGES[tm[i]], 8);

  // Display the columns (the matrices are transposed)
  for (int i = 0; i < 8; i++) {
    byte row;
    // First matrix
    row = digit[0][i] >> 3 | digit[1][i] << 3;
    mtx.setColumn(0, i, row);
    // Second matrix
    row = digit[1][i] >> 5 | digit[2][i] << 2;
    mtx.setColumn(1, i, row);
    // Third matrix
    row = digit[2][i] >> 6 | digit[3][i];
    mtx.setColumn(2, i, row);
  }
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

  // Init and configure RTC
  if (! rtc.begin()) {
    Serial.println(F("Couldn't find RTC"));
  }

  if (rtc.lostPower()) {
    Serial.println(F("RTC lost power, lets set the time!"));
    // The next line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }
}


void loop() {
  DateTime now = rtc.now();
  Serial.println(now.unixtime());
  mtxShowTime(now.hour(), now.minute());
}
