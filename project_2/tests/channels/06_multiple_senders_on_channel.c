/*
 * Test 06
 * Tests that if there are multiple senders on a channel,
 * the OS goes into an error state.
 */

#include "trace.h"
#include "kernel.h"
#include "06_multiple_senders_on_channel.h"

#include <string.h>

int count = 0;

void task(void) {
	int arg = 0;
	arg = Task_GetArg();

	add_to_trace(arg, ENTER);

	Send(1, 1);

	add_to_trace(arg, EXIT);

	//If either task reaches here then the error was not caught.
	OS_Abort(TEST_FAIL);
}

void main_t() {
	//This will initialize channel 1.
	if(Chan_Init() == 0) {
		OS_Abort(10); //Channel failed to initialize for some reason.
	}
	Task_Create_System(task, 1);
	Task_Create_System(task, 2);
	
}
