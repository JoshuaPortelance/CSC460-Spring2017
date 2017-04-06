
/*
 * roomba.c
 *
 * Created: 4/3/2017 2:10:03 PM
 *  Author: Josh
 */ 

#define F_CPU 16000000L	    // Specify oscillator frequency
#define CHECK_BIT(var, pos) ((var) & (1 << (pos)))
#include <avr/io.h>
#include <util/delay.h>
#include "roomba.h"
#include "uart.h"

/*============================================================================*/
void init_roomba()
{	
	DDRA  |= 0b00000010;	// Enable pin 23 as output.
	PORTA |= 0b00000010;	// Turn pin 23 to high.
	
	init_uart_roomba();		// Initialize serial 2 to 19200 baud.
	
	//Set baud rate by toggling the brc pin 3 times.
	_delay_ms(2500);
	PORTA &= 0b11111101;	// Turn pin 23 to low
	_delay_ms(300);
	PORTA |= 0b00000010;	// Turn pin 23 to high
	_delay_ms(300);
	PORTA &= 0b11111101;	// Turn pin 23 to low
	_delay_ms(300);
	PORTA |= 0b00000010;	// Turn pin 23 to high
	_delay_ms(300);
	PORTA &= 0b11111101;	// Turn pin 23 to low
	_delay_ms(300);
	PORTA |= 0b00000010;	// Turn pin 23 to high

	init_uart_roomba();		// Initialize serial 2 to 19200 baud.
	
	//Power on
	serial_write_roomba(START);
	
	_delay_ms(200);
	
	//Enter safe mode
	serial_write_roomba(SAFE);
}

/*============================================================================*/
void roomba_drive(int velocity, int radius)
{
	serial_write_roomba(DRIVE);
	serial_write_roomba(HIGH_BYTE(velocity));
	serial_write_roomba(LOW_BYTE(velocity));
	serial_write_roomba(HIGH_BYTE(radius));
	serial_write_roomba(LOW_BYTE(radius));
}

/*============================================================================*/
void roomba_dock() {
	serial_write_roomba(DOCK);
}

/*============================================================================*/
void roomba_power_off() {
	serial_write_roomba(STOP);
}

/*============================================================================*/
unsigned int roomba_detect_physical_wall()
{
	serial_write_roomba(SENSORS);
	serial_write_roomba(7);
	while (rx_data_in_blue_tooth_buffer != 1);	// Wait for the Roomba to send us data.
	unsigned int collision = serial_read_roomba();

	if (collision != 0)
	{
		//serial_write_usb('H');
		return 1;
	}
	return 0;
}

/*============================================================================*/
unsigned int roomba_detect_virtual_wall()
{
	serial_write_roomba(SENSORS);
	serial_write_roomba(13);
	while (rx_data_in_blue_tooth_buffer != 1);	// Wait for the Roomba to send us data.
	unsigned int collision = serial_read_roomba();
	if ((collision != 1) || (collision != 0))
	{
		return 0; // We got something we were not expecting so just return zero.
	}
	//if(collision == 1)
		//serial_write_usb('G');
	return collision;
}