#ifndef FONT8X8_H
#define FONT8X8_H

#include <stdint.h>
#include "stm32f1xx.h"
#include "ILI9225.h"

// Стандартный ASCII 8x8 шрифт (32–127)
extern const uint8_t Font8x8[128][8];


void drawChar8x8(uint16_t x, uint16_t y, char c, uint16_t color, uint8_t inversion);
void drawString8x8(uint16_t x, uint16_t y, const char *str, uint16_t color, uint8_t inversion);

#endif