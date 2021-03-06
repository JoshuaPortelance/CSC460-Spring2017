/*
 * Test 05
 * Tests that the sender will become blocked if no receivers are on
 * the channel.
 */

#include "trace.h"
#include "kernel.h"
#include "05_sender_waiting_on_receiver.h"

#include <string.h>

void test_results() {
	char * trace = get_trace();
	char * correct_trace = "(1,(2,2),1),";
	if (strcmp(correct_trace, trace) == 0) {
		OS_Abort(TEST_PASS);
	} else {
		OS_Abort(TEST_FAIL);
	}
}

void receiver_task(void) {
	int arg = 0;
	arg = Task_GetArg();

	add_to_trace(arg, ENTER);

	Recv(1);

	add_to_trace(arg, EXIT);
}

void sender_task(void) {
	int arg = 0;
	arg = Task_GetArg();

	add_to_trace(arg, ENTER);

	Send(1, 1);

	add_to_trace(arg, EXIT);

	Disable_Interrupt();
	test_results();
}

void main_a() {
	//This will initialize channel 1.
	if(Chan_Init() == 0) {
		OS_Abort(10); //Channel failed to initialize for some reason.
	}
	Task_Create_System(sender_task, 1);
	Task_Create_System(receiver_task, 2);
}
