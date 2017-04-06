/**
 * \file
 * \brief A simple circular buffer.  Size of buffer is static and can be defined in circular_buffer.h.
 *
 *  Author: Gordon Meyer and Daniel McIlvaney
 */ 

#ifndef CIRCULAR_BUFFER_H_
#define CIRCULAR_BUFFER_H_

#define CIR_BUF_SIZE 22

#include <stdint.h>
#include <stdlib.h>

typedef struct circular_buffer {
	unsigned char buffer[CIR_BUF_SIZE];
	unsigned int size;
	unsigned int next_postion;
	unsigned int start_position;
} circular_buffer;


void Cir_Buf_Add(volatile circular_buffer*, unsigned char byte);
volatile unsigned char Cir_Buf_Read(volatile circular_buffer*);
void Cir_Buf_Init(volatile circular_buffer *cb);

#endif /* CIRCULAR_BUFFER_H_ */