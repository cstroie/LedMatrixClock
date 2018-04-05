/**
  LedMatrixClock - A desk clock using 4 8x8 led matrix modules

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

// EEPROM
#include <EEPROM.h>
// Watchdog, sleep
#include <avr/wdt.h>
#include <avr/sleep.h>

#include "DotMatrix.h"
#include "DS3231.h"

// Software name and vesion
const char DEVNAME[] PROGMEM = "LedMatrix Clock";
const char VERSION[] PROGMEM = "2.5";

// Pin definitions
const int CS_PIN    = 10; // ~SS
const int BEEP_PIN  = 2;

// The RTC
DS3231 rtc;
uint32_t  mtxDisplayWait  = 1000UL;                             // Wait interval
uint32_t  mtxDisplayUntil = 0UL;                                // Display check until
bool      mtxDisplayNow   = true;                               // Force display

// The matrix object
#define MATRICES  4
#define SCANLIMIT 8
DotMatrix mtx = DotMatrix();

// Automatic brightness steps
uint32_t brgtDelayCheck  = 100UL;
uint32_t brgtLastCheck   = 0UL;

// Display modes
enum      mtxModes {MODE_HHMM, MODE_SS, MODE_DDMM, MODE_YY,
                    MODE_TEMP, MODE_VCC, MODE_MCU, MODE_ALL
                   };
uint8_t   mtxMode       = MODE_HHMM;                            // Initial mode
uint32_t  mtxModeUntil  = 0UL;                                  // Expiration time, 0 is never
uint32_t  mtxModeWait   = 10000UL;                              // Expiration interval

// Define the configuration type
struct cfgEE_t {
  union {
    struct {
      uint8_t font: 4;  // Display font
      uint8_t brgt: 4;  // Display brightness (manual)
      uint8_t mnbr: 4;  // Minimal display brightness (auto)
      uint8_t mxbr: 4;  // Minimum display brightness (auto)
      bool    aubr: 1;  // Manual/Auto display brightness adjustment
      bool    tmpu: 1;  // Temperature units
      uint8_t spkm: 2;  // Speaker mode
      uint8_t spkl: 2;  // Speaker volume level
      uint8_t echo: 1;  // Local echo
      bool    dst:  1;  // DST flag
    };
    uint8_t data[4];    // We use 4 bytes in the structure
  };
  uint8_t crc8;         // CRC8
};
// The global configuration structure
struct cfgEE_t  cfgData;
// EEPROM address to store the configuration to
uint16_t        cfgEEAddress = 0x0180;


/**
  CRC8 computing

  @param inCrc partial CRC
  @param inData new data
  @return updated CRC
*/
uint8_t CRC8(uint8_t inCrc, uint8_t inData) {
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
  for (uint8_t i = 0; i < sizeof(cfg) - 1; i++)
    crc8 = CRC8(crc8, cfg.data[i]);
  return crc8;
}

/**
  Compare two configuration structures

  @param cfg1 the first configuration structure
  @param cfg1 the second configuration structure
  @return true if equal
*/
bool cfgEqual(struct cfgEE_t cfg1, struct cfgEE_t cfg2) {
  bool result = true;
  // Check CRC first
  if (cfg1.crc8 != cfg2.crc8)
    result = false;
  else
    // Compare the overlayed array of the two structures
    for (uint8_t i = 0; i < sizeof(cfg1) - 1; i++)
      if (cfg1.data[i] != cfg2.data[i])
        result = false;
  return result;
}

/**
  Write the configuration to EEPROM, along with CRC8, if different
*/
bool cfgWriteEE() {
  // Temporary configuration structure
  struct cfgEE_t cfgTemp;
  // Read the data from EEPROM into the temporary structure
  EEPROM.get(cfgEEAddress, cfgTemp);
  // Compute the CRC8 checksum of the read data
  uint8_t crc8 = cfgEECRC(cfgTemp);
  // Compute the CRC8 checksum of the actual data
  cfgData.crc8 = cfgEECRC(cfgData);
  // Compare the new and the stored data and check if the stored data is valid
  if (not cfgEqual(cfgData, cfgTemp) or (cfgTemp.crc8 != crc8))
    // Write the data
    EEPROM.put(cfgEEAddress, cfgData);
  // Always return true, even if data is not written
  return true;
}

