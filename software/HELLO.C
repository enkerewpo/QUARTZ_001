#include "STC89C5xRC.H"
#include "intrins.h"

typedef unsigned char BYTE;
typedef unsigned int WORD;

sbit bit9 = P1 ^ 1;
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
}

void Uart_Isr() interrupt 4 using 1 {
  if (RI) {
    RI = 0;
    // Clear receive interrupt flag
    P0 = SBUF;
    // P0 show UART data
    bit9 = SBUF;
  }
  if (TI) {
    TI = 0;
    // Clear transmit interrupt flag
    busy = 0;
    // Clear transmit busy flag
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

void main() {

  Uart1_Init();

  while (1) {
    send_str("Hello, world from wheatfox!\r\n");
    wait(1000);
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