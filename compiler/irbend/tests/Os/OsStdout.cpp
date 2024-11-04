#include "tests/Os/OsExit.h"

#include "Os/OsStdout.h"
#include "tests/Os/OsStdout.h"

using namespace std;

// "Buffer" used to save output (for unit testing).
static string printBuffer = "";

//==============================================================================
// (Source code declared function).
void OsStdout_printStr(std::string msg) {
	// Append "output" data to buffer (IF program is still "running").
	if (!OsExit_hasRet()) {printBuffer += msg;}
}

//==============================================================================
// Get output data.
std::string OsStdout_getOutput(void) {
	// Return contents output so far.
	return printBuffer;
}