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
 * ISA instruction opcode, but determine that instruction using their tokens.
 */
typedef enum {
	// Cores values.
	INSTR_SWP = 0x1,           // Swap Register and Memory
	INSTR_BRC = 0x2,           // Branch Conditional
	INSTR_STR = 0x4,           // Store Memory Word
	INSTR_LDR = 0x5,           // Load Memory Word
	INSTR_JPR = 0x6,           // Jump Base Register (+ Return from Interrupt)
	INSTR_JLR = 0x7,           // Jump and Link Register
	INSTR_ADD = 0x8,           // Arithmetic Addition
	INSTR_SUB = 0x9,           // Arithmetic Subtraction
	INSTR_LBI = 0xA,           // Load Byte Immediate
	INSTR_XOR = 0xB,           // Bitwise XOR
	INSTR_SHL = 0xC,           // Shift Left
	INSTR_SHR = 0xD,           // Shift Right (Arithmetic/Logical)
	INSTR_ORR = 0xE,           // Bitwise ORR
	INSTR_AND = 0xF,           // Bitwise AND

	// Special values.
	INSTR_INVALID              // Invalid/Unknown instruction
} InstrType_e;

#endif /* SRC_DOMAINLAYER_INSTRTYPE_E_H_ */
