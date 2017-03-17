/*
 * user_space.c
 *
 * Created: 3/11/2017 9:11:25 AM
 *  Author: Josh
 */

#define F_CPU 16000000L	    // Specify oscillator frequency
#include <util/delay.h>
#include <avr/io.h>
#include "os.h"

void init_LED(void)
{
	DDRB	= 0b11111111;	//configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560)
	PORTB	= 0b10000000;	//Set port to low
}

void enable_LED(void)
{
	PORTB	|= 0b10000000;	//Set port to high
}

void disable_LED(void)
{
	PORTB	&= 0b01111111;	//Set port to low
}

/*============
  * A Simple Test
  *============
  */

/**
  * A cooperative "Ping" task.
  * Added testing code for LEDs.
  */
void Ping()
{
	init_LED();
	for(;;) {
		enable_LED();
		_delay_ms(50);
		Task_Next();
	}
}

/**
  * A cooperative "Pong" task.
  * Added testing code for LEDs.
  */
void Pong()
{
	init_LED();
	for(;;) {
		disable_LED();
		_delay_ms(50);
		Task_Next();
	}
}

void ToggleLED()
{
	init_LED();
	for(;;) {
		PORTB ^= 0b10000000;
		Task_Next();
	}
}

void main_a(void)
{
	// Create all tasks.

	/* This is working!!!!
	Task_Create_System(Ping, 1);
	Task_Create_System(Pong, 2);
	*/

	/* This is working!!!!
	Task_Create_RR(Ping, 1);
	Task_Create_RR(Pong, 2);
	*/

	// This is working!!!!
	Task_Create_Period(ToggleLED, 1, 2, 1, 20);
}