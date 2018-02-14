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
#include "fntcpm.h"
#include "fnthel.h"
#include "fntltw.h"
#include "fntncs.h"
#include "fntrct.h"
#include "fntskd.h"
#include "fntsqr.h"
#include "fntstd.h"

const uint64_t* const FONTS[] = {FNTBBT, FNTBLD, FNTCLN, FNTCPM,
                                 FNTHEL, FNTLTW, FNTNCS, FNTRCT,
                                 FNTSKD, FNTSQR, FNTSTD,
                                };
uint8_t fontSize  = 10;
uint8_t fontCount = 11;

#endif /* FONTS_H */
