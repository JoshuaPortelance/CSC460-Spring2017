#include "os.h"
#include <avr/io.h>
#include <avr/interrupt.h>
// EXPECTED RUNNING ORDER: S1,Timer,S1,P1,RR1,P1,RR1
//
// Timer............... Write
// S1 block on Recv		Recv       block on Send	 Send
// P1												 Recv@ms=150	Task_Next.......
// RR1																Delay.......................
CHAN c1=0;

void DEBUG_INIT()
{
	DDRB |= (1<<PB1);	//pin 52
	DDRB |= (1<<PB2);	//pin 51
	DDRB |= (1<<PB3);	//pin 50
}

void DEBUG_ON(int num)
{
	switch (num) {
		case 1:
		PORTB |= (1<<PB1);
		break;
		case 2:
		PORTB |= (1<<PB2);
		break;
		case 3:
		PORTB |= (1<<PB3);
		break;
		default:
		break;
	}
}

void DEBUG_OFF(int num)
{
	switch (num) {
		case 1:
		PORTB &= ~(1<<PB1);
		break;
		case 2:
		PORTB &= ~(1<<PB2);
		break;
		case 3:
		PORTB &= ~(1<<PB3);
		break;
		default:
		break;
	}
}

void configure_timer()
{
	//Clear timer config.
	TCCR3A = 0;
	TCCR3B = 0;
	//Set to CTC (mode 4)
	TCCR3B |= (1<<WGM32);

	//Set prescaller to 256
	TCCR3B |= (1<<CS32);

	//Set TOP value (0.1 seconds)
	OCR3A = 6250;

	//Set timer to 0 (optional here).
	TCNT3 = 0;
	
	//Enable interupt A for timer 3.
	TIMSK3 |= (1<<OCIE3A);
}

void timer_handler()
{
	Write(c1,1);
	//Disable interupt
	TIMSK3 &= ~(1<<OCIE3A);
}

ISR(TIMER3_COMPA_vect)
{
	timer_handler();
}

void Task_RR1() {
	while(Now() < 250) {
		Task_Next();
	}
	DEBUG_ON(3);
	for(;;);
}

void Task_P1()
{
	CHAN c = (CHAN)Task_GetArg();
	int i = Recv(c);
	if(i != 1) {
		for(;;);
	}
	Task_Create_RR(Task_RR1, 0);
	Task_Next();
	DEBUG_ON(2);
	for(;;)	{
		Task_Next();
	}
}

void Task_S1()
{
	CHAN c1 = (CHAN)Task_GetArg();
	int i = Recv(c1);
	if(i != 1) {
		for(;;);
	}
	CHAN c2 = Chan_Init();
	Task_Create_Period(Task_P1, (int)c2, 10, 1, 15);
	Send(c2,1);
	DEBUG_ON(1);
}

void main_a()
{
	DEBUG_INIT();
	CHAN c1 = Chan_Init();

	Task_Create_System(Task_S1, (int)c1);
	
	configure_timer();
}