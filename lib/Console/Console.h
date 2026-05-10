#ifndef CONSOLE_H
#define CONSOLE_H

#include <Arduino.h>
#include <stdio.h>

namespace Console {
  int uart_putchar(char c, FILE *stream);
  int uart_getchar(FILE *stream);
  void init();
}

#endif