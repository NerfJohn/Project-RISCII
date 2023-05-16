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

// Simplified formatter for logging messages.
void MsgLog::logMsg(const MsgLog_t type, const std::string msg) {
	// Determine message prefix.
	string prefix;
	switch (type) {
		case ERR:  prefix = "ERR";  break;
		case WARN: prefix = "WARN"; break;
		default:   prefix = "INFO"; break; // "otherwise, assume verbose..."
	}

	// Log message (INFO requires -v flag).
	if ((type != INFO) || MsgLog::m_doVerbose) {
		cout << prefix << "! " << msg << endl;
	}

	// Terminate the program as needed (error or warning w/ -Werror flag).
	if ((type == ERR) || ((type == WARN) && MsgLog::m_doWerror)) {
		cout << "Exiting..." << endl;
		exit(1);
	}
}

// Simplified formatter for logging messages. Adds line number to log.
void MsgLog::logMsg(const MsgLog_t type, const int lineNum, std::string msg) {
	// Prepend line number to message to re-use other implementation.
	ostringstream lineNumMsg;
	lineNumMsg << "(Line " << lineNum << ") " << msg;
	MsgLog::logMsg(type, lineNumMsg.str());
}
