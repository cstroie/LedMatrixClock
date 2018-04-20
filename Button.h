/**
	Button - A tiny library to make reading buttons very simple.
	It handles debouncing automatically, and monitoring of state.

  Copyright (C) 2016 Michael D K Adams <http://www.michael.net.nz>
  Copyright (C) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  Released under the MIT license
*/

#ifndef BUTTON_H
#define BUTTON_H

#include <Arduino.h>

class Button {
  public:
    Button(uint8_t pin, uint16_t dly = 50);
    void begin();
    bool read();
    bool toggled();
    bool pressed();
    bool released();

    const static bool PRESSED   = LOW;
    const static bool RELEASED  = HIGH;

  private:
    bool      has_changed();

    uint8_t   _pin;
    uint16_t  _delay;
    bool      _state;
    bool      _has_changed;
    uint32_t  _ignore_until;
};

#endif /* BUTTON_H */
