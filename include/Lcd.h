#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

// LCD Configuration
const uint8_t LCD_ADDRESS = 0x27;
const uint8_t LCD_COLUMNS = 16;
const uint8_t LCD_ROWS = 2;

void LcdInit();
void LcdClear();
void LcdSetCursor(uint8_t col, uint8_t row);
void LcdPrint(const char *text);
void LcdPrintAt(uint8_t col, uint8_t row, const char *text);
void LcdBacklightOn();
void LcdBacklightOff();

#endif
