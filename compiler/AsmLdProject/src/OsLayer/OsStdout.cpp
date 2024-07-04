/*
 * OsStdout.cpp
 *
 * "Wrapper for OS specific stdout printing practices"
 */

#include <iostream>
#include "OsLayer/OsStdout.h"

using namespace std;

//==============================================================================
// Prints the message to stdout file. Always flushes the output.
void OsStdout_printString(std::string msg) {
	// Stream string to stdout file (flush to guarantee printing).
	cout << msg << flush;
}

//==============================================================================
// Prints the message to stdout file. Always newlines and flushes output.
void OsStdout_printStringLn(std::string msg) {
	// Stream string to stdout file (with newline- flush to guarantee printing).
	cout << msg << endl << flush;
}
