/*
 * InstrType_e.h: Binary representation of instruction keywords/opcodes.
 */

#ifndef DOMAIN_INSTRTYPE_E_H_
#define DOMAIN_INSTRTYPE_E_H_

/*
 * Enum to define the available ISA level instructions.
 *
 * Enum is meant to be a more restrictive "LexToken" type- focusing on the
 * instructions such that instruction nodes can explicitly express an
 * ISA instruction opcode, but determine that instruction using their tokens.
 */
typedef enum {
	// Real values.
	INSTR_SHR = 0xD,           // Shift Right (Arithmetic/Logical)

	// Special values.
	INSTR_INVALID              // Unknown/invalid instruction
} InstrType_e;

#endif /* DOMAIN_INSTRTYPE_E_H_ */
