/*
 * OsExit.h: Abstraction layer for ending program with a given return code.
 */

#ifndef COMMON_OS_OSEXIT_H_
#define COMMON_OS_OSEXIT_H_

#include "Domain/RetCode_e.h"

/*
 * @brief Terminates program, returning given code to the caller.
 *
 * @param retCode value to return to the caller
 */
void OsExit_exit(RetCode_e const retCode);

#endif /* COMMON_OS_OSEXIT_H_ */
