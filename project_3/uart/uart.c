/*
 * uart.c
 *
 * Created: 4/1/2017 2:39:58 PM
 *  Author: Josh
 */ 

#define F_CPU 16000000UL
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
	if((UCSR0A & _BV(RXC0)) == 0)		// If data if not available, return null char.
	{
		return '\0';
	}
	return UDR0;
}

void serial_write_usb(unsigned char data_out)
{
	while ((UCSR0A & _BV(UDRE0)) == 0)		// while NOT ready to transmit
	{;;}
	UDR0 = data_out;
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
	if((UCSR1A & _BV(RXC1)) == 0)		// If data if not available, return null char.
	{
		return '\0';
	}
	return UDR1;
}

void serial_write_bt(unsigned char data_out)
{
	while ((UCSR1A & _BV(UDRE1)) == 0)		// while NOT ready to transmit
	{;;}
	UDR1 = data_out;
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
	if((UCSR2A & _BV(RXC2)) == 0)		// If data if not available, return null char.
	{
		return '\0';
	}
	return UDR2;
}

void serial_write_roomba(unsigned char data_out)
{
	while ((UCSR2A & _BV(UDRE2)) == 0)		// while NOT ready to transmit
	{;;}
	UDR2 = data_out;
}