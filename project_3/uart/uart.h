/*
 * uart.h
 *
 * Created: 4/1/2017 2:44:48 PM
 *  Author: Josh
 */

#define _BV(bit) \ (1 << (bit))

void init_uart_usb(void);
unsigned char serial_read_usb(void);
void serial_write_usb(unsigned char DataOut);

void init_uart_bt(void);
unsigned char serial_read_bt(void);
void serial_write_bt(unsigned char DataOut);

void init_uart_roomba(void);
unsigned char serial_read_roomba(void);
void serial_write_roomba(unsigned char DataOut);
