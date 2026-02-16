#include <Arduino.h>
#include <stdio.h>
#include "Led.h"
#include "Lcd.h"
#include "KeypadWrapper.h"

// Pin Configuration
const uint8_t GREEN_LED_PIN = 12;
const uint8_t RED_LED_PIN = 13;
const uint32_t SERIAL_BAUD_RATE = 9600;

// Code Configuration
const char VALID_CODE[] = "1234";
const uint8_t MAX_CODE_LENGTH = 6;
const uint16_t DISPLAY_DELAY_MS = 2000;

// State Variables
char enteredCode[MAX_CODE_LENGTH + 1];
uint8_t codeIndex = 0;

// STDIO Stream Setup
FILE serialStream;

int serialPutChar(char c, FILE *stream)
{
  Serial.write(c);
  return 0;
}

int serialGetChar(FILE *stream)
{
  while (!Serial.available())
    ;
  return Serial.read();
}

void DisplayWelcomeMessage()
{
  LcdClear();
  LcdPrintAt(0, 0, "Enter Code:");
  LcdPrintAt(0, 1, "");

  printf("========================================\n");
  printf("Lab 1.2 - Code Validation System\n");
  printf("========================================\n");
  printf("STDIO Library - LCD + Keypad Interface\n");
  printf("Instructions:\n");
  printf("  - Enter 4-digit code on keypad\n");
  printf("  - Press # to submit code\n");
  printf("  - Press * to clear entry\n");
  printf("========================================\n");
  printf("System ready. Waiting for input...\n");
}

void ValidateCode()
{
  enteredCode[codeIndex] = '\0';

  printf("Code entered: %s\n", enteredCode);

  if (strcmp(enteredCode, VALID_CODE) == 0)
  {
    printf("Code VALID - Access Granted!\n");

    LcdClear();
    LcdPrintAt(0, 0, "Access Granted!");
    LcdPrintAt(0, 1, "Welcome!");

    LedOn(GREEN_LED_PIN);
    LedOff(RED_LED_PIN);
  }
  else
  {
    printf("Code INVALID - Access Denied!\n");

    LcdClear();
    LcdPrintAt(0, 0, "Access Denied!");
    LcdPrintAt(0, 1, "Try Again");

    LedOn(RED_LED_PIN);
    LedOff(GREEN_LED_PIN);
  }

  delay(DISPLAY_DELAY_MS);

  LedOff(GREEN_LED_PIN);
  LedOff(RED_LED_PIN);

  codeIndex = 0;
  memset(enteredCode, 0, MAX_CODE_LENGTH + 1);

  DisplayWelcomeMessage();
}

void ClearEntry()
{
  printf("Entry cleared\n");

  codeIndex = 0;
  memset(enteredCode, 0, MAX_CODE_LENGTH + 1);

  LcdClear();
  LcdPrintAt(0, 0, "Enter Code:");
  LcdPrintAt(0, 1, "");
}

void ProcessKeyPress(char key)
{
  if (key == '#')
  {
    printf("Submit key pressed\n");

    if (codeIndex > 0)
    {
      ValidateCode();
    }
    else
    {
      printf("No code entered\n");
      LcdClear();
      LcdPrintAt(0, 0, "No Code Entered!");
      delay(1000);
      DisplayWelcomeMessage();
    }
  }
  else if (key == '*')
  {
    ClearEntry();
  }
  else if (codeIndex < MAX_CODE_LENGTH)
  {
    enteredCode[codeIndex++] = key;

    printf("Key pressed: %c\n", key);

    LcdSetCursor(codeIndex - 1, 1);
    LcdPrint("*");

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "Entered: %s", enteredCode);
    printf("%s\n", buffer);
  }
  else
  {
    printf("Max code length reached\n");
  }
}

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  fdev_setup_stream(&serialStream, serialPutChar, serialGetChar, _FDEV_SETUP_RW);
  stdout = &serialStream;
  stdin = &serialStream;

  LedInit(GREEN_LED_PIN);
  LedInit(RED_LED_PIN);

  LcdInit();
  KeypadInit();

  memset(enteredCode, 0, MAX_CODE_LENGTH + 1);
  codeIndex = 0;

  DisplayWelcomeMessage();
}

void loop()
{
  char key = KeypadGetKey();

  if (key != NO_KEY_PRESSED)
  {
    ProcessKeyPress(key);
  }
}
