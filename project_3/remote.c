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
#include "servo.h"
#include <avr/io.h>
#include <stdlib.h>

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

		// Cycle through char array.
		unsigned int input_pos = 0;
		
		while(rx_data_in_blue_tooth_buffer == 1)
		{
			unsigned char inByte = serial_read_bt();

			// FOR DEBUG.
			//serial_write_usb(inByte);
			
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
					pan_target_speed     = atoi(panSpeed_val);
					tilt_target_speed    = atoi(tiltSpeed_val);
					laser_target_state	 = atoi(laser_val);
					roomba_target_speed  = atoi(roombaSpeed_val);
					roomba_target_radius = atoi(roombaRadius_val);
					
					// Reset values.
					input_pos = 0;
					packetnum = 0;
					status = 0;
					
					break;
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

/*============================================================================*/
// Update the positions of the servos.
// ~ 0.08ms average runtime
void update_servos()
{
	for(;;)
	{
		if (tilt_target_speed != tilt_current_speed)
		{
			if (tilt_target_speed > MAXSERVO)
			{
				tilt_target_speed = MAXSERVO;
			}
			else if (tilt_target_speed < MINSERVO)
			{
				tilt_target_speed = MINSERVO;
			}
			adjust_tilt_angle(tilt_target_speed - tilt_current_speed);
		}

		if (pan_target_speed != pan_current_speed)
		{
			if (pan_target_speed > MAXSERVO)
			{
				pan_target_speed = MAXSERVO;
			}
			else if (pan_target_speed < MINSERVO)
			{
				pan_target_speed = MINSERVO;
			}
			adjust_pan_angle(pan_target_speed - pan_current_speed);
		}
		Task_Next();
	}
}

/*============================================================================*/
void setup()
{
	// Serial INIT
	init_uart_usb();
	init_uart_bt();
	
	// Roomba INTI
	init_roomba();			// This takes around 5 seconds to complete, so offset other tasks accordingly.
	
	// Laser INIT
	DDRA |= 0b00000001;		// Enable pin 22 as output
	PORTA &= 0b11111110;	// Turn pin 22 to low
	
	// Servo INIT
	init_servos();
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

/*
void servo_testing()
{
	for(;;)
	{
		adjust_pan_angle(0);
		Task_Next();
	}
}
*/

/*
void init_LED(void)
{
	DDRB	= 0b11111111;	//configure pin 7 of PORTB as output (digital pin 13 on the Arduino Mega2560)
	PORTB	= 0b10000000;	//Set port to low
}
*/

/*
void Blink()
{
	//init_LED();
	for(;;)
	{
		PORTB ^= 0b10000000;
		Task_Next();
	}
}
*/

void Transmit()
{
	init_uart_usb();
	for(;;)
	{
		serial_write_usb('H');
		Task_Next();
	}
}

/*
volatile unsigned char x = 'H';
void ReadUSB()
{
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
	for(;;)
	{
		serial_write_usb(x);
		Task_Next();
	}
}
*/

/*
void serial_buffer_test()
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

		// Cycle through char array.
		unsigned int input_pos = 0;

		// FOR SIMULATION
		//rx_data_in_buffer = 1;
		//int cc = 0;
		//unsigned char test[] = "#-5|-5|1|-500|-2000%";
		//volatile unsigned char testVar = test[0];
		// FOR SIMULATION

		while(rx_data_in_buffer == 1)
		{
			//serial_write_usb(serial_read_usb());
			
			unsigned char inByte = serial_read_usb();
			//volatile unsigned char inByte = test[cc++];

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
					pan_target_speed     = atoi(panSpeed_val);
					tilt_target_speed    = atoi(tiltSpeed_val);
					laser_target_state	 = atoi(laser_val);
					roomba_target_speed  = atoi(roombaSpeed_val);
					roomba_target_radius = atoi(roombaRadius_val);

					if (laser_target_state == 1)
					{
						serial_write_usb('1');
					}
					else
					{
						serial_write_usb('0');
					}
					serial_write_usb('\n');
					
					// Reset values.
					input_pos = 0;
					packetnum = 0;
					status = 0;

					break;
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
*/

/*============================================================================*/
void a_main()
{
	/*
		// Start task arguments are:
		// Scheduler_StartTask(start offset in ms, period in ms, function);
		Scheduler_StartTask(0, 30, receiveTransmission);
		Scheduler_StartTask(3, 20, updateServos);
		Scheduler_StartTask(6, 100, updateRoomba);
		Scheduler_StartTask(9, 100, updateLaser);
	*/

	//Task_Create_Period(Blink, 0, 10, 1, 1);
	//Task_Create_Period(Blink, 0, 10, 1, 1);
	Task_Create_Period(Transmit, 2, 10, 1, 500);
	//Task_Create_RR(ReadUSB, 0);
	//Task_Create_RR(WriteUSB, 0);
	//Task_Create_Period(ReadUSB, 0, 6, 1, 1);
	//Task_Create_Period(WriteUSB, 0, 6, 1, 3);
	
	// Creating Tasks
	Task_Create_System(setup, 0);
	Task_Create_Period(receive_transmission, 2, 5, 1, 502);
	//Task_Create_Period(b, 0, 3, 2, 1);
	//Task_Create_Period(update_servos, 0, 2, 1, 2);
	//Task_Create_Period(update_roomba, 0, 10, 1, 3);
	//Task_Create_Period(update_laser, 0, 10, 1, 4);
	//Task_Create_Period(servo_testing, 0, 2, 1, 1);
	//Task_Create_Period(serial_buffer_test, 0, 3, 2, 1);
}