/**
  Read the configuration from EEPROM, along with CRC8, and verify
*/
bool cfgReadEE(bool useDefaults = false) {
  // Temporary configuration structure
  struct cfgEE_t cfgTemp;
  // Read the data from EEPROM into the temporary structure
  EEPROM.get(cfgEEAddress, cfgTemp);
  // Compute the CRC8 checksum of the read data
  uint8_t crc8 = cfgEECRC(cfgTemp);
  // And compare with the read crc8 checksum
  if      (cfgTemp.crc8 == crc8)  cfgData = cfgTemp;
  else if (useDefaults)           cfgDefaults();
  return (cfgTemp.crc8 == crc8);
}

/**
  Reset the configuration to factory defaults
*/
bool cfgDefaults() {
  cfgData.font = 0x01;
  cfgData.brgt = 0x01;
  cfgData.mnbr = 0x00;
  cfgData.mxbr = 0x0F;
  cfgData.aubr = 0x01;
  cfgData.tmpu = 0x01;
  cfgData.spkm = 0x00;
  cfgData.spkl = 0x00;
  cfgData.echo = 0x01;
  cfgData.dst  = 0x00;
  return true;
};

/**
  Compute the brightness
*/
uint8_t brightness() {
  static uint16_t lstLight = analogRead(A0);
  if (cfgData.aubr) {
    // Automatic, read the LDR connected to A0
    uint16_t nowLight = analogRead(A0);
    // Exponential smooth 12.5%
    lstLight = ((lstLight << 3) - lstLight + nowLight + 4) >> 3;
    // Map in min..max range
    return map(lstLight, 0, 1023, cfgData.mnbr, cfgData.mxbr);
  }
  else
    // Manual
    return cfgData.brgt;
}

/**
  Short BEEP

  @param duration beep duration in ms
*/
void beep(uint16_t duration = 20) {
  pinMode(BEEP_PIN, OUTPUT);
  digitalWrite(BEEP_PIN, HIGH);
  delay(duration);
  digitalWrite(BEEP_PIN, LOW);
}

