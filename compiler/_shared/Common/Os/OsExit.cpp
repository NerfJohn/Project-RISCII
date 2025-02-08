/*
 * OsExit.cpp: Abstraction layer for ending program with a given return code.
 */

#include <stdlib.h>

#include "Common/Os/OsExit.h"

using namespace std;

//==============================================================================
// Terminates program, returning given code to the caller.
void OsExit_exit(int const retCode) {
	// Exit program with given code (ie function does NOT return).
	exit(retCode);
}
