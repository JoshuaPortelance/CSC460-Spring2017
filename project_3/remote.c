/*
 * remote.c
 *
 * Created: 4/1/2017 2:04:18 PM
 *  Author: Josh
 */

#define F_CPU 16000000UL
#define BAUD 9600
#include "os.h"
#include "uart.h"
#include <avr/io.h>
#include <util/setbaud.h>

void init_LED(void)
{
	DDRB	= 0b11111111;	//configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560)
	PORTB	= 0b10000000;	//Set port to low
}

void Blink()
{
	init_LED();
	for(;;)
	{
		PORTB	^= 0b10000000;
		Task_Next();
	}
}

void Transmit()
{
	init_uart_usb();
	for(;;)
	{
		serial_write_usb('H');
		Task_Next();
	}
}

void a_main()
{
	//Task_Create_Period(Blink, 0, 10, 1, 1);
	Task_Create_Period(Transmit, 0, 10, 1, 1);
}