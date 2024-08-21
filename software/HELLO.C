#include "STC89C5xRC.H"
#include "intrins.h"
#include "stdio.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

// global buffer for RXD input
BYTE buffer[32];
BYTE buffer_index = 0;

sbit bit0 = P1 ^ 0;
sbit bit1 = P1 ^ 1;
bit busy;

void wait(unsigned int i);

void Uart1_Init(void) // 2400bps@12.000MHz
{
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
  // enable RXD to receive data
  SCON |= 0x10;
}

void Uart_Isr() interrupt 4 using 1 {
  if (RI) {
    RI = 0;
    buffer[buffer_index++] = SBUF;
    bit0 = RI;
  }
  if (TI) {
    TI = 0;
    busy = 0;
    bit1 = TI;
  }
}

void send_char(char c) {
  while (busy)
    ;
  // Wait for the last transmission to complete
  busy = 1;
  // Set the busy flag
  SBUF = c;
  // Send data to UART buffer
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

BYTE get_char() {
  while (buffer_index == 0)
    ;
  return buffer[--buffer_index];
}

BYTE tmp_char;

void main() {
  char cmd[32];
  int i = 0;
  Uart1_Init();
  send_str("Welcome to QUARTZ_RT minimal OS for STC89 series microcontrollers\n\r");
  send_str("Author: wheatfox(enkerewpo@hotmail.com)\n\r");
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
      send_str("You entered: ");
      send_str(cmd);
      send_str("\n\r");
    }
    i = 0;
  }
}

void wait(unsigned int i) {
  unsigned int j;
  for (j = 0; j < i; j++) {
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
    _nop_();
  }
}