/*
 * OsExit.cpp
 *
 * "Wrapper for OS specific program self-termination practices"
 */

#include <stdlib.h>
#include "OsLayer/OsExit.h"

//==============================================================================
// Exit program for given reason. Function does NOT return.
void OsExit_exitWithReason(ExitReasonType_e exitCode) {
	// Exit program with given code (function does NOT return).
	exit(exitCode);

	/* This line is never reached */
}
