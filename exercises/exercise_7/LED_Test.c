#define F_CPU 16000000L		// Specify oscillator frequenc
#include <avr/io.h>
#include "LED_Test.h"
/**
 * \file LED_Test.c
 * \brief Small set of test functions for controlling LEDs on a AT90USBKey
 * 
 * \main page Simple set of functions to control the state of the on board
 *  LEDs on the AT90USBKey. 
 *
 * \author Alexander M. Hoole
 * \date October 2006
 */

void init_LED(void)
{
	DDRB	= 0b11111111;	//configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560)
	PORTB	= 0b00000000;	//Set port to low
}

void enable_LED(void)
{
	PORTB	= 0b10000000;	//Set port to high
}

void disable_LED(void)
{
	PORTB	= 0b00000000;	//Set port to low
}
