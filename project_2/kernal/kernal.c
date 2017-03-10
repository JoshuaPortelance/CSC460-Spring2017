/*
 * Kernel.c
 *
 * Created: 3/10/2017 11:00:39 AM
 *  Author: Josh
 */
 
/*
 * Definitions.
 */
#define Disable_Interrupt()		asm volatile ("cli"::)
#define Enable_Interrupt()		asm volatile ("sei"::)

/*
 * Function Prototypes.
 */ 
// Kernel functions.
static void Kernel_OS_Abort(unsigned int error);
static void Kernel_idle(void);
static void Kernel_main_loop(void);
static void Kernel_request_handler(void);

// Context switching functions.
// void exit_kernel(void);
// void enter_kernel(void);

// Task functions.
static PID  Kernel_Task_Create_System(void (*f)(void), int arg);
static PID  Kernel_Task_Create_RR(void (*f)(void), int arg);
static PID  Kernel_Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset);
static void Kernel_Task_Next(void);
static int  Kernel_Task_GetArg(void);

// Channel functions.
static CHAN Kernel_Chan_Init();
static void Kernel_Send(CHAN ch, int v);
static int  Kernel_Recv(CHAN ch);
static void Kernel_Write(CHAN ch, int v);

// Other functions.
static unsigned int Kernel_Now();


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
static void Kernel_OS_Abort(unsigned int error)
{

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
static PID Kernel_Task_Create_System(void (*f)(void), int arg)
{

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
static PID Kernel_Task_Create_RR(void (*f)(void), int arg)
{

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
static PID Kernel_Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset)
{

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
static void Kernel_Task_Next(void)
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
static int Kernel_Task_GetArg(void)
{

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
static CHAN Kernel_Chan_Init()
{

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
static void Kernel_Send(CHAN ch, int v)
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
static int Kernel_Recv(CHAN ch)
{

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
static void Kernel_Write(CHAN ch, int v)
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
static unsigned int Kernel_Now()
{

}