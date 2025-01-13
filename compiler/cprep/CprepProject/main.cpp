/*
 * main.cpp: Main elements/execution of the c-preprocessor program.
 */

#include "Os/OsStdout.h"
#include "Os/OsExit.h"

using namespace std;

//==============================================================================
// Start/main process of the asmld.exe program.
int main(int argc, char* argv[]) {
	// Nope.
	OsStdout_printStr("Nope");
	OsExit_exit(RET_SUCCESS);
}
