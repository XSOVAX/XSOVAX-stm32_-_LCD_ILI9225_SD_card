#ifndef FONT8X8_H
#define FONT8X8_H

#include <stdint.h>
#include "stm32f1xx.h"
#include "ILI9225.h"
#include "menu.h"

extern const uint16_t Font8x16[256][8];

void drawChar8x16(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color);
void drawString8x16(uint16_t x, uint16_t y, const char *str, uint16_t color, uint8_t inversion);

#endif