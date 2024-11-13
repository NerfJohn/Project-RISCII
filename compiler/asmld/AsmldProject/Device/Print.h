/*
 * Print.h: Standardizes types of printing formats/behavior.
 */

#ifndef DEVICE_PRINT_H_
#define DEVICE_PRINT_H_

#include <stdint.h>
#include <string>
#include "Domain/LogType_e.h"

/*
 * Singleton class to handle printed formatting logic for all serial output.
 *
 * Effectively standardizes how serial messages are output to the user. This
 * includes both formatting headers/newlines onto each message, but also
 * implementing logic to filter messages based on desired info granularity.
 */
class Print {
public:
	// Enforce Print class as a singleton.
	Print(Print const&)          = delete;
	void operator=(Print const&) = delete;

	/*
	 * Retrieves the singleton instance for operations.
	 *
	 * @return reference to Print singleton
	 */
	static Print& inst(void);

	/*
	 * Sets the log level of the singleton.
	 *
	 * Modifies the singleton's "threshold" for logged info. This affects how
	 * Print::log() calls are processed (ie if their messages are printed).
	 *
	 * @param level log level to set threshold to
	 */
	void setLogLevel(LogType_e const level);

	/*
	 * Prints given message using "log" line formatting.
	 *
	 * Formats and prints given message (with respect to log levels- will NOT
	 * print if given log level isn't within set log level's scope). Location
	 * information, such as a filename and line number, can also be formatted.
	 *
	 * @param level log level to print message at
	 * @param file  name of file (eg logging info about given file)
	 * @param line  line number (eg logging error at file/line location)
	 * @param msg   string to print (ie heart of the log info)
	 */
	void log(LogType_e const level, std::string const& msg);
	void log(LogType_e const level,
			 std::string const& file,
			 std::string const& msg);
	void log(LogType_e const level,
			 std::string const& file,
			 uint32_t const line,
			 std::string const& msg);

	/*
	 * Print given message using "bug" line formatting. Always printed.
	 *
	 * @param msg string to print
	 */
	void bug(std::string const& msg);

	/*
	 * Print given message using "cli" line formatting. Always printed.
	 *
	 * @param msg string to print
	 */
	void cli(std::string const& msg);

private:
	// Local save of "threshhold" log level.
	static LogType_e m_level;

	// Hide constructor/destructor to force as singleton.
	Print()  {/* no actions */}
	~Print() {/* no actions */}
};

#endif /* DEVICE_PRINT_H_ */
