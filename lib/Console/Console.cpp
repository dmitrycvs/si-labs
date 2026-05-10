#include "Console.h"

static FILE uart_stream;

namespace Console {
  int uart_putchar(char c, FILE *stream) {
    Serial.write(c);
    if (c == '\n') Serial.write('\r');
    return 0;
  }

  int uart_getchar(FILE *stream) {
    if (!Serial.available()) return -1;
    char c = (char)Serial.read();
    Serial.write(c);
    return (int)c;
  }

  void init() {
    Serial.begin(9600);
    fdev_setup_stream(&uart_stream, uart_putchar, uart_getchar, _FDEV_SETUP_RW);
    stdout = &uart_stream;
    stdin  = &uart_stream;
  }
}
