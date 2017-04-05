/**
 * \file
 * \brief A simple circular buffer.  Size of buffer is static and can be defined in circular_buffer.h.
 *
 *  Author: Gordon Meyer and Daniel McIlvaney
 */ 

#include "circular_buffer.h"
#include <avr/interrupt.h>

/** Initialize values in a Circular Buffer 
	@param	cb		Address of the circular buffer to initialize.
*/
inline void Cir_Buf_Init(volatile circular_buffer *cb)
{
	cb->size = cb->next_postion = cb->start_position = 0;
}

/** Adds a byte to the buffer.  If buffer full the oldest byte will be replaced 
	@param	cb		Address of the circular buffer
	@param	byte	Byte to add to buffer
*/
void Cir_Buf_Add(volatile circular_buffer *cb, unsigned char byte)
{	
	*(cb->buffer + cb->next_postion) = byte;
	
	// Wrap around
	if(++cb->next_postion >= CIR_BUF_SIZE) cb->next_postion = 0;
	
	// Update size or start position
	if(cb->size < CIR_BUF_SIZE) cb->size++;
	else cb->start_position++;
	
}

/** Read a byte from the buffer.  Internal pointer will be updated. 
	@param	cb		Address of the circular buffer
	@return			Oldest byte in the Buffer
*/
volatile unsigned char Cir_Buf_Read(volatile circular_buffer* cb)
{	
	volatile unsigned char *byte = NULL;
	
	if(cb->size == 0) return 0;
	byte = cb->buffer + cb->start_position++;
	if(cb->start_position >= CIR_BUF_SIZE) cb->start_position = 0;
	cb->size--;
	
	return *byte;
}