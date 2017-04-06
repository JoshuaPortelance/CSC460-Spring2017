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
		
	if(rx_data_in_blue_tooth_buffer == 1)
	{
		unsigned char inByte = serial_read_bt();

		// FOR DEBUG.
		serial_write_usb(inByte);

		if (inByte == '#')
		{
			status = 1;
			while(status == 1)
			{
				if(rx_data_in_blue_tooth_buffer == 1)
				{
					inByte = serial_read_bt();
					
					// FOR DEBUG.
					serial_write_usb(inByte);
					
					if (inByte == '%')
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
						pan_speed     = atoi(panSpeed_val);
						tilt_speed    = atoi(tiltSpeed_val);
						laser_target_state	 = atoi(laser_val);
						roomba_target_speed  = atoi(roombaSpeed_val);
						roomba_target_radius = atoi(roombaRadius_val);
			
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
		}
	}
}

/*============================================================================*/
// Update the state of the laser.
// ~ 0.05ms average runtime
void update_laser()
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
	if (roomba_current_speed != roomba_target_speed || roomba_current_radius != roomba_target_radius)
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
}

/*============================================================================*/
// Update the positions of the servos.
// ~ 0.08ms average runtime
void update_servos()
{
	adjust_tilt_angle(tilt_speed);
	adjust_pan_angle(pan_speed);
}

/*============================================================================*/
void update()
{
	update_laser();
	update_servos();
	//servo_testing();
	update_roomba();
	
	// PAN/TILT Servo Variables.
	pan_speed   = 0;
	tilt_speed  = 0;

	// Roomba Variables.
	//roomba_target_speed   = 0;
	//roomba_target_radius  = 0;
	//roomba_current_speed  = 0;
	//roomba_current_radius = 0;

	// Laser Init.
	//laser_state         = 0;    //0 is off, 1 is on.
	//laser_target_state  = 0;	//0 is off, 1 is on.
}

void remote()
{
	for(;;)
	{
		receive_transmission();
		update();
		Task_Next();	
	}
}

/*============================================================================*/
void setup()
{
	// Serial INIT
	init_uart_usb();
	init_uart_bt();
	init_uart_roomba();
	
	// Roomba INTI
	init_roomba();			// This takes around 5 seconds to complete, so offset other tasks accordingly.
	
	// Laser INIT
	DDRA |= 0b00000001;		// Enable pin 22 as output
	PORTA &= 0b11111110;	// Turn pin 22 to low
	
	// Servo INIT
	init_servos();
}

/*============================================================================*/
void a_main()
{
	//Task_Create_Period(Blink, 0, 10, 1, 1);
	//Task_Create_Period(Blink, 0, 10, 1, 1);
	//Task_Create_Period(Transmit, 2, 10, 1, 500);
	//Task_Create_RR(ReadUSB, 0);
	//Task_Create_RR(WriteUSB, 0);
	//Task_Create_Period(ReadUSB, 0, 6, 1, 1);
	//Task_Create_Period(WriteUSB, 0, 6, 1, 3);
	
	// Creating Tasks
	Task_Create_System(setup, 0);
	//Task_Create_Period(remote, 1, 3, 4, 505);
	Task_Create_System(remote, 1);
	//Task_Create_Period(receive_transmission, 1, 6, 2, 500);
	//Task_Create_Period(update, 2, 6, 2, 503);
	
	//Task_Create_Period(b, 0, 3, 2, 1);
	//Task_Create_Period(update_servos, 0, 10, 1, 500);
	//Task_Create_Period(update_roomba, 0, 10, 1, 3);
	//Task_Create_Period(update_laser, 0, 10, 1, 4);
	//Task_Create_Period(servo_testing, 0, 2, 1, 502);
	//Task_Create_Period(serial_buffer_test, 0, 3, 2, 1);
}