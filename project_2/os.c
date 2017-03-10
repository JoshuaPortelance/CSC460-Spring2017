/*
 *	os.c
 *
 *	Created: 3/10/2017 10:26:07 AM
 *	Author: Josh
 */ 

/*
 *	OS_Abort
 *
 *	Aborts the RTOS and enters a "non-executing" state with an error code.
 *
 *	Params:
 *		unsigned int error - The error code related to the abort.
 *	Return:
 *		void
 */
void OS_Abort(unsigned int error)
{

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
PID   Task_Create_System(void (*f)(void), int arg)
{

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
PID   Task_Create_RR(void (*f)(void), int arg)
{

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
PID   Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset)
{

}


/*
 *	Task_Next
 *
 *	Switches to the next highest priority task.
 *
 *	Params:
 *		void
 *	Return:
 *		void
 */
void Task_Next(void)
{

}


/*
 *	Task_GetArg
 *
 *	Gets the integer that was assigned to the task when created.
 *
 *	Params:
 *		void
 *	Return:
 *		int - The arg that was used when creating the task.
 */
int  Task_GetArg(void)
{

}


/*
 *	Chan_Init
 *
 *	Initializes a channel.
 *
 *	Params:
 *		void
 *	Return:
 *		CHAN - An initialized channel if successful, otherwise NULL.
 */
CHAN Chan_Init()
{

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
 *	Return:
 *		void
 */
void Send( CHAN ch, int v )
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
int Recv( CHAN ch )
{

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
 *	Return:
 *		void
 */
void Write( CHAN ch, int v )
{

}


/*
 *	Now
 *
 *	Returns the number of milliseconds since OS_Init().
 *
 *	Params:
 *		void
 *	Return:
 *		unsigned int - The number of milliseconds since OS_Init().
 */
unsigned int Now()
{

}