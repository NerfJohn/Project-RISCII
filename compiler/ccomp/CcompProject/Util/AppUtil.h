/*
 * AppUtil.h: Utilities specific to application level operations/data.
 */

#ifndef UTIL_APPUTIL_H_
#define UTIL_APPUTIL_H_

#include "Domain/DataModel_t.h"

/*
 * @brief Updates data model for occurrence of an error.
 *
 * @param model data model of the program
 * @param code  exit code tied to the specific error
 */
void AppUtil_error(DataModel_t& model, RetCode_e const code);

/*
 * @brief Updates data model for occurrence of a warning.
 *
 * @param model data model of the program
 */
void AppUtil_warn(DataModel_t& model);

/*
 * @brief Prints summary of program's results, then exits.
 *
 * Function does NOT return.
 *
 * @param model data model of the program
 */
void AppUtil_exitSummary(DataModel_t& model);

/*
 * @brief Prints "bug" message, then exits.
 *
 * Function does NOT return.
 *
 * @param model data model of the program
 */
void AppUtil_exitBug(std::string const& msg);

#endif /* UTIL_APPUTIL_H_ */
