/*
 * OsStdout.h
 *
 * "Wrapper for OS specific stdout printing practices"
 */

#ifndef SRC_OSLAYER_OSSTDOUT_H_
#define SRC_OSLAYER_OSSTDOUT_H_

#include <string>

/*
 * Prints the message to stdout file. Always flushes the output.
 *
 * Sends string to OS's definition of the stdout file. The output is always
 * flushed to ensure printing, but a newline is NOT implicitly added.
 *
 * @param msg message to print
 */
void OsStdout_printString(std::string msg);

/*
 * Prints the message to stdout file. Always newlines and flushes output.
 *
 * Sends string to OS's definition of the stdout file. A newline is appended
 * to the message, followed by flushing to ensure printing.
 *
 * @param msg message to print (with an appended newline)
 */
void OsStdout_printStringLn(std::string msg);

#endif /* SRC_OSLAYER_OSSTDOUT_H_ */
