#include "print.h"

// global buffer for RXD input
// round queue
#define BUFFER_SIZE 32
// global buffer for RXD input
BYTE buffer[BUFFER_SIZE];
BYTE buffer_index = 0;

sbit bit0 = P1 ^ 0;
sbit bit1 = P1 ^ 1;
bit busy;

// 2400bps@12.000MHz
void uart1_init(void) {
  PCON |= 0x80;
  SCON = 0x50;
  AUXR &= 0xBF;
  AUXR &= 0xFE;
  TMOD &= 0x0F;
  TMOD |= 0x20;
  TL1 = 0xE6;
  TH1 = 0xE6;
  TR1 = 1;
  ES = 1;
  EA = 1;
  SCON |= 0x10;
}

void uart_irq_handler() interrupt 4 using 1 {
  if (RI) {
    RI = 0;
    buffer[buffer_index++] = SBUF;
  }
  if (TI) {
    TI = 0;
    busy = 0;
  }
}

BYTE get_char() {
  while (buffer_index == 0)
    ;
  return buffer[--buffer_index];
}

void timer_irq_init() {
  TMOD = 0x01;
  TH0 = 0xFC;
  TL0 = 0x67;
  ET0 = 1;
  EA = 1;
  TR0 = 1;
}

int __strcmp(char *s1, char *s2) {
  // dictionary order comparison, less return -1, equal return 0, greater return
  // 1
  while (*s1 && *s2) {
    if (*s1 < *s2) {
      return -1;
    } else if (*s1 > *s2) {
      return 1;
    }
    s1++;
    s2++;
  }
  if (*s1) {
    return 1;
  } else if (*s2) {
    return -1;
  } else {
    return 0;
  }
}

void __stripe(char *s) {
  // remove leading and trailing empty characters
  // spaces, tabs, newlines, carriage returns
  char *p = s;
  char *q = s;
  while (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r') {
    p++;
  }
  while (*p) {
    *q++ = *p++;
  }
  *q = '\0';
  p = q - 1;
  while (p >= s && (*p == ' ' || *p == '\t' || *p == '\n' || *p == '\r')) {
    *p-- = '\0';
  }
}

void main() {
  BYTE tmp_char;
  char cmd[32];
  int i = 0;
  uart1_init();
  // timer_irq_init();
  send_str("Welcome to QUARTZ_RT minimal program for STC91C2 series "
           "microcontrollers\n\r");
  send_str("Author: wheatfox(enkerewpo@hotmail.com)\n\r");
  send_str("Build time: ");
  send_str(__TIME__);
  send_str(" ");
  send_str(__DATE__);
  send_str("\n\r");
  // enter shell
  while (1) {
    send_str("> ");
    while (1) {
      tmp_char = get_char();
      if (tmp_char == '\n') {
        // ignore '\n'
      } else if (tmp_char == '\r') {
        cmd[i] = '\0';
        break;
      } else {
        cmd[i++] = tmp_char;
        send_char(tmp_char);
      }
    }
    send_str("\n\r");
    if (i != 0) {
      __stripe(cmd);
      send_str("You entered: ");
      send_str(cmd);
      send_str("\n\r");
      if (__strcmp(cmd, "hello") == 0) {
        send_str("Hello, world!\n\r");
      } else if (__strcmp(cmd, "help") == 0) {
        send_str("Available commands: hello, help\n\r");
      } else {
        send_str("Unknown command\n\r");
      }
      send_str("\n\r");
    }
    i = 0;
  }
}