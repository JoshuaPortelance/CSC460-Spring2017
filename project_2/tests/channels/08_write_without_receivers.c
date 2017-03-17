/*
 * Test 07
 * Tests that write transmits data and unblocks receivers already
 * on the channel.
 */

#include "trace.h"
#include "kernel.h"
#include "08_write_without_receivers.h"

#include <string.h>

void test_results() {
	char * trace = get_trace();
	char * correct_trace = "(1,1),";
	if (strcmp(correct_trace, trace) == 0) {
		OS_Abort(TEST_PASS);
		} else {
		OS_Abort(TEST_FAIL);
	}
}

void task(void) {
	int arg = 0;
	arg = Task_GetArg();

	add_to_trace(arg, ENTER);

	Write(1, 1);

	add_to_trace(arg, EXIT);

	Disable_Interrupt();
	test_results();
}

void main_a() {
	//This will initialize channel 1.
	if(Chan_Init() == 0) {
		OS_Abort(10); //Channel failed to initialize for some reason.
	}
	Task_Create_System(task, 1);
}
