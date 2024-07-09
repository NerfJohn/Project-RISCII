/*
 * InstrType_e.h
 *
 * "Enum describing the available ISA instructions"
 */

#ifndef SRC_DOMAINLAYER_INSTRTYPE_E_H_
#define SRC_DOMAINLAYER_INSTRTYPE_E_H_

/*
 * Enum to define the available ISA level instructions.
 *
 * Enum is meant to be a more restrictive "LexToken" type- focusing on the
 * instructions such that instruction items can explicitly express an
 * ISA instruction, but determine that instruction using their tokens.
 */
typedef enum {
	// Cores values.
	INSTR_SHR,                 // Shift Right

	// Special values.
	INSTR_INVALID              // Invalid/Unknown instruction
} InstrType_e;

#endif /* SRC_DOMAINLAYER_INSTRTYPE_E_H_ */
