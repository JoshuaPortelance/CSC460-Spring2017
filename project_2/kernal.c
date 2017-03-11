/*
 * Kernel.c
 *
 * Created: 3/10/2017 11:00:39 AM
 *  Author: Josh
 */

/*
 *	Naming scheme:
 *		All functions that are included in kernel.h will be named Kernel_...
 *		All functions that are only used internally will not have the Kernel_... at the start.
 */

#include <avr/io.h>
#include <string.h>
#include "kernel.h"
#include "os.h"
#include "user_space.h"
 
/*
 * Definitions.
 */
#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)

/*
 * Globals.
 */
static PD Process[MAXTHREAD];
volatile static PD* Cp;
volatile unsigned char *KernelSp;
unsigned char *CurrentSp;
volatile static unsigned int NextP;
volatile static unsigned int KernelActive;
volatile static unsigned int Tasks;

/*
 * Function Prototypes.
 */ 
// Kernel functions.
void OS_Init(void);
void OS_Start(void);
void Kernel_OS_Abort(unsigned int error);
void idle(void);
void main_loop(void);
void request_handler(void);
void Dispatch(void);

// Context switching functions.
extern void CSwitch();
extern void Exit_Kernel();    /* this is the same as CSwitch() */
extern void Enter_Kernel();

// Task functions.
PID  Kernel_Task_Create_System(void (*f)(void), int arg);
PID  Kernel_Task_Create_RR(void (*f)(void), int arg);
PID  Kernel_Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset);
void Create_Task_At(PD *p, voidfuncptr f, int arg, PID pid);
void Kernel_Task_Next(void);
int  Kernel_Task_GetArg(void);
void Task_Terminate(void);

// Channel functions.
CHAN Kernel_Chan_Init();
void Kernel_Send(CHAN ch, int v);
int  Kernel_Recv(CHAN ch);
void Kernel_Write(CHAN ch, int v);

// Other functions.
unsigned int Kernel_Now();


void OS_Init(void)
{
	int x;

	Tasks = 0;
	KernelActive = 0;
	NextP = 0;
	//Reminder: Clear the memory for the task on creation.
	for (x = 0; x < MAXTHREAD; x++) {
		memset(&(Process[x]),0,sizeof(PD));
		Process[x].state = DEAD;
	}
}

void OS_Start(void)
{
	if ((! KernelActive) && (Tasks > 0))
	{
		Disable_Interrupt();
		/* we may have to initialize the interrupt vector for Enter_Kernel() here. */

		/* here we go...  */
		KernelActive = 1;
		request_handler();
		/* NEVER RETURNS!!! */
	}
}


/*
 *	Kernel_idle
 *
 *	Idle task.
 *
 *	Params:
 *		void
 *	Return:
 *		void
 */
void idle(void)
{

}


/*
 *	Kernel_main_loop
 *
 *	Main kernel loop
 *
 *	Params:
 *		void
 *	Return:
 *		void
 */
void main_loop(void)
{

}


/*
 *	Kernel_request_handler
 *
 *	Kernel request handler.
 *
 *	Params:
 *		void
 *	Return:
 *		void
 */
void request_handler(void)
{
	Dispatch();  /* select a new task to run */

	while(1)
	{
		Cp->request = NONE; /* Clear its request. */

		/* Activate this newly selected task. */
		CurrentSp = Cp->sp;
		Exit_Kernel();

		/* If this task makes a system call, it will return to here! */

		/* Save CP's stack pointer. */
		Cp->sp = CurrentSp;

		switch(Cp->request)
		{
			case ABORT:
				Kernel_OS_Abort(Cp->error);
			case CREATESY:
				Kernel_Task_Create_System(Cp->code, Cp->creation_arg);
				break;
			case CREATEPD:
			case CREATERR:
			case NEXT:
			case NONE:
				/* NONE could be caused by a timer interrupt */
				Cp->state = READY;
				Dispatch();
				break;
			case TERMINATE:
				/* deallocate all resources used by this task */
				Cp->state = DEAD;
				Dispatch();
				break;
			case GETARG:
			case SEND:
			case RECV:
			case WRITE:
			case NOW:
			default:
				/* Houston! we have a problem here! */
				//Kernel_OS_Abort(Cp->error);	//Cp->error should be changed to a custom error code for this case.
			break;
		}
	}
}


