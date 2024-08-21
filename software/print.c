#include "core.h"

extern bit busy;

void send_char(char c) {
  while (busy) ;
  busy = 1;
  SBUF = c;
}

void send_str(char *s) {
  while (*s) {
    send_char(*s++);
  }
}

void send_hex(BYTE hex) {
  BYTE hex_str[2];
  BYTE i;
  hex_str[0] = (hex >> 4) & 0x0F;
  hex_str[1] = hex & 0x0F;
  send_str("0x");
  for (i = 0; i < 2; i++) {
    if (hex_str[i] < 10) {
      send_char(hex_str[i] + '0');
    } else {
      send_char(hex_str[i] - 10 + 'A');
    }
  }  
}