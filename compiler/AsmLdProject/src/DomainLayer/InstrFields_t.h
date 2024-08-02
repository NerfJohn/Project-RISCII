/*
 * InstrFields_t.h
 *
 * "Type to define assembly level fields of instruction (in processed form)"
 */

#ifndef SRC_DOMAINLAYER_INSTRFIELDS_T_H_
#define SRC_DOMAINLAYER_INSTRFIELDS_T_H_

#include <string>
#include "DomainLayer/InstrType_e.h"

// Starting/default values of InstrFields_t members.
#define INSTRFIELDS_NO_REG ((uint32_t)(0x7FFFFFFF)) // big, not 16-bit number
#define INSTRFIELDS_NO_IMM ((int32_t)(0x7FFFFFFF))  // big, not 16-bit number

/*
 * Contains potential fields for an instruction in purest form.
 *
 * Type is intended to be used to capture instruction args as separate fields
 * prior to formatting into one binary instruction. It is expected some (but
 * likely not all) fields will be overwritten on instantiation.
 */
typedef struct {
	// Bit fields of an instruction.
	InstrType_e m_opcode = INSTR_INVALID;
	std::string m_flags  = "";
	uint32_t    m_r1     = INSTRFIELDS_NO_REG;
	uint32_t    m_r2     = INSTRFIELDS_NO_REG;
	uint32_t    m_r3     = INSTRFIELDS_NO_REG;
	int32_t     m_imm    = INSTRFIELDS_NO_IMM;
} InstrFields_t;

#endif /* SRC_DOMAINLAYER_INSTRFIELDS_T_H_ */
