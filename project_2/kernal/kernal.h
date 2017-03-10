/*
 * Kernel.h
 *
 * Created: 3/10/2017 11:00:55 AM
 *  Author: Josh
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