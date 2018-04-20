/**
  Button - A tiny library to make reading buttons very simple.
  It handles debouncing automatically, and monitoring of state.

  Copyright (C) 2016 Michael D K Adams <http://www.michael.net.nz>
  Released under the MIT license
*/

#include "Button.h"
#include <Arduino.h>

Button::Button(uint8_t pin, uint16_t dly): _pin(pin), _delay(dly), _state(HIGH), _ignore_until(0), _has_changed(false) {
}


void Button::begin() {
  pinMode(_pin, INPUT_PULLUP);
}

/**
  Read the button

  @return the button state
*/
bool Button::read() {
  // Ignore any pin changes until after this delay
  if (_ignore_until > millis()) {
  }
  // Check if the pin has changed
  else if (digitalRead(_pin) != _state) {
    _ignore_until = millis() + _delay;
    _state = !_state;
    _has_changed = true;
  }
  return _state;
}

/**
  Check if the button has been toggled from on -> off, or vice versa

  @return true if toggled
*/
bool Button::toggled() {
  read();
  return has_changed();
}

/**
  Check if the button has been pressed

  @return true if pressed
*/
bool Button::pressed() {
  return (read() == PRESSED && has_changed() == true);
}

/**
  Check if the button has been released

  @return true if released
*/
bool Button::released() {
  return (read() == RELEASED && has_changed() == true);
}

/**
  Check if the button state has changed after calling the read() function
  and reset the changed flag

  @return true if changed
*/
bool Button::has_changed() {
  if (_has_changed == true) {
    _has_changed = false;
    return true;
  }
  return false;
}
