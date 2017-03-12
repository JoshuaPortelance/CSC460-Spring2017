/*
 *	os.c
 *
 *	Created: 3/10/2017 10:26:07 AM
 *	Author: Josh
 */

#define F_CPU 16000000L	// Specify oscillator frequency
#include <util/delay.h>
#include <avr/io.h>
#include <string.h>
#include <avr/interrupt.h>
#include "os.h"
#include "kernel.h"
#include "error_codes.h"
#include "user_space.h"

/*
 * Globals.
 */
static PD Process[MAXTHREAD];

static PD* system_queue[MAXTHREAD];
volatile static unsigned int system_queue_front = 0;
volatile static unsigned int system_queue_rear = -1;
volatile static unsigned int system_queue_size = 0;

static PD* periodic_queue[MAXTHREAD];
volatile static unsigned int periodic_queue_front = 0;
volatile static unsigned int periodic_queue_rear = -1;
volatile static unsigned int periodic_queue_size = 0;

static PD* rr_queue[MAXTHREAD];
volatile static unsigned int rr_queue_front = 0;
volatile static unsigned int rr_queue_rear = -1;
volatile static unsigned int rr_queue_size = 0;

static PD* periodic_lookup[MAXTHREAD];

volatile static PD* Cp;
volatile unsigned char *KernelSp;
unsigned char *CurrentSp;
volatile static unsigned int NextP;
volatile static unsigned int KernelActive;

volatile static unsigned int Tasks;
volatile static unsigned int Periodic_Tasks;
volatile static unsigned int System_Tasks;
volatile static unsigned int RR_Tasks;

volatile static unsigned int current_time;	//	The current time in milliseconds since Kernel_OS_Init.

/*
 * API Function Prototypes.
 */
	void OS_Abort(unsigned int error);
	PID Task_Create_System(void (*f)(void), int arg);
	PID Task_Create_RR(void (*f)(void), int arg);
	PID Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset);
	void Task_Next(void);
	int Task_GetArg(void);
	CHAN Chan_Init();
	void Send(CHAN ch, int v);
	int Recv(CHAN ch);
	void Write(CHAN ch, int v);
	unsigned int Now();

/*
 * Kernel Function Prototypes.
 */
	// Context switching functions.
	extern void CSwitch();
	extern void Exit_Kernel();    /* this is the same as CSwitch() */
	extern void Enter_Kernel();

	// Kernel functions.
	void Kernel_OS_Init(void);
	void Kernel_OS_Start(void);
	void Kernel_Idle(void);
	void Kernel_OS_Abort(unsigned int error);
	void Kernel_Request_Handler(void);
	void Kernel_Dispatch(void);

	// Task functions.
	PID  Kernel_Task_Create_System(void (*f)(void), int arg);
	PID  Kernel_Task_Create_Round_Robin(void (*f)(void), int arg);
	PID  Kernel_Task_Create_Periodic(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset);
	void Kernel_Create_Task_At(PD *p, voidfuncptr f, int arg, PID pid, PROCESS_PRIORITIES priority, TICK period, TICK wcet, TICK offset);
	void Kernel_Task_Terminate(void);

	// Channel functions.
	CHAN Kernel_Chan_Init();
	void Kernel_Send(CHAN ch, int v);
	int  Kernel_Recv(CHAN ch);
	void Kernel_Write(CHAN ch, int v);

	// Timing functions.
	unsigned int Kernel_Now();

	// Queue functions.
	void enqueue_system(PD* p);
	PD* dequeue_system(void);
	void enqueue_periodic(PD* p);
	PD* dequeue_periodic(void);
	void enqueue_rr(PD* p);
	PD* dequeue_rr(void);

/*
 * API Functions.
 * ================================================================================================
 * ================================================================================================
 * ================================================================================================
 */
/*
 *	OS_Abort
 *
 *	Aborts the RTOS and enters a "non-executing" state with an error code.
 *
 *	Params:
 *		unsigned int error - The error code related to the abort.
 */
