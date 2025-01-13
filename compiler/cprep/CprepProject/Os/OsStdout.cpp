/*
 * OsStdout.cpp: Abstraction layer for sending standard serial output.
 */

#include <iostream>

#include "Os/OsStdout.h"

using namespace std;

//==============================================================================
// Prints the given string to the stdout file. Always flushes buffer.
void OsStdout_printStr(std::string const& msg) {
	// Simply send data to stdout- flush to ensure send.
	cout << msg << flush;
}