/**
  Show the time specfied in unpacked BCD (4 bytes)
*/
void showTimeBCD(uint8_t* HHMM) {
  // Create a new array, containing the colon symbol
  uint8_t HH_MM[] = {HHMM[0], HHMM[1], 0x0A, HHMM[2], HHMM[3]};

  // Digits positions and count
  uint8_t pos[] = {23, 17, 13, 9, 3};
  uint8_t posCount = sizeof(pos) / sizeof(*pos);

  // Clear the framebuffer
  mtx.fbClear();

  // Print on framebuffer
  for (uint8_t d = 0; d < posCount; d++)
    mtx.fbPrint(pos[d], HH_MM[d]);

  // Display the framebuffer
  mtx.fbDisplay();
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
  Check and display mode HHMM
*/
void showModeHHMM() {
  if (rtc.rtcOk) {
    // Check the alarms, the Alarm 2 triggers once per minute
    if ((rtc.checkAlarms() & 0x02) or mtxDisplayNow) {
      // Read the RTC, in BCD format
      bool newHour = rtc.readTimeBCD();
      // Check DST adjustments each new hour and read RTC if adjusted
      if (newHour)
        if (checkDST())
          rtc.readTimeBCD();
      // Show the time
      showTimeBCD(rtc.R);
      // Beep each hour, on the dot
      if (newHour and (cfgData.spkm & 0x01)) beep();
    }
  }
}

/**
  Display the mode TEMP
*/
void showModeTEMP() {
  // Get the temperature
  int8_t temp = rtc.readTemperature(cfgData.tmpu);

  // Create a new array, containing the degree symbol and units letter
  uint8_t data[] = {0x0B, abs(temp) / 10, abs(temp) % 10, 0x0D, cfgData.tmpu ? 0x0C : 0x0F};

  // Digits positions and count
  uint8_t pos[] = {27, 21, 15, 9, 3};
  uint8_t posCount = sizeof(pos) / sizeof(*pos);

  // Clear the framebuffer
  mtx.fbClear();

  // Print into the framebuffer
  for (uint8_t d = temp < 0 ? 0 : 1; d < posCount; d++)
    mtx.fbPrint(pos[d], data[d]);

  // Display the framebuffer
  mtx.fbDisplay();
}

/**
  Set the display mode

  @param mode the chosen mode
*/
void mtxSetMode(uint8_t mode) {
  mtxMode = mode % MODE_ALL;
  if (mtxMode <= MODE_SS) mtxModeUntil =  0UL;                    // Never expire for HHMM and SS
  else                    mtxModeUntil = millis() + mtxModeWait;  // Expire after a while
  // Force display
  mtxDisplayNow = true;
}

/**
  AT-Hayes style command processing
*/
void handleHayes() {
  char buf[35] = "";
  int8_t len = -1;
  bool result = false;
  if (Serial.find("AT")) {
    // Read until newline, no more than 4 chararcters
    len = Serial.readBytesUntil('\r', buf, 4);
    buf[len] = '\0';
    // Local terminal echo
    if (cfgData.echo) {
      Serial.print(F("AT")); Serial.println(buf);
    }
    // Check the first character, could be a symbol or a letter
    switch (buf[0]) {
      case '*': // Our extension, one letter and 1..2 digits (0-99), or '?', '='
        switch (buf[1]) {
          case 'A': // Auto brightness switch
            if (len == 2) {
              cfgData.aubr = 0x00;
              mtx.intensity(brightness());
              result = true;
            }
            else if (buf[2] >= '0' and buf[2] <= '1') {
              // Set auto brightness
              cfgData.aubr = (buf[2] - '0') & 0x01;
              mtx.intensity(brightness());
              result = true;
            }
            else if (buf[2] == '?') {
              // Get brightness
              Serial.print(F("*A: ")); Serial.println(cfgData.aubr);
              result = true;
            }
            break;
          case 'B': // Brightness
            if (buf[2] >= '0' and buf[2] <= '9') {
              // Set brightness
              uint8_t brgt = buf[2] - '0';
              // Read one more char
              if (buf[3] >= '0' and buf[3] <= '9')
                brgt = (brgt * 10 + (buf[3] - '0')) % 0x10;
              cfgData.aubr = false;
              cfgData.brgt = brgt;
              // Set the brightness
              mtx.intensity(brightness());
              result = true;
            }
            else if (buf[2] == '?') {
              // Get brightness
              Serial.print(F("*B: ")); Serial.println(cfgData.brgt);
              result = true;
            }
            break;
          case 'D': // DST switch
            if (len == 2) {
              cfgData.dst = 0x00;
              mtxDisplayNow = true;
              result = true;
            }
            else if (buf[2] >= '0' and buf[2] <= '1') {
              // Set DST
              cfgData.dst = (buf[2] - '0') & 0x01;
              mtxDisplayNow = true;
              result = true;
            }
            else if (buf[2] == '?') {
              // Get DST
              Serial.print(F("*D: ")); Serial.println(cfgData.dst);
              result = true;
            }
            break;
          case 'F': // Font
            if (buf[2] >= '0' and buf[2] <= '9') {
              // Set font
              uint8_t font = buf[2] - '0';
              // Read one more char
              if (buf[3] >= '0' and buf[3] <= '9')
                font = (font * 10 + (buf[3] - '0')) % fontCount;
              cfgData.font = font;
              mtx.loadFont(cfgData.font);
              result = true;
            }
            else if (buf[2] == '?') {
              // Get font
              Serial.print(F("*F: ")); Serial.println(cfgData.font);
              result = true;
            }
            break;
          case 'H': // Highest (maximum) auto brightness level
            if (len == 2) {
              cfgData.mxbr = 0x0F;
              mtx.intensity(brightness());
              result = true;
            }
            else if (buf[2] >= '0' and buf[2] <= '9') {
              uint8_t value = buf[2] - '0';
              // Read one more char
              if (buf[3] >= '0' and buf[3] <= '9')
                value = (value * 10 + (buf[3] - '0')) & 0x0F;
              cfgData.mxbr = value;
              mtx.intensity(brightness());
              result = true;
            }
            else if (buf[2] == '?') {
              // Get font
              Serial.print(F("*H: ")); Serial.println(cfgData.mxbr);
              result = true;
            }
            break;
          case 'L': // Lowest (minimum) auto brightness level
            if (len == 2) {
              cfgData.mnbr = 0x00;
              mtx.intensity(brightness());
              result = true;
            }
            else if (buf[2] >= '0' and buf[2] <= '9') {
              uint8_t value = buf[2] - '0';
              // Read one more char
              if (buf[3] >= '0' and buf[3] <= '9')
                value = (value * 10 + (buf[3] - '0')) & 0x0F;
              cfgData.mnbr = value;
              mtx.intensity(brightness());
              result = true;
            }
            else if (buf[2] == '?') {
              // Get font
              Serial.print(F("*L: ")); Serial.println(cfgData.mnbr);
              result = true;
            }
            break;
          case 'M': // MCU temperature
            if (len == 2) {
              Serial.print((float)readMCUTemp() / 100.0, 2); Serial.println(F("C"));
              result = true;
            }
            break;
          case 'T': // Time and date setting and query
            //  Usage: AT*T="YYYY/MM/DD HH:MM:SS" or, using date(1),
            //  ( sleep 2 && date "+AT\*T=\"%Y/%m/%d %H:%M:%S\"" ) > /dev/ttyUSB0
            if (buf[2] == '=' and buf[3] == '"') {
              // Set time and date
              uint16_t  year  = Serial.parseInt();
              uint8_t   month = Serial.parseInt();
              uint8_t   day   = Serial.parseInt();
              uint8_t   hour  = Serial.parseInt();
              uint8_t   min   = Serial.parseInt();
              uint8_t   sec   = Serial.parseInt();
              if (year != 0 and month != 0 and day != 0) {
                // The date is quite valid, set the clock to 00:00:00 if not
                // specified
                rtc.writeDateTime(sec, min, hour, day, month, year);
                // Check if DST and set the flag
                cfgData.dst = rtc.dstCheck(year, month, day, hour);
                // Store the configuration
                cfgWriteEE();
                // TODO Check
                result = true;
              }
            }
            else if (buf[2] == '?') {
              // Get time and date
              rtc.readTime(true);
              Serial.print(rtc.Y); Serial.print(F("/"));
              Serial.print(rtc.m); Serial.print(F("/"));
              Serial.print(rtc.d); Serial.print(F(" "));
              Serial.print(rtc.H); Serial.print(F(":"));
              Serial.print(rtc.M); Serial.print(F(":"));
              Serial.print(rtc.S); Serial.println();
              result = true;
            }
            break;
          case 'U': // Temperature units and query
            if (buf[2] >= '0' and buf[2] <= '1') {
              // Set temperature units
              cfgData.tmpu = buf[2] == '1' ? true : false;
              result = true;
            }
            else if (buf[2] == 'C') {
              // Set temperature units
              cfgData.tmpu = true;
              result = true;
            }
            else if (buf[2] == 'F') {
              // Set temperature units
              cfgData.tmpu = false;
              result = true;
            }
            else if (buf[2] == '?') {
              // Get temperature units
              Serial.print(F("*U: ")); Serial.println(cfgData.tmpu ? "C" : "F");
              result = true;
            }
            else if (len == 2) {
              // Show the temperature
              Serial.print((int)rtc.readTemperature(cfgData.tmpu));
              Serial.println(cfgData.tmpu ? "C" : "F");
              result = true;
            }
            break;
          case 'V': // Vcc
            if (len == 2) {
              Serial.print((float)readVcc() / 1000.0, 3); Serial.println(F("V"));
              result = true;
            }
            break;
        }
        break;
      case '&': // Standard '&' extension
        switch (buf[1]) {
          case 'F': // Factory defaults
            result = cfgDefaults();
            break;
          case 'V': // Show the configuration
            Serial.print(F("*A: ")); Serial.print(cfgData.aubr); Serial.print(F("; "));
            Serial.print(F("*B: ")); Serial.print(cfgData.brgt); Serial.print(F("; "));
            Serial.print(F("*L: ")); Serial.print(cfgData.mnbr); Serial.print(F("; "));
            Serial.print(F("*H: ")); Serial.print(cfgData.mxbr); Serial.println(F("; "));
            Serial.print(F("*F: ")); Serial.print(cfgData.font); Serial.print(F("; "));
            Serial.print(F("*D: ")); Serial.print(cfgData.dst);  Serial.print(F("; "));
            Serial.print(F("*U: ")); Serial.print(cfgData.tmpu ? "C" : "F"); Serial.println(F("; "));
            Serial.print(F("E: ")); Serial.print(cfgData.echo); Serial.print(F("; "));
            Serial.print(F("L: ")); Serial.print(cfgData.spkl); Serial.print(F("; "));
            Serial.print(F("M: ")); Serial.print(cfgData.spkm); Serial.println(F("; "));
            result = true;
            break;
          case 'W': // Store the configuration
            result = cfgWriteEE();
            break;
          case 'Y': // Read the configuration
            result = cfgReadEE();
            break;
        }
        break;
      case 'I': // ATI Show info
        showBanner();
        Serial.println(__DATE__);
        result = true;
        break;
      case 'E': // ATE Set local echo
        if (len == 1) {
          cfgData.echo = 0x00;
          result = true;
        }
        else if (buf[1] >= '0' and buf[1] <= '1') {
          // Set echo on or off
          cfgData.echo = (buf[1] - '0') & 0x01;
          result = true;
        }
        else if (buf[1] == '?') {
          // Get local echo
          Serial.print(F("E: ")); Serial.println(cfgData.echo);
          result = true;
        }
        break;
      case 'L': // ATL Set speaker volume level
        if (len == 1) {
          cfgData.spkl = 0x00;
          result = true;
        }
        else if (buf[1] >= '0' and buf[1] <= '3') {
          // Set speaker volume
          cfgData.spkl = (buf[1] - '0') & 0x03;
          result = true;
        }
        else if (buf[1] == '?') {
          // Get speaker volume level
          Serial.print(F("L: ")); Serial.println(cfgData.spkl);
          result = true;
        }
        break;
      case 'M': // ATM Speaker control
        if (len == 1) {
          cfgData.spkm = 0x00;
          result = true;
        }
        else if (buf[1] >= '0' and buf[1] <= '3') {
          // Set speaker on or off mode
          cfgData.spkm = (buf[1] - '0') & 0x03;
          result = true;
        }
        else if (buf[1] == '?') {
          // Get speaker mode
          Serial.print(F("M: ")); Serial.println(cfgData.spkm);
          result = true;
        }
        break;
      case 'Z': // ATZ Reset
        softReset(WDTO_2S);
        break;
      case '?':
        // Help messages
        Serial.println(F("AT?"));
        Serial.println(F("AT*Fn"));
        Serial.println(F("AT*Bn"));
        Serial.println(F("AT*Un"));
        Serial.println(F("AT*T=\"YYYY/MM/DD HH:MM:SS\""));
        Serial.println(F("AT&F"));
        Serial.println(F("AT&V"));
        Serial.println(F("AT&W"));
        Serial.println(F("AT&Y"));
        result = true;
        break;
      default:
        result = (len == 0);
    }

    // Last response line
    if (len >= 0) {
      if (result) Serial.println(F("OK"));
      else        Serial.println(F("ERROR"));

      // Force time display
      mtxDisplayNow = true;
      Serial.flush();
    }
  }
}

/**
  Check the DST adjustments
*/
bool checkDST() {
  int8_t dstAdj = rtc.dstSelfAdjust(cfgData.dst);
  if (dstAdj != 0) {
    // Do the adjustment, no need to re-read the RTC
    rtc.setHours(dstAdj, false);
    // Toggle the DST config bit
    cfgData.dst ^= 1;
    // Save the config
    cfgWriteEE();
    return true;
  }
  return false;
}

/**
  Print the banner to serial console
*/
void showBanner() {
  char buf[40] = "";
  strcpy_P(buf, DEVNAME);
  strcat_P(buf, PSTR(" "));
  strcat_P(buf, VERSION);
  Serial.println(buf);
}

/**
  Analog raw reading, after a delay, while sleeping, using interrupt

  @return raw analog read value (long)
*/
long readRaw() {
  // Set the registers
  ADCSRA |= _BV(ADPS0) | _BV(ADPS1) | _BV(ADPS2);   // prescaler of 128
  ADCSRA |= _BV(ADEN);                              // enable the ADC
  ADCSRA |= _BV(ADIE);                              // enable interrupt
  // Wait for voltage to settle
  delay(10);
  // Take an ADC reading in sleep mode
  noInterrupts();
  // Start conversion
  ADCSRA |= _BV(ADSC);
  set_sleep_mode(SLEEP_MODE_ADC);
  interrupts();
  // Awake again, reading should be done, but better make sure
  while (bit_is_set(ADCSRA, ADSC));
  // Reading register "ADCW" takes care of how to read ADCL and ADCH
  long wADC = ADCW;
  // The returned reading
  return wADC;
}

/**
  Read the analog pin after a delay, while sleeping, using interrupt

  @param pin the analog pin
  @return raw analog read value
*/
int readAnalog(uint8_t pin) {
  // Allow for channel or pin numbers
  if (pin >= 14) pin -= 14;

  // Set the analog reference to DEFAULT, select the channel (low 4 bits).
  // This also sets ADLAR (left-adjust result) to 0 (the default).
  ADMUX = _BV(REFS0) | (pin & 0x07);

  // Raw analog read
  long wADC = readRaw();

  // The returned reading
  return (int)(wADC);
}

/**
  Read the internal MCU temperature
  The internal temperature has to be used with the internal reference of 1.1V.
  Channel 8 can not be selected with the analogRead function yet.

  @return temperature in hundredths of degrees Celsius, *calibrated for my device*
*/
int readMCUTemp() {
  // Set the internal reference and mux.
  ADMUX = (_BV(REFS1) | _BV(REFS0) | _BV(MUX3));

  // Raw analog read
  long wADC = readRaw();

  // The returned temperature is in hundreds degrees Celsius; not calibrated
  return (int)(100 * wADC - 27315);
}

/*
  Read the power supply voltage, by measuring the internal 1V1 reference

  @return voltage in millivolts, uncalibrated
*/
int readVcc() {
  // Set the reference to Vcc and the measurement to the internal 1.1V reference
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);

  // Raw analog read
  long wADC = readRaw();

  // Return Vcc in mV; 1125300 = 1.1 * 1024 * 1000
  return (int)(1125300UL / wADC);
}

