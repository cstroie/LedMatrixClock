/**
  fonts.h - Custom LED matrix fonts

  Copyright (c) 2017-2018 Costin STROIE <costinstroie@eridu.eu.org>

  This file is part of LedMatrixClock.
*/

#ifndef FONTS_H
#define FONTS_H

/*
  Beautiful Boot
  https://xantorohara.github.io/led-matrix-editor/#00708898a8c88870|0070202020203020|00f8102040808870|00708880402040f8|004040f848506040|00708880807808f8|0070888878081060|00101010204088f8|0070888870888870|00304080f0888870|0000202000202000|0000000070000000|0070880808088870|0000000060909060|0000000000000000|00080808780808f8
*/
const uint64_t FNTBBT[] PROGMEM = {
  0x00708898a8c88870,
  0x0070202020203020,
  0x00f8081060808870,
  0x00708880604080f8,
  0x004040f848506040,
  0x00708880780808f8,
  0x00708888780808f0,
  0x00101010204080f8,
  0x0070888870888870,
  0x00384080f0888870,
  0x0000002000200000,
  0x00000000f8000000,
  0x0070880808088870,
  0x0000000060909060,
  0x0000000000000000,
  0x00080808780808f8,
};

/*
  Bold 5x8 font
  https://xantorohara.github.io/led-matrix-editor/#70c8c8c8c8c8c870|f060606060607060|f8183060c0c0c870|70c8c0c060c0c870|c0c0c0c0f8c8c8c8|70c8c0c0781818f8|7098989878189870|3030303060c0c0f8|70c8c8c870c8c870|70c8c0f0c8c8c870|0000606000606000|0000000078000000|7098981818189870|0000000060b0b060|0000000000000000|18181818781818f8*/
const uint64_t FNTBLD[] PROGMEM = {
  0x70c8c8c8c8c8c870,
  0xf060606060607060,
  0xf8183060c0c0c870,
  0x70c8c0c060c0c870,
  0xc0c0c0c0f8c8c8c8,
  0x70c8c0c0781818f8,
  0x7098989878189870,
  0x3030303060c0c0f8,
  0x70c8c8c870c8c870,
  0x70c8c0f0c8c8c870,
  0x0000606000606000,
  0x0000000078000000,
  0x7098981818189870,
  0x0000000060b0b060,
  0x0000000000000000,
  0x18181818781818f8,
};

/*
  Helvetica font
  https://xantorohara.github.io/led-matrix-editor/#70d8d8d8d8d8d870|6060606060607860|f8183060c0c0d870|70d8c0c060c0d870|c0c0f8c8d0d0e0c0|70d8c8c0781818f8|70d8d8d87818d870|3030606060c0c0f8|70d8d8d870d8d870|70d8c0f0d8d8d870|0060600000606000|00000000f0000000|70d818181818d870|00000070d8d8d870|0000000000000000|18181818781818f8
*/
const uint64_t FNTHLV[] PROGMEM = {
  0x70d8d8d8d8d8d870,
  0x6060606060607860,
  0xf8183060c0c0d870,
  0x70d8c0c060c0d870,
  0xc0c0f8c8d0d0e0c0,
  0x70d8c8c0781818f8,
  0x70d8d8d87818d870,
  0x3030606060c0c0f8,
  0x70d8d8d870d8d870,
  0x70d8c0f0d8d8d870,
  0x0060600000606000,
  0x00000000f0000000,
  0x70d818181818d870,
  0x00000070d8d8d870,
  0x0000000000000000,
  0x18181818781818f8,
};

/*
  Lucida TypeWriter font
  https://xantorohara.github.io/led-matrix-editor/#0070d8d8d8d8d870|0060606060607860|00f8183060c0d870|0070d8c070c0d870|006060f868687060|0070d8c0781818f8|0070d8d87818d870|003030604080f8f8|0070d8d878d8d870|0070d8c0f0d8d870|0000202000202000|0000000078000000|00f038181818b8f0|0000000060b0b060|0000000000000000|00181818781818f8
*/
const uint64_t FNTLTW[] PROGMEM = {
  0x0070d8d8d8d8d870,
  0x0060606060607860,
  0x00f8183060c0d870,
  0x0070d8c070c0d870,
  0x006060f868687060,
  0x0070d8c0781818f8,
  0x0070d8d87818d870,
  0x003030604080f8f8,
  0x0070d8d878d8d870,
  0x0070d8c0f0d8d870,
  0x0000202000202000,
  0x0000000078000000,
  0x00f038181818b8f0,
  0x0000000060b0b060,
  0x0000000000000000,
  0x00181818781818f8,
};

