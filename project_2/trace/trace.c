/*
 * trace.c
 *
 * Created: 3/15/2017 9:11:25 AM
 *  Author: Josh
 *
 * Based off of Andrey Polyakov and Jared Griffis
 * project 3 from 2013.
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "trace.h"
#include "os.h"

trace_entry trace[MAX_TRACE_LENGTH];
char str_trace[MAX_TRACE_LENGTH * 3 + 1];
int volatile trace_counter = 0;

char * get_trace() {
	int i;
	int char_count = 0;
	for (i = 0; i < trace_counter; i++) {
		if (trace[i].event == ENTER) {
			//			str_trace[3 * i] = '(';
			//			str_trace[3 * i + 1] = 48 + trace[i].process_number;
			//			str_trace[3 * i + 2] = ',';
			char_count += sprintf(str_trace + char_count, "(%d,", trace[i].process_number);
			} else {
			//			str_trace[3 * i] = 48 + trace[i].process_number;
			//			str_trace[3 * i + 1] = ')';
			//			str_trace[3 * i + 2] = ',';
			char_count += sprintf(str_trace + char_count, "%d),", trace[i].process_number);
		}
	}
	str_trace[3 * i] = '\0';
	return str_trace;
}

void add_to_trace(unsigned int process_number, trace_event event)
{
    if (trace_counter < MAX_TRACE_LENGTH)
    {
        trace[trace_counter].process_number = process_number;
        trace[trace_counter].event = event;
        trace_counter++;
    }
    else
    {
        OS_Abort(10);
    }
}