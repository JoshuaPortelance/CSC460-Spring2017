/*
 * base.c
 *
 * Created: 4/1/2017 2:04:31 PM
 *  Author: Jakob
 */ 

#define F_CPU 16000000UL
#include "os.h"
//#include "base.h"
#include "uart.h"
#include <avr/io.h>
#include <stdlib.h>

/*
void init() {
//   Serial.begin(9600);
//   Serial1.begin(9600);
  
  
  
  pinMode(rightJoySWPin, INPUT_PULLUP); //Init pullup res. 4 rightjoySWpin.
  //make input, then set to high to get pullup!
  //https://www.arduino.cc/en/Reference/Constants

  //INIT Joystick Centers
  getJoyCenter();
  delay(2000);

  Scheduler_StartTask(6, 50, bt_sendData);
  Scheduler_StartTask(8, 50, checkLeftJoyAxis);
  Scheduler_StartTask(10, 50, checkRightJoyAxis);
  Scheduler_StartTask(12, 50, getLaserSpeed);
  Scheduler_StartTask(14, 50, getRoombaSpeed);
  Scheduler_StartTask(0, 135, rightJoySwitch);
  //Task_Create_Period(Transmit, 0, 10, 1, 1);
  
}

/*============================================================================*/
void init_LED(void){
	DDRB	= 0b11111111;	//configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560)
	PORTB	= 0b10000000;	//Set port to low
}

/*============================================================================*/
// INITIALIZES Joystick Centers
// void getJoyCenter() {
//   int repeat = 30;
//   for(i=0;i < repeat; i++){
//     leftXAxisCenter  += analogRead(leftJoyXPin);
//     leftYAxisCenter  += analogRead(leftJoyYPin);
//     rightXAxisCenter += analogRead(rightJoyXPin);
//     rightYAxisCenter += analogRead(rightJoyYPin);
//     delay(10);
//   }
//   leftXAxisCenter  /= repeat;
//   leftYAxisCenter  /= repeat;
//   rightXAxisCenter /= repeat;
//   rightYAxisCenter /= repeat;
// }

/*============================================================================*/
// SCHEDULED
void bt_sendData()
{
	for(;;)
	{
		int i = 0;
		//TODO FOR NEGATIVES
		int panSpeed = 5;
		int tiltSpeed = 5;
		int laserState = 1;
		int speedRoomba = -249;
		int radiusRoomba = -1234;
		
		
		// #5|5|1|-500|-2000%
		char data[18];
		data[0] = '#';
		data[1] = (char) panSpeed;
		data[2] = '|';
		data[3] = (char) tiltSpeed;
		data[4] = '|';
		data[5] = (char) laserState;
		data[6] = '|';
		
		// speedroomba 4 chars
		if(speedRoomba < 0)
		{
			data[7] = '-';
		}
		else
		{
			data[7] = ' ';
		}
		
		char* SR = (char)abs(speedRoomba);
		
		if(abs(speedRoomba) < 100)
		{
			data[8] = '0';
			if(abs(speedRoomba) < 10)
			{
				data[9] = '0';
				if(abs(speedRoomba) < 1)
				{
					data[10] = '0';
				}
				else
				{
					data[10] = SR[0];
				}
			}
			else
			{
				data[9]  = SR[0];
				data[10] = SR[1];
			}
		}
		else
		{
			data[8]  = SR[0];
			data[9]  = SR[1];
			data[10] = SR[2];
		}
		
		data[11]='|';
		
		//radiusroomba 5 chars
		if(radiusRoomba < 0)
		{
			data[12] = '-';
		}
		else
		{
			data[12] = ' ';
		}
		
		char* RR = (char)abs(radiusRoomba);
		
		if(abs(radiusRoomba) < 1000)
		{
			data[13] = '0';
			if(abs(radiusRoomba) < 100)
			{
				data[14] = '0';
				if(abs(radiusRoomba) < 10)
				{
					data[15] = '0';
					if(abs(radiusRoomba) < 1)
					{
						data[16] = '0';
					}
					else
					{
						data[16] = RR[0];
					}
				}
				else
				{
					data[15] = RR[0];
					data[16] = RR[1];
				}
			}
			else
			{
				data[14] = RR[0];
				data[15] = RR[1];
				data[16] = RR[2];
			}
		}
		else
		{
			data[13] = RR[0];
			data[14] = RR[1];
			data[15] = RR[2];
			data[16] = RR[3];
		}
		
		data[17] = '%';
		
		for(i = 0; i < 18; i++)
		{
			serial_write_bt(data[i]);
		}
		
		Task_Next();
	}
}

