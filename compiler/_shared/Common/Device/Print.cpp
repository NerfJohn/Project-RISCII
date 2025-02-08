/*
 * Print.cpp: Standardized formatting/behavior for printing serial output.
 */

#include "Common/Device/Terminate.h"
#include "Common/Os/OsStdout.h"

#include "Common/Device/Print.h"

using namespace std;

//==============================================================================

// String literals used in formatted messages.
#define ERROR_NAME   ("ERR")
#define WARNING_NAME ("WRN")
#define INFO_NAME    ("INF")
#define DEBUG_NAME   ("DBG")
#define ASSERT_NAME  ("ASSERT!")
#define NEWLINE      ("\n")

//==============================================================================

// Local saves of log settings.
LogType_e Print::s_level = LOG_WARNING;
string    Print::s_name  = "";

//==============================================================================
// Retrieves the singleton instance for operations.
Print& Print::inst(void) {
	// Static instance- only one is ever created.
	static Print s_instance;

	// Return singleton.
	return s_instance;
}

//==============================================================================
// Sets log type threshold for log().
void Print::setLogLevel(LogType_e const level) {s_level = level;}

//==============================================================================
// Sets log name for log().
void Print::setLogName(std::string const& name) {s_name = name;}

//==============================================================================
// Prints log message (Option 1: level + msg).
void Print::log(LogType_e   const  level,
		        std::string const& msg) {
	// Only log message if within level limit.
	if (level <= s_level) {
		// String to log- always begins with log name.
		string logStr = s_name;

		// Next, add the log's type.
		logStr += " [";
		switch (level) {
			case LOG_ERROR:   logStr += ERROR_NAME;   break;
			case LOG_WARNING: logStr += WARNING_NAME; break;
			case LOG_INFO:    logStr += INFO_NAME;    break;
			case LOG_DEBUG:   logStr += DEBUG_NAME;   break;
			default:
				// Bad type- src code bug!
				Terminate::inst().assert("log() unknown/bad log type");
				break;
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
// Prints log message (Option 2: level + file + msg).
void Print::log(LogType_e   const  level,
		        std::string const& file,
		        std::string const& msg) {
	// Format filename as part of the message.
	string logStr = string("(") + file + ") " + msg;

	// Reuse, reduce, recycle!
	this->log(level, logStr);
}

//==============================================================================
// Prints log message (Option 3: level + file + line + msg).
void Print::log(LogType_e   const  level,
		        std::string const& file,
		        uint32_t    const  line,
		        std::string const& msg) {
	// Format filename and line number as part of the message.
	string lineStr = to_string(line);
	string logStr = string("(") + file + ":" + lineStr + ") " + msg;

	// Reuse, reduce, recycle!
	this->log(level, logStr);
}

//==============================================================================
// Prints assert/bug message.
void Print::bug(std::string const& msg) {
	// Format string with prefix for bugs.
	string logStr = string(ASSERT_NAME) + " " + msg;

	// Print the bug's message.
	OsStdout_printStr(logStr + NEWLINE);
}

//==============================================================================
// Prints cli message.
void Print::cli(std::string const& msg) {
	// Just print plaintext- cli messages have no fancy prefix.
	OsStdout_printStr(msg + NEWLINE);
}
