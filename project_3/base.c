/*
 * base.c
 *
 * Created: 4/1/2017 2:04:31 PM
 * Author: Jakob
 */

#define F_CPU 16000000UL
#include "os.h"
#include "base.h"
#include "uart/uart.c"
#include <util/delay.h>
#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>

/*============================================================================*/
// SCHEDULED
void bt_sendData(){
	for(;;){
		int i = 0;
		int c = 0;
		char buffer[2];

		//TODO FOR NEGATIVES
		// int panSpeed     = 3;
		// int tiltSpeed    = 0;
		// int laserState   = 0;
		// int speedRoomba  = -24;
		// int radiusRoomba = -124;


		// #-5|-5|1|-500|-2000%
		char data[20];
		data[c++] = '#';
		if(panSpeed < 0){
			data[c++] = '-';
		}
		sprintf(buffer,"%d",abs(panSpeed));
		data[c++] = buffer[0];

		data[c++] = '|';

		if(tiltSpeed < 0){
			data[c++] = '-';
		}
		sprintf(buffer,"%d",abs(tiltSpeed));
		data[c++] = buffer[0];

		data[c++] = '|';

		sprintf(buffer,"%d",abs(laserState));
		data[c++] = buffer[0];

		data[c++] = '|';

		// speedroomba 4 chars
		if(speedRoomba < 0){
			data[c++] = '-';
		}
		char SR[4];
		int value1 = abs(speedRoomba);
		snprintf(SR, 4, "%d", value1);

		if(abs(speedRoomba) < 100){
			if(abs(speedRoomba) < 10){
				if(abs(speedRoomba) < 1){
					data[c++] = SR[0];
				}
			}else{
				data[c++]  = SR[0];
				data[c++] = SR[1];
			}
		}else{
			data[c++]  = SR[0];
			data[c++]  = SR[1];
			data[c++] = SR[2];
		}
		data[c++]='|';

		//radiusroomba 5 chars
		if(radiusRoomba < 0){
			data[c++] = '-';
		}

		char RR[5];
		int value2 = abs(radiusRoomba);
		snprintf(RR, 5, "%d", value2);

		if(abs(radiusRoomba) < 1000){
			if(abs(radiusRoomba) < 100){
				if(abs(radiusRoomba) < 10){
					if(abs(radiusRoomba) < 1){
						data[c++] = RR[0];
					}
				}else{
					data[c++] = RR[0];
					data[c++] = RR[1];
				}
			}else{
				data[c++] = RR[0];
				data[c++] = RR[1];
				data[c++] = RR[2];
			}
		}else{
			data[c++] = RR[0];
			data[c++] = RR[1];
			data[c++] = RR[2];
			data[c++] = RR[3];
		}

		data[c++] = '%';

		for(i = 0; i < c; i++)
		{
			serial_write_bt(data[i]);
			serial_write_usb(data[i]);
		}

		Task_Next();
	}
}


