#ifndef FONT_H_
#define FONT_H_

#include "DSP2833x_Device.h"

#define LCD9648_ASCII_FIRST_CHAR  32U
#define LCD9648_ASCII_LAST_CHAR   126U
#define LCD9648_ASCII_GLYPH_COUNT 95U

extern const Uint16 LCD9648_DigitFont[10][5];
extern const Uint16 LCD9648_DecimalPointFont[2];
extern const Uint16 LCD9648_AsciiFont[LCD9648_ASCII_GLYPH_COUNT][5];

#endif /* FONT_H_ */
