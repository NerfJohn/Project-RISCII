/*
 * PrintUtils.cpp
 *
 * "Helpful functions for formatting printed output"
 */

#include "Utils/PrintUtils.h"

using namespace std;

//==============================================================================

// String literals used in formatted files.
#define PRGM_NAME    ("asmld.exe")
#define ERROR_NAME   ("ERR  ")
#define WARNING_NAME ("WARN ")
#define ASSERT_NAME  ("ASSERT!")

//==============================================================================
// Formats basic message to be printed to stdout.
bool PrintUtils_formatLog(LogType_e type, std::string* msg) {
	// Begin all messages with program's name.
	string printMsg = PRGM_NAME;

	// Add appropriate message type.
	printMsg += " "; // separate program name and log type
	switch (type) {
		case LOG_ERR:
			printMsg += string("[") + ERROR_NAME + "]";
			break;
		case LOG_WARN:
			printMsg += string("[") + WARNING_NAME + "]";
			break;
		default:
			// Unknown type (likely design bug)- halt formatting.
			return false; // indicate fault found
	}

	/* Reached if log type is valid */

	// Add custom message.
	printMsg += " "; // separate message type and custom message
	printMsg += *msg;

	// Overwrite custom message with formatted message.
	*msg = printMsg;

	// Successful formatting.
	return true;
}

//==============================================================================
// Formats message to be printed to stdout. Includes a given filename.
bool PrintUtils_formatLog(LogType_e type, std::string file, std::string* msg) {
	// Prepend filename to "use up" the variable.
	*msg = file + ": " + *msg; // separate filename and custom message

	// Now effectively a basic message- reuse the function.
	return PrintUtils_formatLog(type, msg);
}

//==============================================================================
// Formats message to be printed to stdout. Includes a filename and line number.
bool PrintUtils_formatLog(LogType_e type,
		                  std::string file,
						  uint32_t line,
						  std::string* msg
						 ) {
	// Prepend line number to "use up" the variable.
	*msg = to_string(line) + ": " + *msg; // separate line number and message

	// Prepend filename to "use up" the variable.
	*msg = file + "/" + *msg; // separate file and line number (+ message)

	// Now effectively a basic message- reuse the function.
	return PrintUtils_formatLog(type, msg);
}

//==============================================================================
// Formats special assert message to be printed to stdout.
void PrintUtils_formatAssert(std::string* msg) {
	// Begin all messages with program's name.
	string printMsg = PRGM_NAME;

	// Add special type to denote assert.
	printMsg += string(" ") + ASSERT_NAME; // separate program name and type

	// Add custom message.
	printMsg += " "; // separate message type and custom message
	printMsg += *msg;

	// Overwrite custom message with formatted message.
	*msg = printMsg;
}