void OS_Abort(unsigned int error)
{
	if (KernelActive)
	{
		Disable_Interrupt();
		Cp->request = ABORT;
		Cp->error = error;
		Enter_Kernel();
	}
	else
	{
		Kernel_OS_Abort(error);
	}
}

/*
 *	Task_Create_System
 *
 *	Creates a task with System level priority.
 *
 *	Params:
 *		void (*f)(void) - Pointer to the function the tasks will run.
 *		int arg			- An integer number to be assigned to this process instance.
 *	Return:
 *		PID				- Zero if unsuccessful, otherwise a positive integer.
 */
PID Task_Create_System(void (*f)(void), int arg)
{
	if (KernelActive)
	{
		Disable_Interrupt();
		Cp->request = CREATESY;
		Cp->priority = SYSTEM;
		Cp->code = f;
		Cp->creation_arg = arg;
		Enter_Kernel();
	}
	else
	{
		/* call the RTOS function directly */
		Kernel_Task_Create_System(f, arg);
	}
	return Cp->pid;
}

/*
 *	Task_Create_RR
 *
 *	Creates a task with RR level priority.
 *
 *	Params:
 *		void (*f)(void) - Pointer to the function the tasks will run.
 *		int arg			- An integer number to be assigned to this process instance.
 *	Return:
 *		PID				- Zero if unsuccessful, otherwise a positive integer.
 */
PID Task_Create_RR(void (*f)(void), int arg)
{
	if (KernelActive)
	{
		Disable_Interrupt();
		Cp->request = CREATERR;
		Cp->priority = ROUNDROBIN;
		Cp->code = f;
		Cp->creation_arg = arg;
		Enter_Kernel();
	}
	else
	{
		/* call the RTOS function directly */
		Kernel_Task_Create_Round_Robin(f, arg);
	}
	return Cp->pid;
}

/*
 *	Task_Create_Period
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
PID Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset)
{
	if (KernelActive)
	{
		Disable_Interrupt();
		Cp->request = CREATEPD;
		Cp->priority = PERIODIC;
		Cp->code = f;
		Cp->creation_arg = arg;
		Cp->period = period;
		Cp->wcet = wcet;
		Cp->offset = offset;
		Enter_Kernel();
	}
	else
	{
		/* call the RTOS function directly */
		Kernel_Task_Create_Periodic(f, arg, period, wcet, offset);
	}
	return Cp->pid;
}

/*
 *	Task_Next
 *
 *	Switches to the next highest priority task.
 */
void Task_Next(void)
{
	if (KernelActive)
	{
		Disable_Interrupt();
		Cp ->request = NEXT;
		Enter_Kernel();
	}
}

/*
 *	Task_GetArg
 *
 *	Gets the integer that was assigned to the task when created.
 *
 *	Return:
 *		int - The arg that was used when creating the task.
 */
int Task_GetArg(void)
{
	return Cp->creation_arg;
}

/*
 *	Chan_Init
 *
 *	Initializes a channel.
 *
 *	Return:
 *		CHAN - An initialized channel if successful, otherwise NULL.
 */
CHAN Chan_Init()
{
	CHAN ch = 1;
	return ch;
}

/*
 *	Send
 *
 *	Sends the message v over channel ch. 
 *		- If there are no receivers the calling task is blocked.
 *		- Else if there is already a sender on this channel the RTOS will abort in an error state.
 *		- Else the message will be sent over the channel and the task will not be blocked.
 *
 *	Params:
 *		CHAN ch	- Channel to send the message over.
 *		int v	- Message to send over the channel.
 */
void Send(CHAN ch, int v)
{

}

/*
 *	Recv
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
int Recv(CHAN ch)
{
	return 1;
}

/*
 *	Write
 *
 *	Sends the message v over channel ch, but do not wait if there are no receivers.
 *		- If there is already a sender on the channel the RTOS will abort in an error state.
 *		- Else send the message over the channel; the calling task will not be blocked.
 *
 *	Params:
 *		CHAN ch	- Channel to send the message over.
 *		int v	- Message to send over the channel.
 */
