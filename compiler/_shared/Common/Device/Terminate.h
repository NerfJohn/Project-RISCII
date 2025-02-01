/*
 * Terminate.h: Standardizes program exit scenarios.
 */

#ifndef DEVICE_TERMINATE_H_
#define DEVICE_TERMINATE_H_

#include <string>
#include "Domain/DataModel_t.h"
#include "Domain/RetCode_e.h"

// Condensed "assert if null" condition.
#define IF_NULL(ptr,msg) if ((ptr) == nullptr) {Terminate_assert(msg);}

/*
 * @brief Terminates program (without output besides given return code).
 *
 * @param exitCode value to return to caller
 */
void Terminate_silent(RetCode_e const exitCode);

/*
 * @brief Prints given message as assert, then terminates with RET_ASSERT.
 *
 * @param msg message to print as assertion
 */
void Terminate_assert(std::string const& msg);

/*
 * @brief Prints summary of program's status, then terminates.
 *
 * Prints (as LOG_INFO) counts for recorded warnings/errors and return code
 * value (as stored in given model). Terminates using reported return code.
 *
 * @param model data model (of program) with summary data
 */
void Terminate_summary(DataModel_t const& model);

#endif /* DEVICE_TERMINATE_H_ */
