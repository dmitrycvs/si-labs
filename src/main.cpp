#include <Arduino.h>
#include <stdio.h>
#include "Led.h"

const uint8_t LED_PIN = 13;
const uint32_t SERIAL_BAUD_RATE = 9600;
const uint8_t COMMAND_BUFFER_SIZE = 32;

char commandBuffer[COMMAND_BUFFER_SIZE];
uint8_t bufferIndex = 0;

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

FILE serialStream;

void ProcessCommand(const char *command)
{
  char trimmedCommand[COMMAND_BUFFER_SIZE];
  int len = strlen(command);
  int i;

  for (i = 0; i < len && command[i] != '\0'; i++)
  {
    trimmedCommand[i] = command[i];
  }
  trimmedCommand[i] = '\0';

  while (i > 0 && (trimmedCommand[i - 1] == '\n' || trimmedCommand[i - 1] == '\r' || trimmedCommand[i - 1] == ' '))
  {
    trimmedCommand[--i] = '\0';
  }

  if (strcmp(trimmedCommand, "led on") == 0)
  {
    LedOn(LED_PIN);
    printf("OK: LED turned ON\n");
    printf("LED state: ON\n");
  }
  else if (strcmp(trimmedCommand, "led off") == 0)
  {
    LedOff(LED_PIN);
    printf("OK: LED turned OFF\n");
    printf("LED state: OFF\n");
  }
  else if (strlen(trimmedCommand) > 0)
  {
    printf("ERROR: Unknown command '%s'\n", trimmedCommand);
    printf("Available commands:\n");
    printf("  - led on\n");
    printf("  - led off\n");
  }
}

void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);

  fdev_setup_stream(&serialStream, serialPutChar, serialGetChar, _FDEV_SETUP_RW);
  stdout = &serialStream;
  stdin = &serialStream;

  LedInit(LED_PIN);

  printf("\n========================================\n");
  printf("Lab 1.1 - LED Control via Serial\n");
  printf("========================================\n");
  printf("STDIO Library - Serial Interface\n");
  printf("Commands:\n");
  printf("  - led on  : Turn LED ON\n");
  printf("  - led off : Turn LED OFF\n");
  printf("========================================\n");
  printf("Ready. Enter command:\n> ");

  bufferIndex = 0;
  memset(commandBuffer, 0, COMMAND_BUFFER_SIZE);
}

void loop()
{
  if (Serial.available() > 0)
  {
    char receivedChar = Serial.read();

    Serial.print(receivedChar);

    if (receivedChar == '\n' || receivedChar == '\r')
    {
      if (bufferIndex > 0)
      {
        commandBuffer[bufferIndex] = '\0';
        ProcessCommand(commandBuffer);
        printf("> ");

        bufferIndex = 0;
        memset(commandBuffer, 0, COMMAND_BUFFER_SIZE);
      }
    }
    else if (bufferIndex < COMMAND_BUFFER_SIZE - 1)
    {
      commandBuffer[bufferIndex++] = receivedChar;
    }
    else
    {
      printf("\nERROR: Command too long. Max %d characters.\n", COMMAND_BUFFER_SIZE - 1);
      printf("> ");
      bufferIndex = 0;
      memset(commandBuffer, 0, COMMAND_BUFFER_SIZE);
    }
  }
}
