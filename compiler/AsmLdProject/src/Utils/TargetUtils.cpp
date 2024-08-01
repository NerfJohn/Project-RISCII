/*
 * TargetUtils.cpp
 *
 * "Helper functions for tasks involving target specific information"
 */

#include "Utils/TargetUtils.h"

using namespace std;

//==============================================================================

// Definitions for checking/reporting flags per instruction.
#define SHR_FLAGS string("a")

// Definitions for limits on int/uint values.
#define IN_UINT3_RANGE(x)  ((0 <= (x)) && ((x) <= 7))
#define IN_UINT4_RANGE(x)  ((0 <= (x)) && ((x) <= 15))
#define IN_UINT16_RANGE(x) ((0 <= (x)) && ((x) <= 65535))
#define IN_INT16_RANGE(x)  ((-32768 <= (x)) && ((x) <= 32767))

// Definitions for types of int/uint values.
#define NO_TYPE  string("NULL")
#define UINT3    string("uint3")
#define UINT4    string("uint4")
#define UINT16   string("uint16")
#define ANYINT16 string("(u)int16")


// Definitions for sizes of elements.
#define INSTR_SIZE_BYTES (2)
#define BIN_MAX_BYTES    (65536) // 64 KB binary image for target
#define TEXT_MAX_BYTES   (65530) // 16-bit address minus metadata and data word

//==============================================================================
// Converts token into instruction type. Returns INSTR_INVALID on failure.
InstrType_e TargetUtils_asInstr(LexToken_e token) {
	// Instruction for return.
	InstrType_e retInstr = INSTR_INVALID; // guilty till proven innocent

	// Determine matching instruction (switch in case tokens not contiguous).
	switch(token) {
		case TOKEN_SHL: retInstr = INSTR_SHL; break;
		case TOKEN_SHR: retInstr = INSTR_SHR; break;
		default: retInstr = INSTR_INVALID; break; // No matching instruction
	}

	// Return translated instruction.
	return retInstr;
}

//==============================================================================
// Validates a bit flag for a given instruction.
bool TargetUtils_isValidFlag(InstrType_e instr, uint8_t flag) {
	// Boolean to return.
	bool retBool = false; // guilty till proven innocent

	// Check flag against instruction's available flags.
	switch (instr) {
	    case INSTR_SHR: retBool = (SHR_FLAGS.find(flag) != string::npos); break;
	    case INSTR_SHL: // fall-through
		default:        retBool = false; // no flags supported
	}

	// Return result of flag analysis.
	return retBool;
}

//==============================================================================
// Gets string of valid flag options for an instruction.
std::string TargetUtils_getInstrFlags(InstrType_e instr) {
	// String to return.
	string retStr;

	// Get instruction's available flags.
	switch (instr) {
	    case INSTR_SHR: retStr = SHR_FLAGS; break;
	    case INSTR_SHL: // fall-through
		default: break; // no flags supported
	}

	// Return available flags.
	return retStr;
}

//==============================================================================
// Validates if uint is within range of a register.
bool TargetUtils_isValidReg(uint32_t reg) {
	// Register value must fall within register file hardware bounds.
	return IN_UINT3_RANGE(reg);
}

//==============================================================================
// Gets type/range of register as string.
std::string TargetUtils_getRegType(void) {
	// Simply return its typing.
	return UINT3;
}

//==============================================================================
// Validates if int can is within valid range for a given instruction.
bool TargetUtils_isValidImm(InstrType_e instr, int32_t imm) {
	// Result of check to return.
	bool retBool = false; // guilty till proven innocent

	// Use instruction to determine check/result.
	switch (instr) {
		case INSTR_SHL: // fall-through
		case INSTR_SHR: retBool = IN_UINT4_RANGE(imm); break;
		default: retBool = false; // instruction doesn't support immediate
	}

	// Return result.
	return retBool;
}

//==============================================================================
// Gets type/range of immediate for given instruction as string.
std::string TargetUtils_getImmType(InstrType_e instr) {
	// String to return.
	string retStr;

	// Get instruction's range type.
	switch (instr) {
		case INSTR_SHL: // fall-through
	    case INSTR_SHR: retStr = UINT4; break;
		default:  retStr = NO_TYPE; break; // no immediate type supported
	}

	// Return available flags.
	return retStr;
}

//==============================================================================
// Validates if int is within valid range for given data word.
bool TargetUtils_isValidWord(DataWordType_e range, int32_t word) {
	// Result of check to return.
	bool retBool = false; // guilty till proven innocent

	// Use instruction to determine check/result.
	switch (range) {
		case DATA_WORD_UNSIGNED:
			retBool = IN_UINT16_RANGE(word);
			break;
		case DATA_WORD_ANY:
			retBool = IN_UINT16_RANGE(word) | IN_INT16_RANGE(word);
			break;
		default: retBool = false; // instruction doesn't support word
	}

	// Return result.
	return retBool;
}

//==============================================================================
// Gets type/range specified as a string.
std::string TargetUtils_getWordType(DataWordType_e range) {
	// String to return.
	string retStr;

	// Get instruction's range type.
	switch (range) {
	    case DATA_WORD_UNSIGNED: retStr = UINT16; break;
	    case DATA_WORD_ANY:      retStr = ANYINT16; break;
		default:  retStr = NO_TYPE; break; // no word type supported
	}

	// Return available flags.
	return retStr;
}
