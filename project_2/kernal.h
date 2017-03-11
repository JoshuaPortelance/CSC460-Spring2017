/*
 * Kernel.h
 *
 * Created: 3/10/2017 11:00:55 AM
 *  Author: Josh
 */

#include "os.h"
 
/*
 * Definitions.
 */
#define Disable_Interrupt() asm volatile ("cli"::)
#define Enable_Interrupt()	asm volatile ("sei"::)

/*
 * Type Definitions.
 */
typedef void (*voidfuncptr) (void);	/* pointer to void f(void) */

typedef enum process_states
{
	DEAD = 0,
	READY,
	RUNNING
} PROCESS_STATES;

typedef enum kernel_request_type
{
	NONE = 0,
	ABORT,
	CREATESY,
	CREATEPD,
	CREATERR,
	NEXT,
	GETARG,
	TERMINATE,
	SEND,
	RECV,
	WRITE,
	NOW
} KERNEL_REQUEST_TYPE;

typedef enum process_priorities
{
	SYSTEM,
	PERIODIC,
	ROUNDROBIN
} PROCESS_PRIORITIES;

typedef struct ProcessDescriptor
{
	unsigned char *sp;	/* stack pointer into the "workSpace" */
	unsigned char workSpace[WORKSPACE];
	PROCESS_STATES state;
	voidfuncptr  code;	/* function to be executed as a task */
	KERNEL_REQUEST_TYPE request;
	int creation_arg;
	int pid;
	unsigned int error;
	PROCESS_PRIORITIES priority;
} PD;

/*
 * Structures.
 */