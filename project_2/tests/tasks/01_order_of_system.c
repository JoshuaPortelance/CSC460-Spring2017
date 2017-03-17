/*
 * Test 01
 * Tests that the RTOS correctly schedules System tasks.
 */

#include "trace.h"
#include "kernel.h"
#include "01_order_of_system.h"

#include <string.h>

int count = 0;

void test_results() {
	char * trace = get_trace();
	char * correct_trace = "(1,1),(2,2),(3,3),(4,4),(5,5),(6,6),(7,";
	if (strcmp(correct_trace, trace) == 0) {
		OS_Abort(TEST_PASS);
	} else {
		OS_Abort(TEST_FAIL);
	}
}

void task(void) {
	int arg = 0;
	arg = Task_GetArg();

	for (;;) {
		add_to_trace(arg, ENTER);

		if (arg == 7) {
			Disable_Interrupt();
			test_results();
		}
		add_to_trace(arg, EXIT);
		Task_Next();
	}
}

void main_a() {
	Task_Create_System(task, 1);
	Task_Create_System(task, 2);
	Task_Create_System(task, 3);
	Task_Create_System(task, 4);
	Task_Create_System(task, 5);
	Task_Create_System(task, 6);
	Task_Create_System(task, 7);
}
