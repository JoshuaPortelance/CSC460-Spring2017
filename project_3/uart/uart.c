/*
 * uart.c
 *
 * Created: 4/1/2017 2:39:58 PM
 *  Author: Josh
 */

#define F_CPU 16000000UL
#define BAUD 9600	// This needs to be defined, but is never used directly.
#include "uart.h"
#include "circular_buffer.h"
//#include "circular_buffer.c"
#include <avr/io.h>
#include <util/setbaud.h>
#include <avr/interrupt.h>

#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)

static volatile circular_buffer blue_tooth_rx_buff;
static volatile circular_buffer blue_tooth_tx_buff;
volatile unsigned int blue_tooth_uart_status = 0;

static volatile circular_buffer roomba_rx_buff;
volatile unsigned int roomba_uart_status = 0;

/*============================================================================*/
/*============================================================================*/
void init_uart_usb()
{
	UBRR0H = 0b0000;		// This is for 9600 Baud.
	UBRR0L = 0b01100111;	// This is for 9600 Baud.

	UCSR0A &= ~(_BV(U2X0));

	UCSR0C = _BV(UCSZ01) | _BV(UCSZ00); // 8-bit data
	UCSR0B = _BV(RXEN0) | _BV(TXEN0);   // Enable RX and TX
}

/*============================================================================*/
unsigned char serial_read_usb()
{
	if((UCSR0A & _BV(RXC0)) == 0)		// If data if not available, return null char.
	{
		return '\0';
	}
	return UDR0;
}

/*============================================================================*/
void serial_write_usb(unsigned char data_out)
{
	while ((UCSR0A & _BV(UDRE0)) == 0)		// while NOT ready to transmit
	{;;}
	UDR0 = data_out;
}

/*============================================================================*/
/*============================================================================*/
void init_uart_bt()
{
	UBRR1H = 0b0000;		// This is for 9600 Baud.
	UBRR1L = 0b01100111;	// This is for 9600 Baud.

	// Clear USART Transmit complete flag, normal USART transmission speed
	UCSR1A = (1 << TXC1) | (0 << U2X1);

	// Enable receiver, transmitter, rx complete interrupt and tx complete interrupt.
	UCSR1B = (1 << RXEN1) | (1 << TXEN1) | (1 << RXCIE1) | (1 << TXCIE1);

	// 8-bit data
	UCSR1C = ((1 << UCSZ11) | (1 << UCSZ10));

	// Disable 2x speed
	UCSR1A &= ~(1 << U2X1);

	Cir_Buf_Init(&blue_tooth_rx_buff);
	Cir_Buf_Init(&blue_tooth_tx_buff);

	rx_data_in_blue_tooth_buffer = 0;
}

/*============================================================================*/
unsigned char serial_read_bt()
{
	if (blue_tooth_rx_buff.size == 0)
	{
		return 0;
	}
	else if (blue_tooth_rx_buff.size == 1)
	{
		rx_data_in_blue_tooth_buffer = 0;
	}
	return Cir_Buf_Read(&blue_tooth_rx_buff);
}

/*============================================================================*/
void serial_write_bt(unsigned char data_out)
{
	// Add byte to buffer.
	Cir_Buf_Add(&blue_tooth_tx_buff, data_out);

	// If we are not already transmitting the buffer begin.
	if(!(UART_STATUS_TRANSMITTING & blue_tooth_uart_status))
	{
		// Set status to transmitting.
		blue_tooth_uart_status |= UART_STATUS_TRANSMITTING;

		UDR1 = Cir_Buf_Read(&blue_tooth_tx_buff);
	}
}

/*============================================================================*/
// Interrupt function called for transmit complete.
ISR(USART1_TX_vect)
{
	if(blue_tooth_tx_buff.size > 0)
	{
		UDR1 = Cir_Buf_Read(&blue_tooth_tx_buff);
	}
	else
	{
		blue_tooth_uart_status &= ~(UART_STATUS_TRANSMITTING);
	}
}

/*============================================================================*/
// Interrupt function for data received.
ISR(USART1_RX_vect)
{
	Cir_Buf_Add(&blue_tooth_rx_buff, UDR1);
	rx_data_in_blue_tooth_buffer = 1;
}

/*============================================================================*/
/*============================================================================*/
void init_uart_roomba()
{
	UBRR2H = 0b0000;		// This is for 19200 Baud.
	UBRR2L = 0b00110011;	// This is for 19200 Baud.
	
	// Clear USART Transmit complete flag, normal USART transmission speed
	UCSR2A = (1 << TXC2) | (0 << U2X2);

	// Enable receiver, transmitter, rx complete interrupt and tx complete interrupt.
	UCSR2B = (1 << RXEN2) | (1 << TXEN2) | (1 << RXCIE1);

	// 8-bit data
	UCSR2C = ((1 << UCSZ21) | (1 << UCSZ20));

	// Disable 2x speed
	UCSR2A &= ~(1 << U2X2);

	Cir_Buf_Init(&roomba_rx_buff);

	rx_data_in_roomba_buffer = 0;
}

/*============================================================================*/
unsigned char serial_read_roomba()
{	
	if (roomba_rx_buff.size == 0)
	{
		return 0;
	}
	else if (roomba_rx_buff.size == 1)
	{
		rx_data_in_roomba_buffer = 0;
	}
	return Cir_Buf_Read(&roomba_rx_buff);
}

/*============================================================================*/
void serial_write_roomba(unsigned char data_out)
{
	while ((UCSR2A & _BV(UDRE2)) == 0)		// while NOT ready to transmit
	{;;}
	UDR2 = data_out;
}

/*============================================================================*/
// Interrupt function for data received.
ISR(USART2_RX_vect)
{
	Cir_Buf_Add(&roomba_rx_buff, UDR2);
	rx_data_in_roomba_buffer = 1;
}
