/*
 * BinaryUtils.h
 *
 * "Helper functions for generating binary values from items"
 */

#ifndef SRC_UTILS_BINARYUTILS_H_
#define SRC_UTILS_BINARYUTILS_H_

#include "DomainLayer/InstrFields_t.h"

/*
 * Generates binary instruction from fields. Doesn't check field accuracy.
 *
 * Encapsulates details of binary formatting, but requires caller to ensure
 * field arguments are acceptable. Return instruction reference should always
 * be assumed as modified after calling this function.
 *
 * @param retInstr location to put formatted instruction
 * @param fields instruction field data to format into an instruction
 * @return non-zero for unexpected errors, otherwise zero for success
 */
int BinaryUtils_genInstr(uint16_t& retInstr, InstrFields_t const& fields);

#endif /* SRC_UTILS_BINARYUTILS_H_ */
