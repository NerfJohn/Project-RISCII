#include <stdlib.h>
#include "Os/OsExit.h"
#include "tests/Os/OsExit.h"

using namespace std;

// "Buffer" used to save return code (for unit testing).
static int code = RET_SUCCESS; // assume things went well for now

// Flag used to ensure return code is captured ONCE (tests can't just "exit").
static bool hasRet = false;

//==============================================================================
// (Source code declared function).
void OsExit_exit(RetCode_e const retCode) {
	// Gather return code ONCE.
	if (!hasRet) {code = retCode; hasRet = true;}
}

//==============================================================================
// Getter for return code.
int OsExit_getCode(void) {return code;}

//==============================================================================
// Determine of program has returned.
bool OsExit_hasRet(void) {
	return hasRet;
}
