/*
 * OsExit.h
 *
 * "Wrapper for OS specific program self-termination practices"
 */

#ifndef SRC_OSLAYER_OSEXIT_H_
#define SRC_OSLAYER_OSEXIT_H_

#include "DomainLayer/ExitReasonType_e.h"

/*
 * Exit program for given reason. Function does NOT return.
 *
 * Causes OS to terminate program with a return code equal to the enum's
 * given value. Due to exiting, calls to this function will never return.
 *
 * @param exitCode value returned to the OS upon program termination
 */
void OsExit_exitWithReason(ExitReasonType_e exitCode);

#endif /* SRC_OSLAYER_OSEXIT_H_ */