/*
  New Century Schoolbook font
  https://xantorohara.github.io/led-matrix-editor/#70d8d8d8d8d8d870|7830303030303830|f8f81060c0d8d870|70d8d8c060d8d870|e0c0f8c8d0d0e0e0|70d8d8c0781878f8|70d8d8d87818d870|606060404080f8f8|70d8d8d870d8d870|70d8c0f0d8d8d870|0020200000202000|0000007878000000|7098981818989870|00000070d8d8d870|0000000000000000|3c181858785898fc
*/
const uint64_t FNTNCS[] PROGMEM = {
  0x70d8d8d8d8d8d870,
  0x7830303030303830,
  0xf8f81060c0d8d870,
  0x70d8d8c060d8d870,
  0xe0c0f8c8d0d0e0e0,
  0x70d8d8c0781878f8,
  0x70d8d8d87818d870,
  0x606060404080f8f8,
  0x70d8d8d870d8d870,
  0x70d8c0f0d8d8d870,
  0x0020200000202000,
  0x0000007878000000,
  0x7098981818989870,
  0x00000070d8d8d870,
  0x0000000000000000,
  0x3c181858785898fc,
};

/*
  Skoda font
  https://xantorohara.github.io/led-matrix-editor/#0070888888888870|0070202020202030|00f8080870808078|0078808070808078|004040f848506040|00788080780808f8|0070888878080870|00101020408080f8|0070888870888870|00708080f0888870
*/
const uint64_t FNTSKD[] PROGMEM = {
  0x0070888888888870, // 0
  0x0070202020202030, // 1
  0x00f8080870808078, // 2
  0x0078808070808078, // 3
  0x004040f848506040, // 4
  0x00788080780808f8, // 5
  0x0070888878080870, // 6
  0x00101020408080f8, // 7
  0x0070888870888870, // 8
  0x00708080f0888870, // 9
  0x0000202000202000, // :
  0x00000000e0000000, // -
  0x0070880808088870, // C
  0x0000000060909060, // '
  0x0000000000000000,
  0x00080808780808f8, // F
};

/*
  Rectangular 5x5 font
  https://xantorohara.github.io/led-matrix-editor/#00f8888888f80000|0070202020300000|00f808f880f80000|00f880f080f80000|008080f888880000|00f880f808f80000|00f888f808f80000|0080808088f80000|00f888f888f80000|00f880f888f80000|0000200020000000|0000007000000000|00f8080888f80000|000000e0a0e00000|0000000000000000|0008087808f80000
*/
const uint64_t FNTSQR[] PROGMEM = {
  0x00f8888888f80000,
  0x0070202020300000,
  0x00f808f880f80000,
  0x00f880f080f80000,
  0x008080f888880000,
  0x00f880f808f80000,
  0x00f888f808f80000,
  0x0080808088f80000,
  0x00f888f888f80000,
  0x00f880f888f80000,
  0x0000200020000000,
  0x0000007000000000,
  0x00f8080888f80000,
  0x000000e0a0e00000,
  0x0000000000000000,
  0x0008087808f80000,
};

/*
  Snapix
  https://xantorohara.github.io/led-matrix-editor/#00f8888888f80000|00f8202020380000|00f808f080f80000|00f880f080780000|0040f84848080000|0078807808f80000|0070887808700000|0020204080f80000|0070887088700000|007080f088700000|0000200020000000|0000007000000000|00f8080808f80000|0000000000e0a0e0|0000000000000000|0008087808f80000
*/
const uint64_t FNTSPX[] PROGMEM = {
  0x00f8888888f80000,
  0x00f8202020380000,
  0x00f808f080f80000,
  0x00f880f080780000,
  0x0040f84848080000,
  0x0078807808f80000,
  0x0070887808700000,
  0x0020204080f80000,
  0x0070887088700000,
  0x007080f088700000,
  0x0000200020000000,
  0x0000007000000000,
  0x00f8080808f80000,
  0x0000000000e0a0e0,
  0x0000000000000000,
  0x0008087808f80000,
};

/*
  Standard 5x7 font
  https://xantorohara.github.io/led-matrix-editor/#00708898a8c88870|0070202020203020|00f8102040808870|00708880402040f8|004040f848506040|00708880807808f8|0070888878081060|00101010204088f8|0070888870888870|00304080f0888870|0000202000202000|0000000070000000|0070880808088870|0000000060909060|0000000000000000|00080808780808f8
*/
const uint64_t FNTSTD[] PROGMEM = {
  0x00708898a8c88870,
  0x0070202020203020,
  0x00f8102040808870,
  0x00708880402040f8,
  0x004040f848506040,
  0x00708880807808f8,
  0x0070888878081060,
  0x00101010204088f8,
  0x0070888870888870,
  0x00304080f0888870,
  0x0000202000202000,
  0x0000000070000000,
  0x0070880808088870,
  0x0000000060909060,
  0x0000000000000000,
  0x00080808780808f8,
};