void Write(CHAN ch, int v)
{

}

/*
 *	Now
 *
 *	Returns the number of milliseconds since Kernel_OS_Init().
 *
 *	Return:
 *		unsigned int - The number of milliseconds since Kernel_OS_Init().
 */
unsigned int Now()
{
	return current_time;
}

/*
 * Kernel Functions.
 * ================================================================================================
 * ================================================================================================
 * ================================================================================================
 */
 /*
 *	Kernel_OS_Init
 *
 *	Initializes the data structure and globals needed for execution.
 */
void Kernel_OS_Init(void)
{
	int x;

	Tasks = 0;
	KernelActive = 0;
	NextP = 0;
	Periodic_Tasks = 0;
	System_Tasks = 0;
	RR_Tasks = 0;
	current_time = 0;

	//Reminder: Clear the memory for the task on creation.
	for (x = 0; x < MAXTHREAD; x++) {
		memset(&(Process[x]),0,sizeof(PD));
		Process[x].state = DEAD;
	}

	/* Initialize timer ISR for 1kHz */
	Disable_Interrupt();
	TCCR3A = 0;
	TCCR3B = 0;
	TCNT3 = 0;				//Set timer to 0
	TCCR3B |= (1<<WGM12);	//Set to CTC (mode 4)
	TCCR3B |= (1<<CS12);	//Set to 256 prescaler
	OCR3A = 62500;			//Setting timer target
	TIMSK3 |= (1<<OCIE3A);	//Enable interrupt A for timer 3.

	/* Initialize priority task scheduler ISR for 100Hz */
	TCCR4A = 0;
	TCCR4B = 0;
	TCNT4 = 0;				//Set timer to 0
	TCCR4B |= (1<<WGM12);	//Set to CTC (mode 4)
	TCCR4B |= (1<<CS12);	//Set to 256 prescaler
	OCR4A = 625;			//Setting timer target
	TIMSK4 |= (1<<OCIE4A);	//Enable interrupt A for timer 4.
	Enable_Interrupt();
}

/*
 *	ISR to:
 *		- keep track of milliseconds sense Kernel_OS_Init.
 *		- monitoring for preemption and acting on preemption.
 *		- 
 */
ISR(TIMER3_COMPA_vect)
{
	int preempted = 0;
	current_time++; //Update timer.

	//Check and act on preemption.
	if(Cp->priority == ROUNDROBIN && (System_Tasks > 0 || Periodic_Tasks > 0))
	{
		Cp->state = READY;
		preempted = 1;
	}
	else if(Cp->priority == PERIODIC && System_Tasks > 0)
	{
		Cp->state = READY;
		preempted = 1;
	}

	//End any RR going over 1 TICK.
	if(Cp->priority == ROUNDROBIN && (Cp->start_time - current_time) >= MSECPERTICK)
	{
		Cp->state = READY;
		preempted = 1;
	}

	//Reschedule if preempted.
	if(preempted == 1)
	{
		switch(Cp->priority)
		{
			case SYSTEM:
				enqueue_system(Cp);
				break;
			case PERIODIC:
				enqueue_periodic(Cp);
				break;
			case ROUNDROBIN:
				enqueue_rr(Cp);
				break;
			default:
				Kernel_OS_Abort(DEFUALT_PRIORITY);
				break;
		}
		Task_Next();
	}
}

/*
 *	ISR to check for periodic functions that should be set to ready.
 *	Also checks if a currently running task should be preempted.
 */
ISR(TIMER4_COMPA_vect)
{
	int i;
	for(i = 0; i < Periodic_Tasks; i++)
	{
		if(periodic_lookup[i]->num_runs == 0)
		{
			if((current_time - (periodic_lookup[i]->offset * 10)) > 0)
			{
				periodic_lookup[i]->state = READY;
				enqueue_periodic(periodic_lookup[i]);
			}
		}
		else
		{
			if((periodic_lookup[i]->start_time - current_time) >= (periodic_lookup[i]->period * 10))
			{
				periodic_lookup[i]->state = READY;
				enqueue_periodic(periodic_lookup[i]);
			}
		}
	}
}

