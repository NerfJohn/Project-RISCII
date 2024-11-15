/*
 * Terminate.h: Wrapper for handling different program exit scenarios.
 */

#ifndef DEVICE_TERMINATE_H_
#define DEVICE_TERMINATE_H_

#include <string>
#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"

/*
 * Terminates the program without any additional output/processing.
 *
 * No additional metrics are logged before exiting. This is most ideal for
 * assertions and command line error scenarios. Naturally, this function
 * doesn't return.
 *
 * @param exitCode value to return to caller
 */
void Terminate_silent(RetCode_e const exitCode);

/*
 * Terminates using RET_ASSERT. Prints assert message just prior to exiting.
 *
 * Simply prints the given message, using assertion formatting, and exits with
 * RET_ASSERT. Naturally, it implements the "action" of an assert statement.
 * Likwise, it naturally doesn't return.
 *
 * @param msg message to print with assert formatting prior to exiting
 */
void Terminate_assert(std::string const& msg);

/*
 * Terminates using model data- logging a summary of program's results.
 *
 * Logs the results of the program (eg number of errors, return code, etc) prior
 * to exiting. Likewise, uses the reported return code when exiting. Naturally,
 * this function doesn't return.
 *
 * @param model shared data of the entire program
 */
void Terminate_summary(DataModel_t const& model);

#endif /* DEVICE_TERMINATE_H_ */
