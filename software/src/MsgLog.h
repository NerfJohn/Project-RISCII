/*
 * MsgLog.h
 */

#ifndef MSGLOG_H_
#define MSGLOG_H_

#include <string>

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
	 * Setter functions for pertinent logging parameters.
	 *
	 * The verbose flag (-v) allows INFO messages to be logged, while the
	 * Werror flag (-Werror) allows WARN messages to cause the program to
	 * terminate (similar to ERR and ASSERT messages).
	 *
	 * Both flags are FALSE by default.
	 *
	 * @param setFlag new boolean value for flag
	 */
	static void setVerbose(const bool setFlag) {MsgLog::m_doVerbose = setFlag;}
	static void setWerror(const bool setFlag) {MsgLog::m_doWerror = setFlag;}

	/*
	 * Simplified formatters for logging messages.
	 *
	 * Functions handle formatting, printing, and business logic of messages. A
	 * line number may be provided to be formatted into the message (but is left
	 * out if not given or is less than 0).
	 *
	 * Nature of each message type:
	 * 1) ERR- Error preventing compilation, exits after logging
	 * 2) WARN- Warning complicating compilation, exits if "setWerror(true)"
	 * 3) INFO- General compilation info, logged if "setVerbose(true)"
	 * 4) ASSERT- Error in SOURCE CODE itself, exits after logging
	 *
	 * @param msg message to save to log
	 * @param lineNum line number to add to message (if given and >= 0)
	 */
	static void logERR(std::string msg, int lineNum = -1);
	static void logWARN(std::string msg, int lineNum = -1);
	static void logINFO(std::string msg, int lineNum = -1);
	static void logASSERT(std::string msg, int lineNum = -1);

	/*
	 * Std. Destructor.
	 */
	virtual ~MsgLog(void) {/* Empty dtor */}

private:
	// Private constructor to prevent instantiation.
	MsgLog(void) {/* Empty ctor */}

	// Helper function encapsulating main logging/business logic.
	static void logMsg(std::string type, int lineNum, std::string msg);

	// Logging parameters- control behavior of different message types.
	static bool m_doVerbose;
	static bool m_doWerror;
};

#endif /* MSGLOG_H_ */
