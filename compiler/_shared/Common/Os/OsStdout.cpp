/*
 * OsStdout.cpp: Abstraction layer for sending standard serial output.
 */

#include <iostream>

#include "Common/Os/OsStdout.h"

using namespace std;

//==============================================================================
// Prints given string verbatim to stdout file- always flushing/blocking.
void OsStdout_printStr(std::string const& msg) {
	// Print and flush.
	cout << msg << flush;
}