/*
 *	Kernel_OS_Start
 *
 *	Starts the RTOS.
 */
void Kernel_OS_Start(void)
{
	if ((! KernelActive) && (Tasks > 0))
	{
		Disable_Interrupt();
		/* we may have to initialize the interrupt vector for Enter_Kernel() here. */

		/* here we go...  */
		KernelActive = 1;
		Kernel_Request_Handler();
		/* NEVER RETURNS!!! */
	}
}

/*
 *	Kernel_OS_Abort
 *
 *	Aborts the RTOS and enters a "non-executing" state with an error code.
 *	This is done by entering an infinite loop that blinks the on-board LED
 *	to try to communicate the error. The number of blinks in a cycle is 
 *	related to the enum value defined in error_codes.h.
 *
 *	Params:
 *		unsigned int error - The error code related to the abort.
 */
void Kernel_OS_Abort(unsigned int error)
{
	//	Initializing the on-board LED.
	DDRB	= 0b11111111;
	PORTB	= 0b00000000;
	unsigned int i;
	for(;;)
	{
		PORTB	&= 0b01111111;		//	Turn LED off.
		_delay_ms(5000);
		for(i=0; i <= error; i++)
		{
			PORTB	^= 0b10000000;	//	Toggle LED.
			_delay_ms(250);
		}
	}
}

/*
 *	Kernel_Idle
 *
 *	Idle task.
 */
void Kernel_Idle(void)
{
	for (;;){};
}

/*
 *	Kernel_Request_Handler
 *
 *	Handles all system requests. This function is only called once by
 *	Kernel_OS_Start and never terminates.
 */
void Kernel_Request_Handler(void)
{
	Kernel_Dispatch();  /* select a new task to run */

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
				break;
			case CREATESY:
				Kernel_Task_Create_System(Cp->code, Cp->creation_arg);
				break;
			case CREATEPD:
				Kernel_Task_Create_Periodic(Cp->code, Cp->creation_arg, Cp->period, Cp->wcet, Cp->offset);
				break;
			case CREATERR:
				Kernel_Task_Create_Round_Robin(Cp->code, Cp->creation_arg);
				break;
			case NEXT:
			case NONE:
				/* NONE could be caused by a timer interrupt */
				Cp->state = READY;
				switch(Cp->priority)
				{
					case SYSTEM:
						enqueue_system(Cp);
						break;
					case PERIODIC:
						enqueue_periodic(Cp);
						break;
					case ROUNDROBIN:
						enqueue_rr(Cp);
						break;
					default:
						Kernel_OS_Abort(DEFUALT_PRIORITY);
						break;
				}
				Kernel_Dispatch();
				break;
			case TERMINATE:
				/* deallocate all resources used by this task */
				Cp->state = DEAD;
				Kernel_Dispatch();
				break;
			case SEND:
			case RECV:
			case WRITE:
			default:
				/* Houston! we have a problem here! */
				Cp->error = DEFUALT_REQUEST;
				Kernel_OS_Abort(Cp->error);
			break;
		}
	}
}

/*
 *	Kernel_Dispatch
 *
 *	Determines which task will run next. This is our scheduler.
 */
void Kernel_Dispatch(void)
{
	/* find the next READY task
	 * Note: if there is no READY task, then this will loop forever!.
	 */
	/*
	while(Process[NextP].state != READY)
	{
		NextP = (NextP + 1) % MAXTHREAD;
	}
	*/
	if(periodic_queue_size > 1)
	{
		Kernel_OS_Abort(MILTIPLE_READY_PERIODIC_TASKS);
	}

	if(system_queue_size > 0)
	{
		Cp = dequeue_system();
	}
	else if(periodic_queue_size > 0)
	{
		Cp = dequeue_periodic();
	}
	else if(rr_queue_size > 0)
	{
		Cp = dequeue_rr();
	}
	/*
	else
	{
		Kernel_Idle();
	}
	*/

	//Cp = &(Process[NextP]);
	CurrentSp = Cp->sp;
	Cp->state = RUNNING;
	Cp->start_time = Now();
	Cp->num_runs++;

	NextP = (NextP + 1) % MAXTHREAD;
}

