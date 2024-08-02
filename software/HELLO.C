#include "STC89C5xRC.H"
#include <stdio.h>

void Uart1_Init(void)	//9600bps@12.010MHz
{
	PCON |= 0x80;		//Enable SMOD bit
	SCON = 0x50;		//8 bits and variable baudrate
	AUXR |= 0x40;		//imer clock is 1T mode
	AUXR &= 0xFE;		//UART 1 use Timer1 as baudrate generator
	TMOD &= 0x0F;		//Set timer work mode
	TMOD |= 0x20;		//Set timer work mode
	TL1 = 0xB2;			//Initial timer value
	TH1 = 0xB2;			//Set reload value
	ET1 = 0;			//Disable Timer%d interrupt
	TR1 = 1;			//Timer1 start run
}


void _putchar(unsigned char m) { SBUF = m; }

void delay() {
  unsigned char i, j, k;
  i = 8;
  j = 1;
  k = 243;
  do {
    do {
      while (--k)
        ;
    } while (--j);
  } while (--i);
}

void main() {
  Uart1_Init();
  while (1) {
    _putchar('p');
    delay();
  }
}