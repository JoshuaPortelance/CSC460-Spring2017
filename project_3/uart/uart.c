/*
 * uart.c
 *
 * Created: 4/1/2017 2:39:58 PM
 *  Author: Josh
 */ 

#define F_CPU 16000000UL
#define _BV(bit) \ (1 << (bit))
#define BAUD 9600	// This needs to be defined, but is never used directly.
#include "uart.h"
#include <avr/io.h>
#include <util/setbaud.h>

void init_uart_usb() {
	UBRR0H = 0b0000;		// This is for 9600 Baud.
	UBRR0L = 0b01100111;	// This is for 9600 Baud.

	UCSR0A &= ~(_BV(U2X0));

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); /* 8-bit data */
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   /* Enable RX and TX */
}

unsigned char serial_read_usb()
{
	while ((UCSR0A & _BV(RXC0)) == 0)		// While data is NOT available to read
	{;;}
	return UDR0;
}

void serial_write_usb(unsigned char DataOut)
{
	while ((UCSR0A & _BV(UDRE0)) == 0)		// while NOT ready to transmit
	{;;}
	UDR0 = DataOut;
}


void init_uart_bt() {
	UBRR1H = 0b0000;		// This is for 9600 Baud.
	UBRR1L = 0b01100111;	// This is for 9600 Baud.

	UCSR1A &= ~(_BV(U2X1));

	UCSR1C = _BV(UCSZ11) | _BV(UCSZ10); /* 8-bit data */
	UCSR1B = _BV(RXEN1) | _BV(TXEN1);   /* Enable RX and TX */
}

unsigned char serial_read_bt()
{
	while ((UCSR1A & _BV(RXC1)) == 0)		// While data is NOT available to read
	{;;}
	return UDR0;
}

void serial_write_bt(unsigned char DataOut)
{
	while ((UCSR1A & _BV(UDRE1)) == 0)		// while NOT ready to transmit
	{;;}
	UDR0 = DataOut;
}


void init_uart_roomba() {
	UBRR2H = 0b0000;		// This is for 19200 Baud.
	UBRR2L = 0b00110011;	// This is for 19200 Baud.

	UCSR2A &= ~(_BV(U2X2));

	UCSR2C = _BV(UCSZ21) | _BV(UCSZ20); /* 8-bit data */
	UCSR2B = _BV(RXEN2) | _BV(TXEN2);   /* Enable RX and TX */
}

unsigned char serial_read_roomba()
{
	while ((UCSR2A & _BV(RXC2)) == 0)		// While data is NOT available to read
	{;;}
	return UDR0;
}

void serial_write_roomba(unsigned char DataOut)
{
	while ((UCSR2A & _BV(UDRE2)) == 0)		// while NOT ready to transmit
	{;;}
	UDR0 = DataOut;
}