void a()
{
	for(;;)
		{
			serial_write_bt('a');
			Task_Next();
		}
}

/*============================================================================*/
// Main control loop of the program.
void a_main(){
	init_uart_bt();
	Task_Create_Period(a, 0, 5, 1, 1);
}

/*============================================================================*/
// SCHEDULED
// void getLaserSpeed() {
//   panSpeed = -(rightXAxis - rightXAxisCenter);
//   panSpeed = panSpeed/90;

//   tiltSpeed = -(rightYAxis - rightYAxisCenter);
//   tiltSpeed = tiltSpeed/90;
// }
/*============================================================================*/
// SCHEDULED
// void getRoombaSpeed() {
//   int radiusOffset = 40;
//   //0 is straight
//   //2000 (+and-) are next increment, (+/-)1 is fastest spin
//   radiusRoomba = -(leftXAxis - leftXAxisCenter);
//   if(radiusRoomba < radiusOffset && radiusRoomba > -radiusOffset){
//     radiusRoomba = 0;
//   }else{
//     radiusRoomba = radiusRoomba*4;

//     if(radiusRoomba >  2000)radiusRoomba =  1999;
//     if(radiusRoomba < -2000)radiusRoomba = -1999;

//     if(radiusRoomba <  2000 && radiusRoomba >=  radiusOffset)radiusRoomba = 2000-radiusRoomba;
//     if(radiusRoomba > -2000 && radiusRoomba <= -radiusOffset)radiusRoomba = -2000-radiusRoomba;
//   }

//   //(+/-)500
//   int MAXSPEED = 3; //* 100 !!!
//   int speedOffset = 30;
//   float tempspeed=0.000;
//   speedRoomba = -(leftYAxis - leftYAxisCenter);
//   if(speedRoomba < speedOffset && speedRoomba > -speedOffset){
//     speedRoomba = 0;
//   }else{
//     tempspeed = speedRoomba / 5; //(normal max is 500)
//     speedRoomba = tempspeed * MAXSPEED;
//     if(speedRoomba >  500)speedRoomba =  500;
//     if(speedRoomba < -500)speedRoomba = -500;
//   }
// }
/*============================================================================*/
// SCHEDULED
// void rightJoySwitch() {
//   if (digitalRead(rightJoySWPin) == 0) { //pushing button makes value go to 0
//     if(laserState == 0){
//       laserState = 1;
//     }else{
//       laserState = 0;
//     }
//   }
// }
/*============================================================================*/
// SCHEDULED
// void checkLeftJoyAxis() {
//   digitalWrite(32, HIGH);
//   leftXAxis = analogRead(leftJoyXPin);
//   leftYAxis = analogRead(leftJoyYPin);
//   if(leftXAxis<joyMin || leftXAxis>joyMax)errorDisplay("leftXAxis");
//   if(leftYAxis<joyMin || leftYAxis>joyMax)errorDisplay("leftYAxis");
//   digitalWrite(32, LOW);
// }
/*============================================================================*/
// SCHEDULED
// void checkRightJoyAxis() {
//   digitalWrite(33, HIGH);
//   rightXAxis = analogRead(rightJoyXPin);
//   rightYAxis = analogRead(rightJoyYPin);
//   if(rightXAxis<joyMin || rightXAxis>joyMax)errorDisplay("rightXAxis");
//   if(rightYAxis<joyMin || rightYAxis>joyMax)errorDisplay("rightYAxis");
//   digitalWrite(33, LOW);
// }