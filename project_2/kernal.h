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
} PD;

/*
 * Structures.
 */

/*
 * Functions.
 */
void Kernel_OS_Abort(unsigned int error);
// void exit_kernel(void);
// void enter_kernel(void);

PID  Kernel_Task_Create_System(void (*f)(void), int arg);
PID  Kernel_Task_Create_RR(void (*f)(void), int arg);
PID  Kernel_Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset);
void Kernel_Task_Next(void);
int  Kernel_Task_GetArg(void);

CHAN Kernel_Chan_Init();
void Kernel_Send(CHAN ch, int v);
int  Kernel_Recv(CHAN ch);
void Kernel_Write(CHAN ch, int v);

unsigned int Kernel_Now();