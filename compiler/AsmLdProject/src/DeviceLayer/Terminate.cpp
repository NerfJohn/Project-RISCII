/*
 * Terminate.cpp
 *
 * "Device for terminating the application with a given error code"
 */

#include "OsLayer/OsExit.h"

#include "DeviceLayer/Terminate.h"

//==============================================================================
// Getter for singleton instance. Instantiates on first call.
Terminate* Terminate::getInst(void) {
	// Static instance- implicitly creates one (and only one) instance.
	static Terminate instance;

	// Return pointer to instance.
	return &instance;
}

//==============================================================================
// Terminates the program with given reason. Does NOT return.
void Terminate::exit(ExitReasonType_e exitCode) {
	// Call OS layer wrapper to handle termination (does not return).
	OsExit_exitWithReason(exitCode);

	/* This line is never reached */
}
