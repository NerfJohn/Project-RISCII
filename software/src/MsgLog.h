/*
 * MsgLog.h
 */

#ifndef MSGLOG_H_
#define MSGLOG_H_

#include <string>

/*
 * Defines log message types.
 *
 * Types reflect nature of the message being logged. They can also be used by
 * the message logger to determine actions to take (e.g. terminate after
 * logging).
 */
enum MsgLogType {
	MSG_ERR,
	MSG_WARN,
	MSG_INFO
};

/*
 * Static class to simplify business logic of log messages.
 *
 * Class provides simple interface for setting up input parameters for logging
 * business logic and for formatting log messages. Given the ubiquity of logging
 * messages during execution, the class acts as a single, static object.
 */
class MsgLog {
public:
	/*
	 * Setter functions for pertinent logging paramters.
	 *
	 * The verbose flag (-v) allows INFO messages to be logged, while the
	 * werror flag (-Werror) allows WARN messages to cause the program to
	 * terminate (just like ERR messages).
	 *
	 * @param setFlag new boolean value for flag
	 */
	static void doVerbose(const bool setFlag) {MsgLog::m_doVerbose = setFlag;}
	static void doWerror(const bool setFlag) {MsgLog::m_doWerror = setFlag;}

	/*
	 * Simplified formatters for logging messages.
	 *
	 * Functions mostly handle formatting of log message for consistency across
	 * the program (e.g. display of line numbers). The functions also handle
	 * logging business logic (e.g. terminating after logging an error message).
	 *
	 * @param type type of message to log
	 * @param lineNum line number to add to message (if applicable)
	 * @param msg message to save to log
	 */
	static void logMsg(const MsgLogType type, const std::string msg);
	static void logMsg(const MsgLogType type, const int lineNum, std::string msg);

	/*
	 * Std. Destructor.
	 */
	virtual ~MsgLog(void) {/* Empty dtor */}

private:
	// Private constructor to prevent instantiation.
	MsgLog(void) {/* Empty ctor */}

	// Logging parameters- control behavior of different message types.
	static bool m_doVerbose;
	static bool m_doWerror;
};

#endif /* MSGLOG_H_ */
