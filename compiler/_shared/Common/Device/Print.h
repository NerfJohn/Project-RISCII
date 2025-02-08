/*
 * Print.h: Standardized formatting/behavior for printing serial output.
 */

#ifndef COMMON_DEVICE_PRINT_H_
#define COMMON_DEVICE_PRINT_H_

#include <stdint.h>
#include <string>
#include "Common/Domain/LogType_e.h"

/*
 * @brief Singleton class providing formatting/logic for serial output.
 *
 * Formats cli/assert/log messages. Formatting includes header and appended
 * newline. Logic includes filtering log messages based on threshold log type.
 * Can set log type threshold and name (defaults = LOG_WARNING, "").
 */
class Print {
public:
	// Enforce Print class as a singleton.
	Print(Print const&)          = delete;
	void operator=(Print const&) = delete;

	/*
	 * @brief Retrieves the singleton instance for operations.
	 *
	 * @return reference to Print singleton
	 */
	static Print& inst(void);

	/*
	 * @brief Sets log type threshold for log().
	 *
	 * Sets "high-end" threshold for log output. Log messages with more verbose
	 * type than set log type are NOT printed.
	 *
	 * @param level new log type threshold
	 */
	void setLogLevel(LogType_e const level);

	/*
	 * @brief Sets log name for log().
	 *
	 * Allows log message to include identifying marks (e.g. program name).
	 *
	 * @param name name to add to log messages
	 */
	void setLogName(std::string const& name);

	/*
	 * @brief Prints log message.
	 *
	 * Only prints message if log type is within threshold set by setLogLevel().
	 * Location information (ie filename/line number) are included if given.
	 *
	 * @param level log type of message
	 * @param file  optional filename (included in header)
	 * @param line  optional line number (include in header- requires filename)
	 * @param msg   message to print
	 */
	void log(LogType_e   const  level, // Option 1: level + msg
			 std::string const& msg);
	void log(LogType_e   const  level, // Option 2: level + file + msg
			 std::string const& file,
			 std::string const& msg);
	void log(LogType_e   const  level, // Option 3: level + file + line + msg
			 std::string const& file,
			 uint32_t    const  line,
			 std::string const& msg);

	/*
	 * @brief Prints assert/bug message.
	 *
	 * @param msg message to print
	 */
	void bug(std::string const& msg);

	/*
	 * @brief Prints cli message.
	 *
	 * @param msg message to print
	 */
	void cli(std::string const& msg);

private:
	// Local save of log settings.
	static LogType_e   s_level;
	static std::string s_name;

	// Hide constructor/destructor to force as singleton.
	Print()  {/* no actions */}
	~Print() {/* no actions */}
};

#endif /* COMMON_DEVICE_PRINT_H_ */
