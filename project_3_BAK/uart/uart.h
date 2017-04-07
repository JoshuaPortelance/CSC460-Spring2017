/*
 * uart.h
 *
 * Created: 4/1/2017 2:44:48 PM
 *  Author: Josh
 */
#ifndef _UART_H
#define _UART_H

#ifndef _BV
#define _BV(bit) \ (1 << (bit))
#endif

#ifndef UART_STATUS_TRANSMITTING
#define UART_STATUS_TRANSMITTING 1
#endif

volatile unsigned int rx_data_in_blue_tooth_buffer;
volatile unsigned int rx_data_in_roomba_buffer;

void init_uart_usb(void);
unsigned char serial_read_usb(void);
void serial_write_usb(unsigned char data_out);

void init_uart_bt(void);
unsigned char serial_read_bt(void);
void serial_write_bt(unsigned char data_out);

void init_uart_roomba(void);
unsigned char serial_read_roomba(void);
void serial_write_roomba(unsigned char data_out);

#endif /* _UART_H */