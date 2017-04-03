/*
 * remote.c
 *
 * Created: 4/1/2017 2:04:18 PM
 *  Author: Josh
 */

#define F_CPU 16000000UL
#include "os.h"
#include "uart.h"
#include "remote.h"
#include "roomba.h"
#include <avr/io.h>
#include <stdlib.h>

/*
void init_LED(void)
{
	DDRB	= 0b11111111;	//configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560)
	PORTB	= 0b10000000;	//Set port to low
}

void Blink()
{
	//init_LED();
	for(;;)
	{
		PORTB ^= 0b10000000;
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

volatile unsigned char x = 'H';

void ReadUSB()
{
	//init_uart_usb();
	for(;;)
	{
		unsigned char temp = serial_read_usb();
		if(temp != NULL && temp != '\0')
		{
			x = temp;
		}
		Task_Next();
	}
}

void WriteUSB()
{
	//init_uart_usb();
	for(;;)
	{
		serial_write_usb(x);
		Task_Next();
	}
}
*/

void setup()
{
	/*
	// PIN INIT//////////////////////
	pinMode(idle_debug_pin, OUTPUT);
	pinMode(laser_debug_pin, OUTPUT);
	pinMode(update_servo_debug_pin, OUTPUT);
	pinMode(update_roomba_debug_pin, OUTPUT);
	pinMode(transmit_debug_pin, OUTPUT);
	pinMode(receive_debug_pin, OUTPUT);
	pinMode(check_light_debug_pin, OUTPUT);
	
	pinMode(laserTogglePin, OUTPUT);
	
	// Serial INIT///////////////////
	Serial1.begin(9600);
	Serial.begin(9600);

	// Roomba INIT///////////////////
	r.init();
	
	panServo.attach(panServoPin);
	tiltServo.attach(tiltServoPin);
	
	// Start task arguments are:
	// Scheduler_StartTask(start offset in ms, period in ms, function);
	Scheduler_StartTask(0, 30, receiveTransmission);
	Scheduler_StartTask(3, 20, updateServos);
	Scheduler_StartTask(6, 100, updateRoomba);
	Scheduler_StartTask(9, 100, updateLaser);
	*/
	// Serial INIT
	init_uart_usb();
	init_uart_bt();
	
	// Roomba INTI
	init_roomba();
	
	// Laser INIT
	DDRA |= 0b00000001;		// Enable pin 22 as output
	PORTA &= 0b11111110;	// Turn pin 22 to low
	
	// Servo INIT
	
}

/*============================================================================*/
// Read in a update transmission.
void receive_transmission()
{
	for(;;)
	{
		int status = 0;
		
		// Packet #.
		unsigned int packetnum = 0;
		
		// Expected packet values.
		char panSpeed_val [MAXINPUT];
		char tiltSpeed_val [MAXINPUT];
		char laser_val [MAXINPUT];
		char roombaSpeed_val [MAXINPUT];
		char roombaRadius_val [MAXINPUT];

		serial_write_usb('1');

		// Cycle through char array.
		unsigned int input_pos = 0;
		
		while((UCSR1A & _BV(RXC1)) != 0)
		{
			serial_write_usb('2');
			const unsigned char inByte = serial_read_bt();
			
			// FOR DEBUG.
			serial_write_usb(inByte);
			
			if (inByte == ' ')
			{
				continue;
			}
			
			if (inByte == '#' || status == 1)
			{
				if (inByte == '#')
				{
					status = 1;
				}
				else if (inByte == '%')
				{
					// ADD NULL TERMINATORS.
					switch(packetnum)
					{
						case 0:
						panSpeed_val	  [input_pos] = '\0';  // terminating null byte
						break;
						case 1:
						tiltSpeed_val	  [input_pos] = '\0';  // terminating null byte
						break;
						case 2:
						laser_val         [input_pos] = '\0';  // terminating null byte
						break;
						case 3:
						roombaSpeed_val   [input_pos] = '\0';  // terminating null byte
						break;
						case 4:
						roombaRadius_val  [input_pos] = '\0';  // terminating null byte
						break;
						default:
						break;
					}
					
					// PROCESS INPUT VALUES.
					pan_speed			 = atoi(panSpeed_val);
					tilt_speed			 = atoi(tiltSpeed_val);
					laser_target_state	 = atoi(laser_val);
					roomba_target_speed  = atoi(roombaSpeed_val);
					roomba_target_radius = atoi(roombaRadius_val);
					
					// Reset values.
					input_pos = 0;
					packetnum = 0;
					status = 0;
					
				}
				else if (inByte == '|')
				{
					// ADD NULL TERMINATORS.
					switch(packetnum)
					{
						case 0:
						panSpeed_val	  [input_pos] = '\0';  // terminating null byte
						break;
						case 1:
						tiltSpeed_val     [input_pos] = '\0';  // terminating null byte
						break;
						case 2:
						laser_val         [input_pos] = '\0';  // terminating null byte
						break;
						case 3:
						roombaSpeed_val   [input_pos] = '\0';  // terminating null byte
						break;
						case 4:
						roombaRadius_val  [input_pos] = '\0';  // terminating null byte
						break;
						default:
						break;
					}
					input_pos = 0;
					packetnum++;
				}
				else
				{
					switch(packetnum)
					{
						case 0:
						if (input_pos < (MAXINPUT - 2))
						panSpeed_val[input_pos++] = inByte;
						break;
						case 1:
						if (input_pos < (MAXINPUT - 2))
						tiltSpeed_val[input_pos++] = inByte;
						break;
						case 2:
						if (input_pos < (MAXINPUT - 2))
						laser_val[input_pos++] = inByte;
						break;
						case 3:
						if (input_pos < (MAXINPUT - 2))
						roombaSpeed_val[input_pos++] = inByte;
						break;
						case 4:
						if (input_pos < (MAXINPUT - 2))
						roombaRadius_val[input_pos++] = inByte;
						break;
						default:
						break;
					}
				}
			}
		}
		Task_Next();
	}
}

