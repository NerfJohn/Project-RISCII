/*
 * BinaryUtils.cpp
 *
 * "Helper functions for generating binary values from items"
 */

#include "Utils/BinaryUtils.h"

using namespace std;

//==============================================================================

// Common definitions for status return codes.
#define RET_GOOD (0)
#define RET_ERR  (-1)

// Masks sized to bit field lengths.
#define OPCODE_MASK (0xF)  // 4-bit
#define REG_MASK    (0x7)  // 3-bit
#define IMM4_MASK   (0xF)  // 4-bit
#define IMM8_MASK   (0xFF) // 8-bit

// Macros to position bit fields.
#define TO_BIT12(x) ((x) << 12)
#define TO_BIT9(x)  ((x) << 9)
#define TO_BIT8(x)  ((x) << 8)
#define TO_BIT6(x)  ((x) << 6)
#define TO_BIT5(x)  ((x) << 5)
#define TO_BIT4(x)  ((x) << 4)
#define TO_BIT0(x)  ((x) << 0)

// Definitions of bit flags in assembly.
#define ARITH_FLAG  'a'
#define SHIFT_FLAG  's'

//==============================================================================
// Helper function to format "data action with regs" instructions.
uint16_t BinaryUtils_asInstrRRR(InstrFields_t const& fields) {
	// Returned formatted instruction.
	uint16_t retInstr = 0x0000; // implicit 0s for unset bits

	// Size arguments to bit fields.
	uint16_t op = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1 = REG_MASK & fields.m_r1;
	uint16_t r2 = REG_MASK & fields.m_r2;
	uint16_t r3 = REG_MASK & fields.m_r3;

	// Format instruction.
	retInstr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT6(r2) | TO_BIT0(r3);

	// Return formatted instruction.
	return retInstr;
}

//==============================================================================
// Helper function to format "data action with imm4" instructions.
uint16_t BinaryUtils_asInstrRRI4(InstrFields_t const& fields) {
	// Returned formatted instruction.
	uint16_t retInstr = 0x0000; // implicit 0s for unset bits

	// Size arguments to bit fields.
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1  = REG_MASK & fields.m_r1;
	uint16_t r2  = REG_MASK & fields.m_r2;
	uint16_t imm = IMM4_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	retInstr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT6(r2) | TO_BIT0(imm);

	// Immediate format requires flag (implied by immediate in assembly).
	retInstr |= TO_BIT5(0x1);

	// Return formatted instruction.
	return retInstr;
}

//==============================================================================
// Helper function to format "data action with imm8" instructions.
uint16_t BinaryUtils_asInstrRI8(InstrFields_t const& fields) {
	// Returned formatted instruction.
	uint16_t retInstr = 0x0000; // implicit 0s for unset bits

	// Size arguments to bit fields.
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1  = REG_MASK & fields.m_r1;
	uint16_t imm = IMM8_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	retInstr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT0(imm);

	// Returned formatted instruction.
	return retInstr;
}

//==============================================================================
// Generates binary instruction from fields. Doesn't check field accuracy.
int BinaryUtils_genInstr(uint16_t& retInstr, InstrFields_t const& fields) {
	// Return code- asserted for unexpected failures.
	int retCode = RET_GOOD; // innocent till guilty

	// Format non-flag items (ie instruction format base).
	switch (fields.m_opcode) {
     	case INSTR_SHR:
     		// Shift instruction- format based on 3rd operand.
     		retInstr = (fields.m_imm == INSTRFIELDS_NO_IMM) ?
     				   BinaryUtils_asInstrRRR(fields) :
					   BinaryUtils_asInstrRRI4(fields);
     		break;
     	case INSTR_LBI:
     		// Load byte instruction.
     		retInstr = BinaryUtils_asInstrRI8(fields);
     		break;
		default:
			// Unexpected opcode.
			retCode = RET_ERR;
			break;
	}

	// Format flag bits into instruction.
	for (size_t i = 0; i < fields.m_flags.size(); i++) {
		// Set bit based on flag.
		switch (fields.m_flags.at(i)) {
			case ARITH_FLAG: retInstr |= TO_BIT4(0x1); break;
			case SHIFT_FLAG: retInstr |= TO_BIT8(0x1); break;
			default:
				// Unexpected flag.
				retCode = RET_ERR;
				break;
		}
	}

	// Instruction formatted- return result of process.
	return retCode;
}