void Dispatch(void)
{
	PORTB	|= 0b00010000;

	/* find the next READY task
	 * Note: if there is no READY task, then this will loop forever!.
	 */
	while(Process[NextP].state != READY)
	{
		NextP = (NextP + 1) % MAXTHREAD;
	}

	Cp = &(Process[NextP]);
	CurrentSp = Cp->sp;
	Cp->state = RUNNING;

	NextP = (NextP + 1) % MAXTHREAD;
	PORTB &= 0b11101111;
}


/*
 *	Kernel_Create_Task_At
 *
 *	Basically just initializes the stack for the new process. This function
 *	is only ever called by the other create functions.
 *
 *	Params:
 *		PD *p			- The process to create.
 *		void (*f)(void) - Pointer to the function the tasks will run.
 *	Return:
 *		void
 */
void Create_Task_At(PD *p, voidfuncptr f, int arg, PID pid)
{
	unsigned char *sp;

	//Changed -2 to -1 to fix off by one error.
	sp = (unsigned char *) &(p->workSpace[WORKSPACE-1]);

	//Clear the contents of the workspace
	memset(&(p->workSpace),0,WORKSPACE);

	//Store terminate at the bottom of stack to protect against stack under run.
	*(unsigned char *)sp-- = ((unsigned int)Task_Terminate) & 0xff;
	*(unsigned char *)sp-- = (((unsigned int)Task_Terminate) >> 8) & 0xff;

	//Place return address of function at bottom of stack
	*(unsigned char *)sp-- = ((unsigned int)f) & 0xff;
	*(unsigned char *)sp-- = (((unsigned int)f) >> 8) & 0xff;

	//	Fixing part of the 17-bit problem.
	*(unsigned char *)sp-- = 0;

	//	Place stack pointer at top of stack.
	sp = sp - 34;

	p->sp = sp;			/* stack pointer into the "workSpace" */
	p->code = f;		/* function to be executed as a task */
	p->request = NONE;
	p->creation_arg = arg;
	p->pid = pid;

	p->state = READY;
}


/*
 *	Kernel_OS_Abort
 *
 *	Aborts the RTOS and enters a "non-executing" state with an error code.
 *
 *	Params:
 *		unsigned int error - The error code related to the abort.
 *	Return:
 *		void
 */
void Kernel_OS_Abort(unsigned int error)
{
	for(;;)
	{
		//display error code.
	}
}


/*
 *	Kernel_Task_Create_System
 *
 *	Creates a task with System level priority.
 *
 *	Params:
 *		void (*f)(void) - Pointer to the function the tasks will run.
 *		int arg			- An integer number to be assigned to this process instance.
 *	Return:
 *		PID				- Zero if unsuccessful, otherwise a positive integer.
 */
PID Kernel_Task_Create_System(void (*f)(void), int arg)
{
	//PID pid = 1;
	//return pid;
	int x;

	if (Tasks == MAXTHREAD)
	{
		return 0;  /* Too many task! */
	}

	/* find a DEAD PD that we can use  */
	for (x = 0; x < MAXTHREAD; x++) {
		if (Process[x].state == DEAD) break;
	}

	++Tasks;
	Create_Task_At(&(Process[x]), f, arg, x);
	return x;
}


/*
 *	Kernel_Task_Create_RR
 *
 *	Creates a task with RR level priority.
 *
 *	Params:
 *		void (*f)(void) - Pointer to the function the tasks will run.
 *		int arg			- An integer number to be assigned to this process instance.
 *	Return:
 *		PID				- Zero if unsuccessful, otherwise a positive integer.
 */