/**
  Software reset the MCU
  (c) Mircea Diaconescu http://web-engineering.info/node/29
*/
void softReset(uint8_t prescaller) {
  Serial.print(F("Reboot"));
  // Start watchdog with the provided prescaller
  wdt_enable(prescaller);
  // Wait for the prescaller time to expire
  // without sending the reset signal by using
  // the wdt_reset() method
  while (true) {
    Serial.print(F("."));
    delay(1000);
  }
}

/**
  Main Arduino setup function
*/
void setup() {
  // Init the serial com and print the banner
  Serial.begin(9600);
  showBanner();

  // Read the configuration from EEPROM or
  // use the defaults if CRC8 does not match
  cfgReadEE(true);

  // Init all led matrices
  mtx.init(CS_PIN, MATRICES, SCANLIMIT);
  // Do a display test for a second
  mtx.displaytest(true);
  delay(1000);
  mtx.displaytest(false);
  // Decode nothing
  mtx.decodemode(0);
  // Clear the display
  mtx.clear();
  // Set the brightness
  mtx.intensity(brightness());
  // Power on the matrices
  mtx.shutdown(false);

  // Load the font
  mtx.loadFont(cfgData.font);

  // Init and configure RTC
  if (! rtc.init()) {
    Serial.println(F("DS3231 RTC missing"));
  }

  if (rtc.rtcOk and rtc.lostPower()) {
    Serial.println(F("RTC lost power, lets set the time!"));
    // The next line sets the RTC to the date & time this sketch was compiled
    //rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    // rtc.adjust(DateTime(2014, 1, 21, 3, 0, 0));
  }

  if (rtc.rtcOk) {
    // Check DST adjustments
    checkDST();
    // Show the temperature
    showModeTEMP();
    delay(2000);
  }
}

/**
  Main Arduino loop
*/
void loop() {
  // Check any command on serial port
  if (Serial.available())
    handleHayes();

  // Automatic brightness check and adjustment
  if (cfgData.aubr) {
    if (millis() - brgtLastCheck > brgtDelayCheck) {
      brgtLastCheck = millis();
      mtx.intensity(brightness());
    }
  }

  // Display, check once in a while or force
  if ((millis() > mtxDisplayUntil) or mtxDisplayNow) {
    mtxDisplayUntil = millis() + mtxDisplayWait;
    switch (mtxMode) {
      case MODE_TEMP: // RTC temperature
        showModeTEMP();
        break;
      default:        // Hours and minutes
        showModeHHMM();
    }
    // Reset the forced show time flag
    mtxDisplayNow = false;
  }

  // Check if the display mode expired
  if (mtxModeUntil > 0 and millis() > mtxModeUntil) {
    // Return to default mode, never expiring
    mtxMode       = MODE_HHMM;
    mtxModeUntil  = 0;
  }
}
