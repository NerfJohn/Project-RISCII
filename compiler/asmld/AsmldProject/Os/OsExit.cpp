/*
 * OsExit.cpp: Abstraction layer for ending program with a given return code.
 */

#include <stdlib.h>

#include "Os/OsExit.h"

using namespace std;

//==============================================================================
// Ends the program, returning a given code to the caller.
void OsExit_exit(RetCode_e retCode) {
	// Exit program with given code (function does NOT return).
	exit(retCode);

	/* This line is never reached */
}

