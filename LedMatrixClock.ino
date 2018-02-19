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

#include "DotMatrix.h"
#include "DS3231.h"

const char DEVNAME[] = "LedMatrix Clock";
const char VERSION[] = "2.0";

// Pin definitions
const int DIN_PIN = 11; // MOSI
const int CLK_PIN = 13; // SCK
const int CS_PIN  = 10; // ~SS

// Load the fonts
#include "fonts.h"

// The RTC
DS3231 rtc;

// The matrix object
#define MATRICES 4
DotMatrix mtx = DotMatrix();

// The currently selected font
uint8_t DIGIT[16][8] = {0};


// Define the configuration type
struct cfgEE_t {
  uint8_t font; // Display font
  uint8_t brgt; // Display brightness
  uint8_t tmpu; // Temperature units
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
  crc8 = crc_8(crc8, cfg.tmpu);
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
      (cfg1.tmpu == cfg2.tmpu) and
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
  uint8_t chrbuf[8] = {0};
  font %= fontCount;
  // Load each character into RAM
  for (int i = 0; i < fontSize; i++) {
    // Load into temporary buffer
    memcpy_P(&chrbuf, &FONTS[font][i], 8);
    // Rotate
    for (uint8_t j = 0; j < 8; j++) {
      for (uint8_t k = 0; k < 8; k++) {
        DIGIT[i][7 - k] <<= 1;
        DIGIT[i][7 - k] |= (chrbuf[j] & 0x01);
        chrbuf[j] >>= 1;
      }
    }
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

  // The framebuffer
  uint8_t fb[32] = {0};

  // Digits positions
  uint8_t pos[] = {23, 17, 9, 3};

  // Print into the framebuffer
  for (uint8_t d = 0; d < 4; d++) {
    for (uint8_t l = 0; l < 5; l++) {
      fb[pos[d] + l] |= DIGIT[HHMM[d]][l];
    }
  }

  // Print the colon
  for (uint8_t l = 0; l < 5; l++) {
    fb[13 + l] |= DIGIT[10][l];
  }

  // Display the framebuffer
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t data[4] = {0};
    for (uint8_t m = 0; m < mtx.matrices; m++)
      data[m] = fb[m * 8 + i];
    mtx.sendAllHWSPI(i + 1, data, 4);
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

*/
void command() {
  char buf[35] = "";
  int8_t len = -1;
  bool result = false;
  if (Serial.find("AT")) {
    len = Serial.readBytesUntil('\r', buf, 4);
    buf[len] = '\0';
    if (buf[0] == '*') {
      // One char and numeric value
      if (buf[1] == 'F') {
        // Font
        if (buf[2] >= '0' and buf[2] <= '9') {
          // Set font
          cfgData.font = buf[2] - '0';
          loadFont(cfgData.font);
          result = true;
        }
        else if (buf[2] == '?') {
          // Get font
          Serial.print(F("*F: "));
          Serial.println(cfgData.font);
          result = true;
        }
      }
      else if (buf[1] == 'B') {
        // Brightness
        if (buf[2] >= '0' and buf[2] <= '9') {
          // Set brightness
          uint8_t brgt = buf[2] - '0';
          // Read one more char
          if (buf[3] >= '0' and buf[3] <= '9')
            brgt = (brgt * 10 + (buf[3] - '0')) % 0x10;
          cfgData.brgt = brgt;
          // Set the brightness
          mtx.intensity(cfgData.brgt);
          result = true;
        }
        else if (buf[2] == '?') {
          // Get brightness
          Serial.print(F("*B: "));
          Serial.println(cfgData.brgt);
          result = true;
        }
      }
      else if (buf[1] == 'T') {
        // Temperature
        if (buf[2] >= '0' and buf[2] <= '1') {
          // Set temperature units
          cfgData.tmpu = buf[2] == '1' ? 'C' : 'F';
          result = true;
        }
        else if (buf[2] == '?') {
          // Get temperature units
          Serial.print(F("*T: "));
          Serial.println(cfgData.tmpu == 'C' ? "C" : "F");
          result = true;
        }
        else if (len == 2) {
          // Show the temperature
          Serial.print((int)rtc.readTemperature(cfgData.tmpu == 'C'));
          Serial.println(cfgData.tmpu == 'C' ? "C" : "F");
          result = true;
        }
      }
    }
    else if (buf[0] == '&') {
      switch (buf[1]) {
        case 'F':
          // Factory default
          result = true;
          break;
        case 'V':
          // Show the configuration
          Serial.print(F("*F: "));
          Serial.println(cfgData.font);
          Serial.print(F("*B: "));
          Serial.println(cfgData.brgt);
          Serial.print(F("*T: "));
          Serial.println(cfgData.tmpu == 'C' ? "C" : "F");
          result = true;
          break;
        case 'W':
          // Store the configuration
          cfgWriteEE();
          result = true;
          break;
        case 'Y':
          // Read the configuration
          cfgReadEE();
          result = true;
          break;
      }
    }
    else if (buf[0] == '$') {
      // One char and quoted string value
      if (buf[1] == 'T') {
        // Time and date
        //  Usage: AT$T="YYYY/MM/DD HH:MM:SS" or, using date(1),
        //  ( sleep 2 && date "+AT\$T=\"%Y/%m/%d %H:%M:%S\"" ) > /dev/ttyUSB0
        if (buf[2] == '=' and buf[3] == '"') {
          // Set time and date
          uint16_t  year  = Serial.parseInt();
          uint8_t   month = Serial.parseInt();
          uint8_t   day   = Serial.parseInt();
          uint8_t   hour  = Serial.parseInt();
          uint8_t   min   = Serial.parseInt();
          uint8_t   sec   = Serial.parseInt();
          if (year != 0 and month != 0 and day != 0) {
            rtc.writeDateTime(sec, min, hour, day, month, year);
            // TODO Check
            result = true;
          }
        }
        else if (buf[2] == '?') {
          // TODO Get time and date
          result = true;
        }
      }
    }
    else if (buf[0] == '?' and len == 1) {
      Serial.println(F("AT?"));
      Serial.println(F("AT*Fn"));
      Serial.println(F("AT*Bn"));
      Serial.println(F("AT$T=\"YYYY/MM/DD HH:MM:SS\""));
      result = true;
    }
    else if (buf[0] == 'I' and len == 1) {
      printInfo();
      Serial.println(__DATE__);
      result = true;
    }
    else if (len == 0)
      result = true;
  }

  if (len >= 0) {
    if (result)
      Serial.println(F("OK"));
    else
      Serial.println(F("ERROR"));

    // Force time display
    mtxShowTime = true;
    Serial.flush();
  }
}

void printInfo() {
  Serial.print(DEVNAME);
  Serial.print(" ");
  Serial.println(VERSION);
}

/**
  Main Arduino setup function
*/
void setup() {
  // Init the serial com
  Serial.begin(9600);
  printInfo();

  // Read the configuration from EEPROM
  if (not cfgReadEE()) {
    // Invalid data, use some defaults
    cfgData.font = 1;
    cfgData.brgt = 1;
    cfgData.tmpu = 'C';
  }

  // Init all led matrices
  //mtx.init(DIN_PIN, CLK_PIN, CS_PIN, MATRICES);
  mtx.init(CS_PIN, MATRICES);
  mtx.displaytest(false);
  mtx.clear();
  mtx.intensity(cfgData.brgt);
  mtx.shutdown(false);

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
}

/**
  Main Arduino loop
*/
void loop() {
  //rtc.readTime();
  //showTime(rtc.HH, rtc.MM);

  if (Serial.available()) {
    command();
  }

  // Check the alarms, the Alarm 2 triggers once per minute
  if ((rtc.checkAlarms() & 0x02) or mtxShowTime) {
    rtc.readTimeBCD();
    showTimeBCD(rtc.R, mtxShowTime);
    mtxShowTime = false;
  }
}