/*
  Nokia Bold 5x7 font
  https://xantorohara.github.io/led-matrix-editor/#0070d8d8d8d8d870|0030303030303830|00f8181870c0c078|0078c0c060c0c078|00c0c0f8c8d0e0c0|0078c0c0c0780878|0070d8d8d8781870|003030306060c0f8|0070d8d870d8d870|0070c0f0d8d8d870|0000303000303000|00000000f0000000|00f01818181818f0|0000000070d8d870|0000000000000000|00181818187818f8*/
const uint64_t FNTNOK[] PROGMEM = {
  0x0070d8d8d8d8d870,
  0x0030303030303830,
  0x00f8181870c0c078,
  0x0078c0c060c0c078,
  0x00c0c0f8c8d0e0c0,
  0x0078c0c0c0780878,
  0x0070d8d8d8781870,
  0x003030306060c0f8,
  0x0070d8d870d8d870,
  0x0070c0f0d8d8d870,
  0x0000303000303000,
  0x00000000f0000000,
  0x00f01818181818f0,
  0x0000000070d8d870,
  0x0000000000000000,
  0x00181818187818f8,
};

/*
  Medianoid
  https://xantorohara.github.io/led-matrix-editor/#00f888888888f800|0080808080808000|00f80808f880f800|00f88080f080f800|0040f84848487800|00f88080f808f800|00f88888f808f800|008080808080f800|00f88888f888f800|00808080f888f800|0000200020000000|0000007000000000|00f808080808f800|00000000e0a0e000|0000000000000000|000808087808f800
*/
const uint64_t FNTMDN[] PROGMEM = {
  0x00f888888888f800,
  0x0080808080808000,
  0x00f80808f880f800,
  0x00f88080f080f800,
  0x0040f84848487800,
  0x00f88080f808f800,
  0x00f88888f808f800,
  0x008080808080f800,
  0x00f88888f888f800,
  0x00808080f888f800,
  0x0000200020000000,
  0x0000007000000000,
  0x00f808080808f800,
  0x00000000e0a0e000,
  0x0000000000000000,
  0x000808087808f800,
};

/*
  TallOrder
  https://xantorohara.github.io/led-matrix-editor/#f8888888888888f8|f820202020203820|f808f880808080f8|f880e080808080f8|40f8480808080808|f880f808080808f8|f88888888888f808|80808080808080f8|f888f888888888f8|80f88888888888f8|0000200020000000|00000000f8000000|f8080808080808f8|00000000f09090f0|0000000000000000|08080808087808f8
*/
const uint64_t FNTTLO[] PROGMEM = {
  0xf8888888888888f8,
  0xf820202020203820,
  0xf808f880808080f8,
  0xf880e080808080f8,
  0x40f8480808080808,
  0xf880f808080808f8,
  0xf88888888888f808,
  0x80808080808080f8,
  0xf888f888888888f8,
  0x80f88888888888f8,
  0x0000200020000000,
  0x00000000f8000000,
  0xf8080808080808f8,
  0x00000000f09090f0,
  0x0000000000000000,
  0x08080808087808f8,
};

/*
  TallOrder
  https://xantorohara.github.io/led-matrix-editor/#3048888888889060|4040404050604040|6898102040808870|10284080604080f8|2020f84810102020|10284080780808f0|3048989868081060|101010202040c8b0|3048889060509060|10284090e8c89060|0020200020200000|00000030c0000000|3048888808089060|000000007090e000|0000000000000000|0808083808089060
*/
const uint64_t FNTHND[] PROGMEM = {
  0x3048888888889060,
  0x4040404050604040,
  0x6898102040808870,
  0x10284080604080f8,
  0x2020f84810102020,
  0x10284080780808f0,
  0x3048989868081060,
  0x101010202040c8b0,
  0x3048889060509060,
  0x10284090e8c89060,
  0x0020200020200000,
  0x00000030c0000000,
  0x3048888808089060,
  0x000000007090e000,
  0x0000000000000000,
  0x0808083808089060,
};

const uint64_t* const FONTS[] = {FNTSTD, FNTBBT, FNTSKD,
                                 FNTBLD, FNTNCS, FNTLTW,
                                 FNTHLV, FNTSQR, FNTSPX,
                                 FNTNOK, FNTMDN, FNTTLO,
                                 FNTHND
                                };
uint8_t fontSize  = sizeof(FNTSTD) / sizeof(*FNTSTD);
uint8_t fontCount = sizeof(FONTS) / sizeof(*FONTS);
uint8_t fontWidth = 5;

#endif /* FONTS_H */