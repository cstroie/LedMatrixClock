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

// EEPROM
#include <EEPROM.h>

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

struct CONFIG {
  uint8_t font;
  uint8_t brightness;
  uint8_t crc;
};
CONFIG eeConfig;
uint16_t eeAddress = 400;

// The currently selected font
uint8_t DIGIT[16][8] = {0};
// Display brightness
uint8_t mtxBrightness = 1;

// Show time independently from minutes changing
bool mtxShowTime = true;

/**
  CRC8 computing

  @param crc partial CRC
  @param data new data
  @return updated CRC
*/
uint8_t crc8_update(uint8_t crc, uint8_t data) {
  uint8_t updated = crc ^ data;
  for (uint8_t i = 0; i < 8; ++i) {
    if ((updated & 0x80 ) != 0) {
      updated <<= 1;
      updated ^= 0x07;
    }
    else {
      updated <<= 1;
    }
  }
  return updated;
}

/**
  Write the configuration to EEPROM, along with CRC8

  @param tm the time value to store
*/
void eeConfigWrite() {
  // Temporary config structure
  CONFIG cfg;
  // Read the data from EEPROM
  EEPROM.get(eeAddress, cfg);
  // Compute CRC8 checksum
  uint8_t crc = 0;
  crc = crc8_update(crc, eeConfig.font);
  crc = crc8_update(crc, eeConfig.brightness);
  eeConfig.crc = crc;
  // Write the data
  EEPROM.put(eeAddress, eeConfig);
}

/**
  Read the configuration from EEPROM, along with CRC8 and verify
*/
bool eeConfigRead() {
  CONFIG cfg;
  // Read the data
  EEPROM.get(eeAddress, cfg);
  // Compute CRC8 checksum
  uint8_t crc = 0;
  crc = crc8_update(crc, cfg.font);
  crc = crc8_update(crc, cfg.brightness);
  // Verify
  if (cfg.crc == crc) {
    eeConfig.font = cfg.font;
    eeConfig.brightness = cfg.brightness;
    eeConfig.crc = cfg.crc;
    return true;
  }
  else {
    eeConfig.font = 8;
    eeConfig.brightness = 1;
    return false;
  }
}

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
  Show the time specfied in unpacked BCD (4 bytes)
*/
void showTimeBCD(uint8_t* HHMM, bool force = false) {
  // Keep previous values for hours and minutes
  static uint8_t _hh = 255, _mm = 255;

  // Skip if not changed
  if (not force and (HHMM[1] == _hh) and (HHMM[3] == _mm)) return;

  // Display the columns (the matrices are transposed)
  for (int i = 0; i < 8; i++) {
    uint8_t row;
    // First matrix
    if ((HHMM[1] != _hh) or force) {
      row = DIGIT[HHMM[0]][i] >> 3 | DIGIT[HHMM[1]][i] << 3;
      mtx.setColumn(0, i, row);
    }
    // Second matrix
    if ((HHMM[1] != _hh) or (HHMM[3] != _mm) or force) {
      row = DIGIT[HHMM[1]][i] >> 5 | DIGIT[HHMM[2]][i] << 2;
      mtx.setColumn(1, i, row);
    }
    // Third matrix
    if ((HHMM[3] != _mm) or force) {
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
  Basic serial data parsing for setting time

  Usage: "SET: YYYY/MM/DD HH:MM:SS" or, using date(1),
  ( sleep 2 && date "+SET: %Y/%m/%d %H:%M:%S" ) > /dev/ttyUSB0
*/
void parseTime() {
  if (Serial.findUntil("SET ", "\r")) {
    char buf[16] = "";
    if (Serial.readBytesUntil(' ', buf, 16)) {
      if (strcmp(buf, "TIME") == 0) {
        uint16_t  year  = Serial.parseInt();
        uint8_t   month = Serial.parseInt();
        uint8_t   day   = Serial.parseInt();
        uint8_t   hour  = Serial.parseInt();
        uint8_t   min   = Serial.parseInt();
        uint8_t   sec   = Serial.parseInt();
        rtc.writeDateTime(sec, min, hour, day, month, year);
      }
      else if (strcmp(buf, "FONT") == 0) {
        uint8_t   font = Serial.parseInt();
        font %= 11;
        loadFont(font);
        eeConfig.font = font;
        eeConfigWrite();
        Serial.print(F("Setting font to ")); Serial.println(font);
      }
      else if (strcmp(buf, "BRGHT") == 0) {
        uint8_t   brght = Serial.parseInt();
        brght %= 16;
        for (int address = 0; address < mtx.getDeviceCount(); address++)
          // Set the brightness
          mtx.setIntensity(address, brght);
        eeConfig.brightness = brght;
        eeConfigWrite();
        Serial.print(F("Setting brightness to ")); Serial.println(brght);
      }
    }
    mtxShowTime = true;
    Serial.flush();
  }
  else
    Serial.println(F("Usage: SET: YYYY/MM/DD HH:MM:SS"));
}

/**
  Main Arduino setup function
*/
void setup() {
  // Init the serial com
  Serial.begin(9600);

  // Read the configuration from EEPROM
  eeConfigRead();

  // Init all led matrices
  for (int address = 0; address < mtx.getDeviceCount(); address++) {
    // Set the brightness
    mtx.setIntensity(address, eeConfig.brightness);
    // Clear the display
    mtx.clearDisplay(address);
    // The MAX72XX is in power-saving mode on startup
    mtx.shutdown(address, false);
  }

  // Load the font
  loadFont(eeConfig.font);

  // Init and configure RTC
  if (! rtc.init()) {
    Serial.println(F("DS3231 RTC missing"));
  }

  if (rtc.lostPower()) {
    Serial.println(F("RTC lost power, lets set the time!"));
    // The next line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  // Display the temperature
  Serial.print("T: ");
  Serial.println((int)rtc.readTemperature());
}

/**
  Main Arduino loop
*/
void loop() {
  //rtc.readTime();
  //showTime(rtc.HH, rtc.MM);

  if (Serial.available()) {
    parseTime();
  }

  // Check the alarms, the Alarm 2 triggers once per minute
  if ((rtc.checkAlarms() & 0x02) or mtxShowTime) {
    rtc.readTimeBCD();
    showTimeBCD(rtc.R, mtxShowTime);
    mtxShowTime = false;
  }
}
