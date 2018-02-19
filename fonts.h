/**
  fonts.h - Custom LED matrix fonts

  Copyright (c) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrixClock.
*/

#ifndef FONTS_H
#define FONTS_H

// Load the font definitions
#include "fntbbt.h"
#include "fntbld.h"
#include "fntcln.h"
#include "fnthel.h"
#include "fntltw.h"
#include "fntncs.h"
#include "fntskd.h"
#include "fntsqr.h"
#include "fntstd.h"

const uint64_t* const FONTS[] = {FNTSTD, FNTBBT, FNTSKD,
                                 FNTNCS, FNTBLD, FNTLTW,
                                 FNTSQR, FNTHEL, FNTCLN,
                                };
uint8_t fontSize  = 16;
uint8_t fontCount =  9;
uint8_t fontWidth =  5;

#endif /* FONTS_H */
