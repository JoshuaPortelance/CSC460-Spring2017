
/*
 * servo.c
 *
 * Created: 4/4/2017 9:55:01 AM
 *  Author: Josh
 */

#include "servo.h"
#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>

static float MAX_ANGLE_DELTA = 5;		// Max amount that the servo angle can change by.
static float MIN_ANGLE_DELTA = -5;		// Min amount that the servo angle can change by.
static float MAX_ANGLE_VAL = 300;		// Max value which corresponds to 180 degrees on the servo.
static float MIN_ANGLE_VAL = 69;		// Min value which corresponds to 0 degrees on the servo.
static float DEFUALT_ANGLE_VAL = 188;	// Value which corresponds to 90 degrees which is the default value.

int current_pan_angle;
int current_tilt_angle;

/*============================================================================*/
void init_servos()
{
	// Setting PWM pins 2 & 3 to output and to low.
	DDRE  |= 0b00110000;
	PORTE &= 0b11001111;

	// Initializing timer.
	TCNT3 = 0;												// set counter to 0.
	TCCR3A = 0x00;
	TCCR3B = 0x00;
	TCCR3A = (1 << COM3B1) | (1 << COM3C1) | (1 << WGM30);  // NON Inverted PWM.
	TCCR3B |= (1 << WGM33) | (1 << CS41) | (1 << CS40);		// PRESCALER = 64 MODE 14 (FAST PWM).
	OCR3A = 2500;

	// Setting angle to 90 degrees.
	current_pan_angle  = DEFUALT_ANGLE_VAL;
	current_tilt_angle = DEFUALT_ANGLE_VAL;

	// Setting target angle to the current angle values.
	OCR3C = current_pan_angle;
	OCR3B = current_tilt_angle;
}

/*============================================================================*/
// angle_delta is an integer between -5 and 5.
void adjust_pan_angle(int angle_delta)
{
	//Verifying angle delta.
	if (angle_delta > MAX_ANGLE_DELTA)		// Max increase.
	{
		angle_delta = MAX_ANGLE_DELTA;
	}
	else if (angle_delta < MIN_ANGLE_DELTA)	// Max decrease.
	{
		angle_delta = MIN_ANGLE_DELTA;
	}

	//Verifying angle.	
	current_pan_angle += angle_delta;
	if (current_pan_angle > MAX_ANGLE_VAL)			// Max angle.
	{
		current_pan_angle = MAX_ANGLE_VAL;
	}
	else if (current_pan_angle < MIN_ANGLE_VAL)		// Min angle.
	{
		current_pan_angle = MIN_ANGLE_VAL;
	}
	OCR3C = current_pan_angle;
}

/*============================================================================*/
// angle_delta is an integer between -5 and 5.
void adjust_tilt_angle(int angle_delta)
{
	//Verifying angle delta.
	if (angle_delta > MAX_ANGLE_DELTA)		// Max increase.
	{
		angle_delta = MAX_ANGLE_DELTA;
	}
	else if (angle_delta < MIN_ANGLE_DELTA)	// Max decrease.
	{
		angle_delta = MIN_ANGLE_DELTA;
	}

	//Verifying angle.	
	current_tilt_angle += angle_delta;
	if (current_tilt_angle > MAX_ANGLE_VAL)			// Max angle.
	{
		current_tilt_angle = MAX_ANGLE_VAL;
	}
	else if (current_tilt_angle < MIN_ANGLE_VAL)	// Min angle.
	{
		current_tilt_angle = MIN_ANGLE_VAL;
	}
	OCR3B = current_tilt_angle;
}