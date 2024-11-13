/*
 * OsStdout.h: Abstraction layer for sending standard serial output.
 */

#ifndef OS_OSSTDOUT_H_
#define OS_OSSTDOUT_H_

#include <string>

/*
 * Prints the given string to the stdout file. Always flushes buffer.
 *
 * @param msg string to print to stdout
 */
void OsStdout_printStr(std::string const& msg);

#endif /* OS_OSSTDOUT_H_ */