/*============================================================================*/
// Update the state of the laser.
// ~ 0.05ms average runtime
void update_laser()
{
	for(;;)
	{
		if (laser_state != laser_target_state)
		{
			laser_state = laser_target_state;
			if (laser_target_state == 0)
			{
				PORTA &= 0b11111110;    // Turn laser off.
			}
			else if (laser_target_state == 1)
			{
				PORTA |= 0b00000001;    // Turn laser on.
			}
		}
		Task_Next();
	}
}

/*============================================================================*/
// Update the Roomba.
/* Go straight  - 32768 for radius
 * Stop         - 0 speed, 0 radius
 * Radius should be between -5 to 5
 * Speed should be between -500 to 500
 */
void update_roomba()
{
	for(;;)
	{
		if (roomba_current_speed == roomba_target_speed && roomba_current_radius == roomba_target_radius)
		{
			if (roomba_target_speed > MAXSPEED)
			{
				roomba_target_speed = MAXSPEED;
			}
			else if (roomba_target_speed < MINSPEED)
			{
				roomba_target_speed = MINSPEED;
			}
			
			if (roomba_target_radius > MAXRADIUS)
			{
				roomba_target_radius = MAXRADIUS;
			}
			else if (roomba_target_radius < MINRADIUS)
			{
				roomba_target_radius = MINRADIUS;
			}
			
			roomba_current_speed = roomba_target_speed;
			roomba_current_radius = roomba_target_radius;
			
			roomba_drive(roomba_target_speed, roomba_target_radius);
		}
		Task_Next();	
	}
}

/*
void b()
{
	for(;;)
		{
			while((UCSR1A & _BV(RXC1)) != 0)
			{
				unsigned char inByte = serial_read_bt();
				serial_write_usb(inByte);
			}
			Task_Next();
		}
}
*/

/*============================================================================*/
void a_main()
{
	setup();
	//Task_Create_Period(Blink, 0, 10, 1, 1);
	//Task_Create_Period(Blink, 0, 10, 1, 1);
	//Task_Create_Period(Transmit, 0, 10, 1, 1);
	//Task_Create_RR(ReadUSB, 0);
	//Task_Create_RR(WriteUSB, 0);
	
	// Creating Tasks
	//Task_Create_Period(receive_transmission, 0, 3, 2, 1);
	//Task_Create_Period(b, 0, 3, 2, 1);
	//Task_Create_Period(update_servos, 0, 2, 1, 2);
	//Task_Create_Period(update_roomba, 0, 10, 1, 3);
	//Task_Create_Period(update_laser, 0, 10, 1, 4);
}

/*============================================================================*/
// Update the positions of the servos.
// ~ 0.08ms average runtime
/*
void updateServos(){
  digitalWrite(update_servo_debug_pin, HIGH);
  panPos  += panSpeed;
  if(panPos>MAXSERVO)panPos = MAXSERVO;
  if(panPos<MINSERVO)panPos = MINSERVO;
  
  tiltPos += tiltSpeed;
  if(tiltPos>MAXSERVO)tiltPos = MAXSERVO;
  if(tiltPos<MINSERVO)tiltPos = MINSERVO;
    
  panServo.writeMicroseconds(panPos);
  tiltServo.writeMicroseconds(tiltPos);
  digitalWrite(update_servo_debug_pin, LOW);
}
*/