/*
 * kernal.h
 *
 * Created: 3/10/2017 11:00:55 AM
 *  Author: Josh
 */ 

 void Kernal_OS_Abort(unsigned int error);

 PID  Kernal_Task_Create_System(void (*f)(void), int arg);
 PID  Kernal_Task_Create_RR(void (*f)(void), int arg);
 PID  Kernal_Task_Create_Period(void (*f)(void), int arg, TICK period, TICK wcet, TICK offset);
 void Kernal_Task_Next(void);
 int  Kernal_Task_GetArg(void);

 CHAN Kernal_Chan_Init();
 void Kernal_Send(CHAN ch, int v);
 int  Kernal_Recv(CHAN ch);
 void Kernal_Write(CHAN ch, int v);

 unsigned int Kernal_Now();