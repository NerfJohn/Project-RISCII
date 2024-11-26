/*
 * Instr_t.h: Generic instruction fields broken out into separate values.
 */

#ifndef DOMAIN_INSTR_T_H_
#define DOMAIN_INSTR_T_H_

#include <string>
#include "Domain/InstrType_e.h"

// Starting/default values.
#define INSTR_NO_OPCODE (INSTR_INVALID)
#define INSTR_NO_FLAGS  ("")
#define INSTR_NO_REG    ((uint8_t)(0x7F))        // big & NOT 3-bit
#define INSTR_NO_IMM    ((uint32_t)(0x7FFFFFFF)) // big & NOT 16-bit

/*
 * Contains potential fields for an instruction in purest form.
 *
 * Type is intended to be used to capture instruction args as separate fields
 * prior to formatting into one binary instruction. It is expected some (but
 * likely not all) fields will be overwritten on instantiation.
 */
typedef struct instr_t{ // (name given to appease build warning...)
	// Bit fields of an instruction.
	InstrType_e m_opcode = INSTR_NO_OPCODE;
	std::string m_flags  = INSTR_NO_FLAGS;
	uint8_t     m_r1     = INSTR_NO_REG;
	uint8_t     m_r2     = INSTR_NO_REG;
	uint8_t     m_r3     = INSTR_NO_REG;
	int32_t     m_imm    = INSTR_NO_IMM;
} Instr_t;

#endif /* DOMAIN_INSTR_T_H_ */
