/*
 * Terminate.cpp: Wrapper for handling different program exit scenarios.
 */

#include "Device/Print.h"
#include "Os/OsExit.h"

#include "Device/Terminate.h"

using namespace std;

//==============================================================================
// Terminates the program without any additional output/processing.
void Terminate_silent(RetCode_e const exitCode) {
	// Behold- the Irish goodbye.
	OsExit_exit(exitCode);

	/* Line never reached */
}

//==============================================================================
// Terminates using RET_ASSERT. Prints assert message just prior to exiting.
void Terminate_assert(std::string const& msg) {
	// Print message explaining assert.
	Print::inst().bug(msg);

	// Next, immediately terminate as an assert.
	OsExit_exit(RET_ASSERT);

	/* Line never reached */
}
