/*
 * Test 04
 * Tests that the receivers wait until a sender is on the channel.
 * Once the sender comes and sends then the receivers start again,
 * in the order that they were blocked in. FIFO.
 */

#include "trace.h"
#include "kernel.h"
#include "04_receivers_waiting_on_sender.h"

#include <string.h>

int count = 0;

void test_results() {
	char * trace = get_trace();
	char * correct_trace = "(1,(2,(3,3),2),1),";
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

	//Task 2 should be the last RR to run after being unblocked.
	if (arg == 1) {
		Disable_Interrupt();
		test_results();
	}
}

void sender_task(void) {
	int arg = 0;
	arg = Task_GetArg();

	add_to_trace(arg, ENTER);

	Send(1, 1);

	add_to_trace(arg, EXIT);

	//This will exit and die.
}

void main_a() {
	//This will initialize channel 1.
	if(Chan_Init() == 0) {
		OS_Abort(10); //Channel failed to initialize for some reason.
	}

	Task_Create_System(receiver_task, 1);
	Task_Create_System(receiver_task, 2);
	Task_Create_System(sender_task, 3);
}
