/*
 * OsStdout.h: Abstraction layer for sending standard serial output.
 */

#ifndef COMMON_OS_OSSTDOUT_H_
#define COMMON_OS_OSSTDOUT_H_

#include <string>

/*
 * @brief Prints given string verbatim to stdout file- always flushing/blocking.
 *
 * @param msg string to print to stdout file
 */
void OsStdout_printStr(std::string const& msg);

#endif /* COMMON_OS_OSSTDOUT_H_ */
