/*
 * OsExit.h: Abstraction layer for ending program with a given return code.
 */

#ifndef OS_OSEXIT_H_
#define OS_OSEXIT_H_

#include "Domain/RetCode_e.h"

/*
 * Ends the program, returning a given code to the caller.
 *
 * Exits the program, using given value as the exir code. Naturally, this
 * function does NOT return.
 *
 * @param retCode value to return to the caller
 */
void OsExit_exit(RetCode_e retCode);

#endif /* OS_OSEXIT_H_ */
