/*
 * Terminate.h: Wrapper for handling different program exit scenarios.
 */

#ifndef DEVICE_TERMINATE_H_
#define DEVICE_TERMINATE_H_

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
void Terminate_silent(RetCode_e exitCode);

#endif /* DEVICE_TERMINATE_H_ */