/*
 *	Kernel_Create_Task_At
 *
 *	Basically just initializes the stack for the new process. This function
 *	is only ever called by the other create functions.
 *
 *	Params:
 *		PD *p						- The process to create.
 *		void (*f)(void)				- Pointer to the function the tasks will run.
 *		PID pid						- The pid to give to the function.
 *		PROCESS_PRIORITIES priority	- The priority of the new task.
 *		TICK period					- The period of the new task. If periodic priority, else 0.
 *		TICK wcet					- The worse case running time of the task. If periodic priority, else 0.
 *		TICK offset					- The offset of the new task. If periodic priority, else 0.
 */
void Kernel_Create_Task_At(PD *p, voidfuncptr f, int arg, PID pid, PROCESS_PRIORITIES priority, TICK period, TICK wcet, TICK offset)
{
	unsigned char *sp;

	//Changed -2 to -1 to fix off by one error.
	sp = (unsigned char *) &(p->workSpace[WORKSPACE-1]);

	//Clear the contents of the workspace
	memset(&(p->workSpace),0,WORKSPACE);

	//Store terminate at the bottom of stack to protect against stack under run.
	*(unsigned char *)sp-- = ((unsigned int)Kernel_Task_Terminate) & 0xff;
	*(unsigned char *)sp-- = (((unsigned int)Kernel_Task_Terminate) >> 8) & 0xff;

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
	p->priority = priority;
	p->period = period;
	p->wcet = wcet;
	p->offset =offset;
	p->state = READY;
	p->start_time = 0;
	p->num_runs = 0;
	switch(p->priority)
	{
		case SYSTEM:
			enqueue_system(p);
			break;
		case PERIODIC:
			periodic_lookup[Periodic_Tasks++] = p;
			enqueue_periodic(p);
			break;
		case ROUNDROBIN:
			enqueue_rr(p);
			break;
		default:
			Kernel_OS_Abort(DEFUALT_PRIORITY);
			break;
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
	int x;

	if (Tasks == MAXTHREAD)
	{
		return 0;  /* Too many task! */
	}

	/* find a DEAD PD that we can use  */
	for (x = 0; x < MAXTHREAD; x++)
	{
		if (Process[x].state == DEAD) break;
	}

	++Tasks;
	++System_Tasks;
	Kernel_Create_Task_At(&(Process[x]), f, arg, x, SYSTEM, 0, 0, 0);
	return x;
}

/*
 *	Kernel_Task_Create_Round_Robin
 *
 *	Creates a task with RR level priority.
 *
 *	Params:
 *		void (*f)(void) - Pointer to the function the tasks will run.
 *		int arg			- An integer number to be assigned to this process instance.
 *	Return:
 *		PID				- Zero if unsuccessful, otherwise a positive integer.
 */
PID Kernel_Task_Create_Round_Robin(void (*f)(void), int arg)
{
	int x;

	if (Tasks == MAXTHREAD)
	{
		return 0;  /* Too many task! */
	}

	/* find a DEAD PD that we can use */
	for (x = 0; x < MAXTHREAD; x++)
	{
		if (Process[x].state == DEAD) break;
	}

	++Tasks;
	++RR_Tasks;
	Kernel_Create_Task_At(&(Process[x]), f, arg, x, ROUNDROBIN, 0, 0, 0);
	return x;
}

/*
 *	Kernel_Task_Create_Periodic
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
PID Kernel_Task_Create_Periodic(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset)
{
	int x;

	if (Tasks == MAXTHREAD)
	{
		return 0;  /* Too many task! */
	}

	/* find a DEAD PD that we can use */
	for (x = 0; x < MAXTHREAD; x++)
	{
		if (Process[x].state == DEAD) break;
	}

	++Tasks;
	Kernel_Create_Task_At(&(Process[x]), f, arg, x, PERIODIC, period, wcet, offset);
	return x;
}

