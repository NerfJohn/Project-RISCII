/*
 * PrintUtils.h
 *
 * "Helpful functions for formatting printed output"
 */

#ifndef SRC_UTILS_PRINTUTILS_H_
#define SRC_UTILS_PRINTUTILS_H_

#include <string>
#include "DomainLayer/LogType_e.h"

/*
 * Formats basic message to be printed to stdout.
 *
 * Formats message to include program's name and message's type (as well as
 * the custom message). Variable msg is modified by this function.
 *
 * @param type grouping message falls under
 * @param msg custom message to re-format into log message
 * @return true if formatted successfully, false otherwise
 */
bool PrintUtils_formatLog(LogType_e type, std::string* msg);

/*
 * Formats message to be printed to stdout. Includes a given filename.
 *
 * Acts as variant/flavor of formatLog(LogType_e, std::string) function. Allows
 * a filename to be added to the message in a unified manner. Variable msg is
 * modified by this function.
 *
 * @param type grouping message falls under
 * @param file name of file related to message
 * @param msg custom message to re-format into log message
 * @return true if formatted successfully, false otherwise
 */
bool PrintUtils_formatLog(LogType_e type, std::string file, std::string* msg);

/*
 * Formats message to be printed to stdout. Includes a filename and line number.
 *
 * Acts as variant/flavor of formatLog(LogType_e, std::string) function. Allows
 * a filename and line number to be added to the message in a unified manner.
 * Variable msg is modified by this function.
 *
 * @param type grouping message falls under
 * @param file name of file related to message
 * @param line line number related to message
 * @param msg custom message to re-format into log message
 * @return true if formatted successfully, false otherwise
 */
bool PrintUtils_formatLog(LogType_e type,
		                  std::string file,
						  uint32_t line,
						  std::string* msg
						 );

/*
 * Formats special assert message to be printed to stdout.
 *
 * Format is used to denote special "assert" occasion. To prevent assert
 * cascades, the format is simple and cannot fail to format.
 *
 * @param msg custom message to re-format into assert message
 */
void PrintUtils_formatAssert(std::string* msg);

#endif /* SRC_UTILS_PRINTUTILS_H_ */
