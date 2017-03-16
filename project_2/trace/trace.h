/*
 * trace.h
 *
 * Created: 3/15/2017 9:11:25 AM
 *  Author: Josh
 *
 * Based off of Andrey Polyakov and Jared Griffis
 * project 3 from 2013.
 */

#ifndef __TRACE_H__
#define __TRACE_H__

#define MAX_TRACE_LENGTH 64

#define TEST_PASS 0   //Number of blinks that will be sent to OS_Abort.
#define TEST_FAIL 11  //Number of blinks that will be sent to OS_Abort.

typedef enum {
	ENTER = 0, EXIT
} trace_event;

typedef struct trace_entry {
	unsigned int process_number;
	trace_event event;
} trace_entry;

void add_to_trace(unsigned int process_number, trace_event event);
char * get_trace();

#endif