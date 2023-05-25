/*
 * MsgLog.cpp
 */

#include <iostream>
#include <sstream>

#include "MsgLog.h"

using namespace std;

// Logging parameters- control behavior of different message types.
bool MsgLog::m_doVerbose = false;
bool MsgLog::m_doWerror = false;

// Helper function encapsulating main logging/business logic.
void MsgLog::logMsg(std::string type, int lineNum, std::string msg) {
	// Append line number if applicable (ie non-negative).
	string prefix("! "); // Default to without line number
	if (lineNum >= 0) {
		prefix += "(Line " + to_string(lineNum) + ") ";
	}

	// Log message (INFO require verbose flag).
	if ((type != "INFO") || MsgLog::m_doVerbose) {
		cout << type << prefix << msg << endl;
	}

	// Exit as needed (ERR/ASSERT always exit, WARN exits for Werror flag).
	bool warnExits = (type == "WARN") && MsgLog::m_doWerror;
	if ((type == "ERR") || warnExits || (type == "ASSERT")) {
		cout << "Exiting..." << endl;
		exit(1);
	}
}

// Simplified formatters for logging messages.
void MsgLog::logERR(std::string msg, int lineNum) {
	MsgLog::logMsg("ERR", lineNum, msg);
}
void MsgLog::logWARN(std::string msg, int lineNum) {
	MsgLog::logMsg("WARN", lineNum, msg);
}
void MsgLog::logINFO(std::string msg, int lineNum) {
	MsgLog::logMsg("INFO", lineNum, msg);
}
void MsgLog::logASSERT(std::string msg, int lineNum) {
	MsgLog::logMsg("ASSERT", lineNum, msg);
}

