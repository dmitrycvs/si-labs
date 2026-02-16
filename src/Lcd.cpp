#include "Lcd.h"

static LiquidCrystal_I2C lcd(LCD_ADDRESS, LCD_COLUMNS, LCD_ROWS);

void LcdInit()
{
    lcd.init();
    lcd.backlight();
    lcd.clear();
}

void LcdClear()
{
    lcd.clear();
}

void LcdSetCursor(uint8_t col, uint8_t row)
{
    lcd.setCursor(col, row);
}

void LcdPrint(const char *text)
{
    lcd.print(text);
}

void LcdPrintAt(uint8_t col, uint8_t row, const char *text)
{
    lcd.setCursor(col, row);
    lcd.print(text);
}

void LcdBacklightOn()
{
    lcd.backlight();
}

void LcdBacklightOff()
{
    lcd.noBacklight();
}