/*
 *	Kernel_Task_Terminate
 *
 *	Terminates the calling task.
 */
void Kernel_Task_Terminate()
{
	if (KernelActive)
	{
		Disable_Interrupt();
		Cp -> request = TERMINATE;
		switch(Cp->priority)
		{
			case SYSTEM:
				System_Tasks--;
				break;
			case PERIODIC:
				Periodic_Tasks--;
				break;
			case ROUNDROBIN:
				RR_Tasks--;
				break;
			default:
				Kernel_OS_Abort(DEFUALT_PRIORITY);
				break;
	  }
	  Tasks--;
      Enter_Kernel();
     /* never returns here! */
   }
}

/*
 *	Kernel_Chan_Init
 *
 *	Initializes a channel.
 *
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
 */
void Kernel_Write(CHAN ch, int v)
{

}

/*
 *	enqueue_system
 *
 *	Enqueues the PD of a ready system task into the system ready queue.
 *
 *	Params:
 *		PD* p - Pointer to the tasks process descriptor.
 */
void enqueue_system(PD* p)
{
	if(system_queue_size < MAXTHREAD)
	{
		if(system_queue_rear == MAXTHREAD - 1)
		{
			system_queue_rear = -1;
		}

		system_queue[++system_queue_rear] = p;
		system_queue_size++;
	}
}

/*
 *	dequeue_system
 *
 *	Dequeues the PD of the task that is going to be ran form the system ready queue.
 *
 *	Return:
 *		PD* p - Pointer to the tasks process descriptor that was removed.
 */
PD* dequeue_system(void)
{
	PD* p = system_queue[system_queue_front++];

	if(system_queue_front == MAXTHREAD)
	{
		system_queue_front = 0;
	}

	system_queue_size--;
	return p;
}

/*
 *	enqueue_periodic
 *
 *	Enqueues the PD of a ready periodic task into the periodic ready queue.
 *
 *	Params:
 *		PD* p - Pointer to the tasks process descriptor.
 */
void enqueue_periodic(PD* p)
{
	if(periodic_queue_size < MAXTHREAD)
	{
		if(periodic_queue_rear == MAXTHREAD - 1)
		{
			periodic_queue_rear = -1;
		}

		periodic_queue[++periodic_queue_rear] = p;
		periodic_queue_size++;
	}
}

/*
 *	dequeue_periodic
 *
 *	Dequeues the PD of the task that is going to be ran form the periodic ready queue.
 *
 *	Return:
 *		PD* p - Pointer to the tasks process descriptor that was removed.
 */
PD* dequeue_periodic(void)
{
	PD* p = periodic_queue[periodic_queue_front++];

	if(periodic_queue_front == MAXTHREAD)
	{
		periodic_queue_front = 0;
	}

	periodic_queue_size--;
	return p;
}

/*
 *	enqueue_rr
 *
 *	Enqueues the PD of a ready round robin task into the round robin ready queue.
 *
 *	Params:
 *		PD* p - Pointer to the tasks process descriptor.
 */
void enqueue_rr(PD* p)
{
	if(rr_queue_size < MAXTHREAD)
	{
		if(rr_queue_rear == MAXTHREAD - 1)
		{
			rr_queue_rear = -1;
		}

		rr_queue[++rr_queue_rear] = p;
		rr_queue_size++;
	}
}

/*
 *	dequeue_rr
 *
 *	Dequeues the PD of the task that is going to be ran form the round robin ready queue.
 *
 *	Return:
 *		PD* p - Pointer to the tasks process descriptor that was removed.
 */
PD* dequeue_rr(void)
{
	PD* p = rr_queue[rr_queue_front++];

	if(rr_queue_front == MAXTHREAD)
	{
		rr_queue_front = 0;
	}

	rr_queue_size--;
	return p;
}

int main(void)
{
	Kernel_OS_Init();

	main_a();

	Kernel_OS_Start();
	return 1;
}