PID Kernel_Task_Create_RR(void (*f)(void), int arg)
{
	PID pid = 1;
	return pid;
}


/*
 *	Kernel_Task_Create_Period
 *
 *	Creates a periodic task with the periodic priority level.
 *
 *	Params:
 *		void (*f)(void) - Pointer to the function the tasks will run.
 *		int arg			- An integer number to be assigned to this process instance.
 *		TICK period		- Tasks execution period in TICKs.
 *		TICK wcet		- Worst case execution time of the task in TICKs; must be less then period.
 *		TICK offset		- Tasks start offset in TICKs.
 *	Return:
 *		PID				- Zero if unsuccessful, otherwise a positive integer.
 */
PID Kernel_Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset)
{
	PID pid = 1;
	return pid;
}


/*
 *	Kernel_Task_Next
 *
 *	Switches to the next highest priority task.
 *
 *	Params:
 *		void
 *	Return:
 *		void
 */
void Kernel_Task_Next(void)
{

}


/*
 *	Kernel_Task_GetArg
 *
 *	Gets the integer that was assigned to the task when created.
 *
 *	Params:
 *		void
 *	Return:
 *		int - The arg that was used when creating the task.
 */
int Kernel_Task_GetArg(void)
{
	return 1;
}

/*
 *	Kernel_Task_Terminate
 *
 *	Terminates the calling task.
 *
 *	Params:
 *		void
 *	Return:
 *		void
 */
void Task_Terminate()
{
   if (KernelActive) {
      Disable_Interrupt();
      Cp -> request = TERMINATE;
      Enter_Kernel();
     /* never returns here! */
   }
}


/*
 *	Kernel_Chan_Init
 *
 *	Initializes a channel.
 *
 *	Params:
 *		void
 *	Return:
 *		CHAN - An initialized channel if successful, otherwise NULL.
 */
CHAN Kernel_Chan_Init()
{
	CHAN ch = 1;
	return ch;
}


/*
 *	Kernel_Send
 *
 *	Sends the message v over channel ch. 
 *		- If there are no receivers the calling task is blocked.
 *		- Else if there is already a sender on this channel the RTOS will abort in an error state.
 *		- Else the message will be sent over the channel and the task will not be blocked.
 *
 *	Params:
 *		CHAN ch	- Channel to send the message over.
 *		int v	- Message to send over the channel.
 *	Return:
 *		void
 */
void Kernel_Send(CHAN ch, int v)
{

}


/*
 *	Kernel_Recv
 *
 *	Receives a message over channel ch.
 *		- If there is no sender on the channel the calling task will be blocked.
 *		- Else the message will be received and the calling task will not be blocked
 *
 *	Params:
 *		CHAN ch	- The channel to receive a message over.
 *	Return:
 *		int		- The message that was received.
 */
int Kernel_Recv(CHAN ch)
{
	return 1;
}


/*
 *	Kernel_Write
 *
 *	Sends the message v over channel ch, but do not wait if there are no receivers.
 *		- If there is already a sender on the channel the RTOS will abort in an error state.
 *		- Else send the message over the channel; the calling task will not be blocked.
 *
 *	Params:
 *		CHAN ch	- Channel to send the message over.
 *		int v	- Message to send over the channel.
 *	Return:
 *		void
 */
void Kernel_Write(CHAN ch, int v)
{

}


/*
 *	Kernel_Now
 *
 *	Returns the number of milliseconds since OS_Init().
 *
 *	Params:
 *		void
 *	Return:
 *		unsigned int - The number of milliseconds since OS_Init().
 */
unsigned int Kernel_Now()
{
	return 1;
}


int main(void)
{
	OS_Init();

	unsigned int err = main_a();
	if (err != 0)
	{
		Kernel_OS_Abort(err);
	}

	//go to kernel main loop
	return 1;
}