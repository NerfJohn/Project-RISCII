/*
 * InfoUtil.h: Utilities for updating/querying the program's data model.
 */

#ifndef COMMON_UTIL_INFOUTIL_H_
#define COMMON_UTIL_INFOUTIL_H_

#include "Common/Domain/PrgmInfo_t.h"

/*
 * @brief Update progrm info/summary with most recent error/exit code.
 *
 * Increments found error count- saving error/return code with it. Intended to
 * be used to gather data for eventual use with Common/Device/Terminate.h's
 * summary() function.
 *
 * @param summary summary of program's run instance
 * @param errCode return code asssociated with error
 */
void InfoUtil_recordError(PrgmInfo_t& summary, int const errCode);

/*
 * @brief Update program info/summary with most recent warning.
 *
 * Increments found warning count. Intended to be used to gather data for
 * eventual use with Common/Device/Terminate.h's summary() function.
 *
 * @param summary summary of program's run instance
 */
void InfoUtil_recordWarn(PrgmInfo_t& summary);

#endif /* COMMON_UTIL_INFOUTIL_H_ */
