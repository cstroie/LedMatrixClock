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

// The currently selected font
uint8_t DIGIT[16][8] = {0};


// Define the configuration type
struct cfgEE_t {
  uint8_t font; // Display font
  uint8_t brgt; // Display brightness
  uint8_t crc8; // CRC8
};
// The global configuration structure
struct cfgEE_t  cfgData;
// EEPROM address to store the configuration to
uint16_t        cfgEEAddress = 0x0180;

// Show time independently of minutes changing
bool mtxShowTime = true;


/**
  CRC8 computing

  @param inCrc partial CRC
  @param inData new data
  @return updated CRC
*/
uint8_t crc_8(uint8_t inCrc, uint8_t inData) {
  uint8_t outCrc = inCrc ^ inData;
  for (uint8_t i = 0; i < 8; ++i) {
    if ((outCrc & 0x80 ) != 0) {
      outCrc <<= 1;
      outCrc ^= 0x07;
    }
    else {
      outCrc <<= 1;
    }
  }
  return outCrc;
}

/**
  Compute the CRC8 of the configuration structure

  @param cfgTemp the configuration structure
  @return computed CRC8
*/
uint8_t cfgEECRC(struct cfgEE_t cfg) {
  // Compute the CRC8 checksum of the data
  uint8_t crc8 = 0;
  crc8 = crc_8(crc8, cfg.font);
  crc8 = crc_8(crc8, cfg.brgt);
  return crc8;
}

/**
  Compare two configuration structures

  @param cfg1 the first configuration structure
  @param cfg1 the second configuration structure
  @return true if equal
*/
bool cfgCompare(struct cfgEE_t cfg1, struct cfgEE_t cfg2) {
  // Compute the CRC8 checksum of the data
  if ((cfg1.font == cfg2.font) and
      (cfg1.brgt == cfg2.brgt) and
      (cfg1.crc8 == cfg2.crc8))
    return true;
  else
    return false;
}

/**
  Write the configuration to EEPROM, along with CRC8, if different
*/
void cfgWriteEE() {
  // Temporary configuration structure
  struct cfgEE_t cfgTemp;
  // Read the data from EEPROM
  EEPROM.get(cfgEEAddress, cfgTemp);
  // Compute the CRC8 checksum of the read data
  cfgData.crc8 = cfgEECRC(cfgData);
  // Compare the new and the stored data
  if (not cfgCompare(cfgData, cfgTemp))
    // Write the data
    EEPROM.put(cfgEEAddress, cfgData);
}

/**
  Read the configuration from EEPROM, along with CRC8, and verify
*/
bool cfgReadEE() {
  // Read the data from EEPROM
  EEPROM.get(cfgEEAddress, cfgData);
  // Compute the CRC8 checksum of the read data
  uint8_t crc8 = cfgEECRC(cfgData);
  return (cfgData.crc8 == crc8);
}

/**
  Load the specified font into RAM

  @param font the font id
*/
void loadFont(uint8_t font) {
  uint8_t tmp[8];
  // Load each character into RAM
  for (int i = 0; i < fontSize; i++) {
    //memcpy_P(&tmp, &FONTS[font][i], 8);
    memcpy_P(&DIGIT[i], &FONTS[font][i], 8);
    /*
        for (uint8_t j = 0; j < 8; j++) {
          uint8_t x = tmp[j];
          for (uint8_t k = 8; k > 0; --k) {
            DIGIT[i][k] <<= 1;
            DIGIT[i][k] |= (tmp[j] & 0x01);
            tmp[j] >>= 1;
          }
        }
    */
  }
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

  Usage: "SET TIME YYYY/MM/DD HH:MM:SS" or, using date(1),
  ( sleep 2 && date "+SET TIME %Y/%m/%d %H:%M:%S" ) > /dev/ttyUSB0
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
        font %= fontCount;
        loadFont(font);
        cfgData.font = font;
        cfgWriteEE();
        Serial.print(F("Setting font to ")); Serial.println(font);
      }
      else if (strcmp(buf, "BRGHT") == 0) {
        uint8_t   brgt = Serial.parseInt();
        brgt &= 0x0F;
        for (int address = 0; address < mtx.getDeviceCount(); address++)
          // Set the brightness
          mtx.setIntensity(address, brgt);
        cfgData.brgt = brgt;
        cfgWriteEE();
        Serial.print(F("Setting brightness to ")); Serial.println(brgt);
      }
    }
    mtxShowTime = true;
    Serial.flush();
  }
  else
    Serial.println(F("Usage: SET TIME YYYY/MM/DD HH:MM:SS"));
}

/**
  Main Arduino setup function
*/
void setup() {
  // Init the serial com
  Serial.begin(9600);

  // Read the configuration from EEPROM
  if (not cfgReadEE()) {
    // Invalid data, use some defaults
    cfgData.font = 8;
    cfgData.brgt = 1;
  }

  // Init all led matrices
  for (int address = 0; address < mtx.getDeviceCount(); address++) {
    // Set the brightness
    mtx.setIntensity(address, cfgData.brgt);
    // Clear the display
    mtx.clearDisplay(address);
    // The MAX72XX is in power-saving mode on startup
    mtx.shutdown(address, false);
  }

  // Load the font
  loadFont(cfgData.font);

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
