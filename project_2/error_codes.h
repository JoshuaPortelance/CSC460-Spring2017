/*
 * error_codes.h
 *
 * Created: 3/11/2017 12:35:03 PM
 *  Author: Josh
 */

#ifndef _ERROR_CODES_H
#define _ERROR_CODES_H

typedef enum error_code
{
	NO_ERROR = 0,
	DEFUALT_REQUEST,
	MULTIPLE_SENDERS,
	DEFUALT_PRIORITY,
	MILTIPLE_READY_PERIODIC_TASKS,
	OS_START_RETURNED,
	CHAN_NUM_OUT_OF_RANGE,
	CHAN_NOT_INITIALIZED,
	PERIODIC_TASK_CALLING_BLOCKING_FUNCTION
} ERROR_CODE;

#endif /* _ERROR_CODES_H */