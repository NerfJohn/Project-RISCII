/*
 * Terminate.cpp: Wrapper for handling different program exit scenarios.
 */

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
