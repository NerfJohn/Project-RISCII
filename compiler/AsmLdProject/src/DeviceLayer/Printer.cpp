/*
 * Printer.cpp
 *
 * "Device handling printing of log messages to stdout"
 */

#include <iostream>
#include "OsLayer/OsExit.h"
#include "OsLayer/OsStdout.h"

#include "DeviceLayer/Printer.h"

using namespace std;

//==============================================================================

// Definitions for string literals used in messages.
#define STRLIT_ASMLD_NAME ("asmld.exe")
#define STRLIT_ERROR      ("ERR")
#define STRLIT_WARNING    ("WARN")

//==============================================================================
// Getter for singleton instance. Instantiates on first call.
Printer* Printer::getInst(void) {
	// Static instance- implicitly creates one (and only one) instance.
	static Printer instance;

	// Return pointer to instance.
	return &instance;
}

//==============================================================================
// Print a given message to stdout. Type specifies message grouping.
void Printer::log(LogType_e type, std::string msg) {
	// Begin all print-outs with program's name.
	string printMsg = STRLIT_ASMLD_NAME;

	// Add appropriate message type.
	printMsg += " ["; // separate program name and log type
	switch (type) {
		case LOG_ERR:
			printMsg += string(STRLIT_ERROR) + "]";
			break;
		case LOG_WARN:
			printMsg += string(STRLIT_WARNING) + "]";
			break;
		default:
			// Unknown error- special assert + exit.
			printMsg = string(STRLIT_ASMLD_NAME)
			           + " ASSERT! asserted while adding log type";
			OsStdout_printStringLn(printMsg);
			OsExit_exitWithReason(REASON_ASSERT);
	}

	// Add custom message.
	printMsg += " "; // separate message type and custom message
	printMsg += msg;

	// Print message to stdout.
	OsStdout_printStringLn(printMsg);
}

//==============================================================================
// Print a given message to stdout. Includes given location in message.
void Printer::log(LogType_e type, std::string location, std::string msg) {
	// Prepend the file location to the custom message (with separator).
	string printMsg = location + ": " + msg;

	// Reuse "no file" log function for ease.
	this->log(type, printMsg);
}