/*============================================================================*/
// SCHEDULED
void getSpeeds() {
  for(;;){
    //Servo speeds
    panSpeed = -(rightXAxis - rightXAxisCenter);
    panSpeed = panSpeed/23;
    tiltSpeed = -(rightYAxis - rightYAxisCenter);
    tiltSpeed = tiltSpeed/23;



    //Roomba Radius
    int radiusOffset = 40;
    //0 is straight
    //2000 (+and-) are next increment, (+/-)1 is fastest spin
    radiusRoomba = -(leftXAxis - leftXAxisCenter);
    if(radiusRoomba < radiusOffset && radiusRoomba > -radiusOffset){
      radiusRoomba = 0;
    }else{
      radiusRoomba = radiusRoomba*4;

      if(radiusRoomba >  2000)radiusRoomba =  1999;
      if(radiusRoomba < -2000)radiusRoomba = -1999;

      if(radiusRoomba <  2000 && radiusRoomba >=  radiusOffset)radiusRoomba = 2000-radiusRoomba;
      if(radiusRoomba > -2000 && radiusRoomba <= -radiusOffset)radiusRoomba = -2000-radiusRoomba;
    }



    //Roomba Speed
    //(+/-)500
    int MAXSPEED = 3; //* 100 !!!
    int speedOffset = 30;
    float tempspeed=0.000;
    speedRoomba = -(leftYAxis - leftYAxisCenter);
    if(speedRoomba < speedOffset && speedRoomba > -speedOffset){
      speedRoomba = 0;
    }else{
      tempspeed = speedRoomba / 5; //(normal max is 500)
      speedRoomba = tempspeed * MAXSPEED;
      if(speedRoomba >  500)speedRoomba =  500;
      if(speedRoomba < -500)speedRoomba = -500;
    }
    Task_Next();
	}
}
/*============================================================================*/
// SCHEDULED (periodic @ 135ms)
void rightJoySwitch() {
  for(;;){
		int val = PINB & 1;
		if (val == 0) {
			if(laserState == 0){
	      laserState = 1;
	    }else{
	      laserState = 0;
	    }
		}
    // laserState = PINB & 1; // Dpin 53
    Task_Next();
	}
}
/*============================================================================*/
// SCHEDULED (periodic)
void checkJoysticks() {
  for(;;){
    leftXAxis = readADC(leftJoyXPin);
    leftYAxis = readADC(leftJoyYPin);
    if(leftXAxis<joyMin || leftXAxis>joyMax)Blink();
    if(leftYAxis<joyMin || leftYAxis>joyMax)Blink();
    rightXAxis = readADC(rightJoyXPin);
    rightYAxis = readADC(rightJoyYPin);
    if(rightXAxis<joyMin || rightXAxis>joyMax)Blink();
    if(rightYAxis<joyMin || rightYAxis>joyMax)Blink();
    Task_Next();
	}
}
/*============================================================================*/
// INITIALIZERS
void getJoyCenter() {
  int repeat = 30; //repeat number of times at initialization
	serial_write_usb('S');
  for(i=0;i < repeat; i++){
    leftXAxisCenter  += readADC(leftJoyXPin);
    leftYAxisCenter  += readADC(leftJoyYPin);
    rightXAxisCenter += readADC(rightJoyXPin);
    rightYAxisCenter += readADC(rightJoyYPin);
  }
	serial_write_usb('F');
  leftXAxisCenter  /= repeat;
  leftYAxisCenter  /= repeat;
  rightXAxisCenter /= repeat;
  rightYAxisCenter /= repeat;
}
void init_LED(void){
	DDRB	= 0b11111111;	//configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560)
	PORTB	= 0b10000000;	//Set port to low
}
void initADC() {
	DDRC = (DDRC & 0xF0);
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // Set ADC prescalar to 128 - 125KHz sample rate @ 16MHz
	ADMUX |= (1 << REFS0); // Set ADC reference to AVCC
	ADMUX |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading
	ADCSRA |= (1 << ADEN);  // Enable ADC
	ADCSRA |= (1 << ADSC); //Start a conversion to warmup the ADC.


  // ADMUX  |= 1 << REFS0;
  // ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); // prescaler 128
}
void initLaserSwitch(){
	DDRB  |= 0b00000000;     // init Dpin 53 for rightJoySW
	PORTB |= 0b00000001;     // set pullup for pin 53
}
/*============================================================================*/
// inits
void initialize(){
	init_uart_bt(); // init connected BT
	init_uart_usb();
	init_LED();     // init onboard LED
	initADC();
	initLaserSwitch();
	getJoyCenter(); // init Joystick Center values
}
/*============================================================================*/
// ADC reader (analogRead)
uint16_t readADC(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0 ) | (0x07 & channel);
	ADCSRB = (ADCSRB & 0xF7) | (channel & (1 << MUX5));
	ADCSRA |= (1 << ADSC);
	while ((ADCSRA & (1 << ADSC)));
	return ADCH;
  // ADMUX  &= 0xE0;
  // ADMUX  |= pin & 0x07;
  // ADCSRB  = pin & (1 << 3);
  // ADCSRA |= 1 << ADSC;
  // while(ADCSRA & (1 << ADSC));
  // return ADCW;
}
/*============================================================================*/
// Blink for Error State
void Blink(){
	for(;;){
		PORTB ^= 0b10000000;
		Task_Next();
	}
}

void a(){
	for(;;){
		char c = 'a';
		serial_write_bt(c);
		serial_write_usb(c);
		Task_Next();
	}
}

/*============================================================================*/
// Main control loop of the program.

/*
ADC
ADLAR High means that 256 values
ADC is split 2-8
or them together to get it all (set adlar to 0)
take the full ADCL (low) and or it with ADCH left shifted 8
as soon as you read from ADCH it invalidates ADCH and ADCH so do ADCL first!, put them in variables and then or them together
https://github.com/sdiemert/CSC460Project3/blob/master/basestation/joystick.c

*/
void a_main(){

	Task_Create_System(initialize, 0);
	//PID Task_Create_Period(voidfuncptr f, int arg, TICK period, TICK wcet, TICK offset)
	// Task_Create_Period(a    , 0, 0.1,  2, 1); //testing only
	//FIX TIX TO MS
	Task_Create_Period(bt_sendData    , 0, 14,  2, 200);
	Task_Create_Period(checkJoysticks , 0, 14,  2, 202);
	Task_Create_Period(getSpeeds      , 0, 14,  2, 204);
	Task_Create_Period(rightJoySwitch , 0, 14, 1, 206);
}
