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

		//TODO FOR NEGATIVES
		int panSpeed     = -5;
		int tiltSpeed    = -5;
		int laserState   = 1;
		int speedRoomba  = -249;
		int radiusRoomba = -1234;


		// #-5|-5|1|-500|-2000%
		char data[20];
		data[0] = '#';
		if(panSpeed < 0){
			data[1] = '-';
		}else{
			data[1] = ' ';
		}
		data[2] = (char) panSpeed;
		data[3] = '|';
		if(tiltSpeed < 0){
			data[4] = '-';
		}else{
			data[4] = ' ';
		}
		data[5] = (char) tiltSpeed;
		data[6] = '|';
		data[7] = (char) laserState;
		data[8] = '|';

		// speedroomba 4 chars
		if(speedRoomba < 0){
			data[9] = '-';
		}else{
			data[9] = ' ';
		}
		char SR[4];
		int value1 = abs(speedRoomba);
		snprintf(SR, 4, "%d", value1);

		if(abs(speedRoomba) < 100){
			data[10] = '0';
			if(abs(speedRoomba) < 10){
				data[11] = '0';
				if(abs(speedRoomba) < 1){
					data[12] = '0';
				}else{
					data[12] = SR[0];
				}
			}else{
				data[11]  = SR[0];
				data[12] = SR[1];
			}
		}else{
			data[10]  = SR[0];
			data[11]  = SR[1];
			data[12] = SR[2];
		}
		data[13]='|';

		//radiusroomba 5 chars
		if(radiusRoomba < 0){
			data[14] = '-';
		}else{
			data[14] = ' ';
		}

		char RR[5];
		int value2 = abs(radiusRoomba);
		snprintf(RR, 5, "%d", value2);

		if(abs(radiusRoomba) < 1000){
			data[15] = '0';
			if(abs(radiusRoomba) < 100){
				data[16] = '0';
				if(abs(radiusRoomba) < 10){
					data[17] = '0';
					if(abs(radiusRoomba) < 1){
						data[18] = '0';
					}else{
						data[18] = RR[0];
					}
				}else{
					data[17] = RR[0];
					data[18] = RR[1];
				}
			}else{
				data[16] = RR[0];
				data[17] = RR[1];
				data[18] = RR[2];
			}
		}else{
			data[15] = RR[0];
			data[16] = RR[1];
			data[17] = RR[2];
			data[18] = RR[3];
		}

		data[19] = '%';

		for(i = 0; i < 18; i++)
		{
			serial_write_bt(data[i]);
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
    panSpeed = panSpeed/90;
    tiltSpeed = -(rightYAxis - rightYAxisCenter);
    tiltSpeed = tiltSpeed/90;

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
    laserState = PINB & 1; // Dpin 53
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
  for(i=0;i < repeat; i++){
    //values 0-1023 each time
    leftXAxisCenter  += readADC(leftJoyXPin);
    leftYAxisCenter  += readADC(leftJoyYPin);
    rightXAxisCenter += readADC(rightJoyXPin);
    rightYAxisCenter += readADC(rightJoyYPin);
    _delay_ms(10);
  }
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
  ADMUX  |= 1 << REFS0;
  ADCSRA |= (1 << ADEN) | (1 << ADPS0) | (1 << ADPS1) | (1 << ADPS2); // prescaler 128
}
void initLaserSwitch(){
	DDRB  |= 0b00000000;     // init Dpin 53 for rightJoySW
	PORTB |= 0b00000001;     // set pullup for pin 53
}
/*============================================================================*/
// ADC reader (analogRead)
uint16_t readADC(uint8_t pin) {
  ADMUX  &= 0xE0;
  ADMUX  |= pin & 0x07;
  ADCSRB  = pin & (1 << 3);
  ADCSRA |= 1 << ADSC;
  while(ADCSRA & (1 << ADSC));
  return ADCW;
}
/*============================================================================*/
// Blink for Error State
void Blink(){
	for(;;){
		PORTB ^= 0b10000000;
		_delay_ms(500);
	}
}
/*============================================================================*/
// Main control loop of the program.
void a_main(){
	init_uart_bt(); // init connected BT
	init_LED();     // init onboard LED
	initADC();
	initLaserSwitch();
	getJoyCenter(); // init Joystick Center values

	//PID Task_Create_Period(voidfuncptr f, int arg, TICK period, TICK wcet, TICK offset)
	Task_Create_Period(bt_sendData    , 0, 50,  5, 0);
	Task_Create_Period(getSpeeds      , 0, 50,  5, 3);
	Task_Create_Period(checkJoysticks , 0, 50,  5, 6);
	Task_Create_Period(rightJoySwitch , 0, 135, 5, 9);
}
