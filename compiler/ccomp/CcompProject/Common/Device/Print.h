/*
 * Print.h: Standardizes formatting/behavior for printing serial output.
 */

#ifndef COMMON_DEVICE_PRINT_H_
#define COMMON_DEVICE_PRINT_H_

#include <cstdint>
#include <string>
#include "Common/Domain/LogType_e.h"

// Common definition for "line feed/newline" character for printing.
#define LF ("\n")

/*
 * @brief Sets log type threshold for log().
 *
 * Sets "high-end" threshold for log output. Log messages with more verbose
 * type than set log type are NOT printed.
 *
 * @param level new log type threshold
 */
void Print_setLogLevel(LogType_e const level);

/*
 * @brief Sets log name for log().
 *
 * Allows log message to include identifying marks (e.g. program name).
 *
 * @param name name to add to log messages
 */
void Print_setLogName(std::string const& name);

/*
 * @brief Prints log message.
 *
 * Only prints message if log type is within threshold set by setLogLevel().
 * Location information (ie filename/line number) are included if given.
 *
 * @param level log type of message
 * @param file  optional filename    (included in header)
 * @param line  optional line number (included in header- requires filename)
 * @param msg   message to print
 */
void Print_log(LogType_e   const  level, // Option #1: Level + msg
		       std::string const& msg);
void Print_log(LogType_e   const  level, // Option #2: level + file + msg
		       std::string const& file,
			   std::string const& msg);
void Print_log(LogType_e   const  level, // Option #3: level + file + line + msg
		       std::string const& file,
			   uint32_t    const  line,
			   std::string const& msg);

/*
 * @brief Prints assert/bug message.
 *
 * Given its own name/prefix. Does NOT use set log name. Does NOT exit from
 * program.
 *
 * @param msg message to print
 */
void Print_bug(std::string const& msg);

/*
 * @brief Prints cli message.
 *
 * Given no prefix (effective println). Does NOT use set log name.
 *
 * @param msg message to print
 */
void Print_cli(std::string const& msg);

#endif /* COMMON_DEVICE_PRINT_H_ */
