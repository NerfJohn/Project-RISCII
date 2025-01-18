/*
 * Print.cpp: Standardizes types of printing formats/behavior.
 */

#include "Device/Terminate.h"
#include "Os/OsStdout.h"

#include "Device/Print.h"

using namespace std;

//==============================================================================

// String literals used in formatted messages.
#define PRGM_NAME    ("cprep.exe")
#define ERROR_NAME   ("ERROR")
#define WARNING_NAME ("WARN ")
#define INFO_NAME    ("INFO ")
#define DEBUG_NAME   ("DEBUG")
#define ASSERT_NAME  ("ASSERT!")
#define NEWLINE      ("\n")

//==============================================================================

// Local save of "threshhold" log level.
LogType_e Print::m_level = LOG_SILENT;

//==============================================================================
// Retrieves the singleton instance for operations.
Print& Print::inst(void) {
	// Static instance- only one is ever created.
	static Print m_instance;

	// Return singleton.
	return m_instance;
}

//==============================================================================
// Sets the log level of the singleton.
void Print::setLogLevel(LogType_e const level) {
	// Save the type for future logging control flow.
	m_level = level;
}

//==============================================================================
// Prints given message using "log" line formatting.
void Print::log(LogType_e const level, std::string const& msg) {
	// Only log message if within level limit.
	if (level <= m_level) {
		// String to log- always begins with program's name.
		string logStr = PRGM_NAME;

		// Next, add the log's type.
		logStr += " [";
		switch (level) {
			case LOG_ERROR:   logStr += ERROR_NAME;   break;
			case LOG_WARNING: logStr += WARNING_NAME; break;
			case LOG_INFO:    logStr += INFO_NAME;    break;
			case LOG_DEBUG:   logStr += DEBUG_NAME;   break;
			default:
				// Unknown type input- design issue/bug.
				Terminate_assert("Invalid log level type used for logging");
				/* Line not reached */
		}
		logStr += "]";

		// Then, add message.
		logStr += " ";
		logStr += msg;

		// Finally, log/print message.
		OsStdout_printStr(logStr + NEWLINE);
	}
}

//==============================================================================
// Prints given message using "log" line formatting.
void Print::log(LogType_e const level,
		        std::string const& file,
				std::string const& msg) {
	// Format filename as part of the message.
	string logStr = string("(") + file + ") " + msg;

	// Reuse, reduce, recycle!
	this->log(level, logStr);
}

//==============================================================================
// Prints given message using "log" line formatting.
void Print::log(LogType_e const level,
		        std::string const& file,
				uint32_t const line,
				std::string const& msg) {
	// Format filename and line number as part of the message.
	string lineStr = to_string(line);
	string logStr = string("(") + file + ":" + lineStr + ") " + msg;

	// Reuse, reduce, recycle!
	this->log(level, logStr);
}

//==============================================================================
// Print given message using "bug" line formatting. Always printed.
void Print::bug(std::string const& msg) {
	// Format string with prefix for bugs.
	string logStr = string(ASSERT_NAME) + " " + msg;

	// Print the bug's message.
	OsStdout_printStr(logStr + NEWLINE);
}

//==============================================================================
// Print given message using "cli" line formatting. Always printed.
void Print::cli(std::string const& msg) {
	// Just print plaintext- cli messages have no fancy prefix.
	OsStdout_printStr(msg + NEWLINE);
}
