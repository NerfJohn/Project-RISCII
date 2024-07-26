/*
 * PrintUtils.cpp
 *
 * "Helpful functions for formatting printed output"
 */

#include "Utils/PrintUtils.h"

using namespace std;

//==============================================================================

// Common definitions for status return codes.
#define RET_GOOD (0)
#define RET_ERR  (-1)

// String literals used in formatted messages.
#define PRGM_NAME    ("asmld.exe")
#define ERROR_NAME   ("ERR  ")
#define WARNING_NAME ("WARN ")
#define INFO_NAME    ("INFO ")
#define DEBUG_NAME   ("DEBUG")
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
		case LOG_INFO:
			printMsg += string("[") + INFO_NAME + "]";
			break;
		case LOG_DEBUG:
			printMsg += string("[") + DEBUG_NAME + "]";
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

//==============================================================================
// Formats model data into a summary of the assembly process. Info type msg.
int PrintUtils_formatSummary(DataModel_t const& model, std::string& msg) {
	// Code returned- indicating success of function.
	int retCode = RET_GOOD; // innocent till guilty

	// Get summary's main information.
	string numWarns = to_string(model.m_numWarnings);
	string numErrs  = to_string(model.m_numErrors);
	string prgmRes  = (model.m_firstReason == REASON_SUCCESS) ?
			          string("Image created") :
					  string("Image NOT created");

	// Format as an informational message.
	msg = numWarns + " warnings, " + numErrs + " errors- " + prgmRes;
	if(!PrintUtils_formatLog(LOG_INFO, &msg)) {retCode = RET_ERR;}

	// Finished- return result of formatting.
	return retCode;
}

//==============================================================================
// Formats a help menu string that can be directly printed.
std::string PrintUtils_formatHelp(void) {
	// Help menu (for now) is a manually updated constant.
	return string("Assembler/Linker for Project RISCII hardware\n") +
			      "Usage: asmld.exe [Options...] <input files...>\n" +
				  "\n" +
				  "Options:\n" +
				  "  -h        print this help menu and exit\n" +
				  "  -o  <arg> sets filename to use for output file\n" +
				  "  -ll <arg> set stdout log level to:\n" +
				  "              error   errors only\n" +
				  "              warning errors and warnings (default)\n" +
				  "              info    process related info\n" +
				  "              debug   all available output\n";
}
