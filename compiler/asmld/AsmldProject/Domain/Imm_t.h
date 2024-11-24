/*
 * Imm_t.h: Immediate value representation with configurable "signedness".
 */

#ifndef DOMAIN_IMM_T_H_
#define DOMAIN_IMM_T_H_

#include <cstdint>

/*
 * Representation of immediate value with optional "no sign".
 *
 * Effectively an integer with additional data to indicate the value's sign.
 * This allows decimal and hexadecimal immediates to be commonly represented
 * without forcing hexadecimal values to have an implied sign.
 */
typedef struct {
	// Core values- integer value and indicator of integer vs binary value.
	int32_t m_val;
	bool    m_isBin;
} Imm_t;

#endif /* DOMAIN_IMM_T_H_ */
