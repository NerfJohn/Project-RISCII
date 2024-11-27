/*
 * IsaUtil.cpp: Utilities derived from the implemented ISA.
 */

#include "Util/IsaUtil.h"

using namespace std;

//==============================================================================

// Definitions for checking valid flags.
#define FLAG_PREFIX  ('%')
#define SHR_FLAGS    string("a")
#define LBI_FLAGS    string("s")
#define BRC_FLAGS    string("nzpc")
#define JPR_FLAGS    string("r")

// Definitions for converting register/immediate strings to ints.
#define REG_PREFIX   ("$")
#define HEX_PREFIX   ("0x")
#define DEC_BASE     (10)
#define HEX_BASE     (16)

// Definitions for range checking integer values.
#define IN_UINT3(x)  ((0 <= (x)) && ((x) <= 7))
#define IN_UINT4(x)  ((0 <= (x)) && ((x) <= 15))

// Masks sized to bit field lengths.
#define OPCODE_MASK (0xF)  // 4-bit
#define REG_MASK    (0x7)  // 3-bit
#define IMM4_MASK   (0xF)  // 4-bit
#define IMM5_MASK   (0x1F) // 5-bit
#define IMM6_MASK   (0x3F) // 6-bit
#define IMM8_MASK   (0xFF) // 8-bit

// Macros to position bit fields.
#define TO_BIT12(x) ((x) << 12)
#define TO_BIT11(x) ((x) << 11)
#define TO_BIT10(x) ((x) << 10)
#define TO_BIT9(x)  ((x) << 9)
#define TO_BIT8(x)  ((x) << 8)
#define TO_BIT6(x)  ((x) << 6)
#define TO_BIT5(x)  ((x) << 5)
#define TO_BIT4(x)  ((x) << 4)
#define TO_BIT0(x)  ((x) << 0)

// Definitions of bit flags in assembly.
#define ARITH_FLAG  'a'

//==============================================================================
// Helper function to generate instruction with "RegRegReg" data format.
static void IsaUtil_toRRR(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1 = REG_MASK & fields.m_r1;
	uint16_t r2 = REG_MASK & fields.m_r2;
	uint16_t r3 = REG_MASK & fields.m_r3;

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT6(r2) | TO_BIT0(r3);
}

//==============================================================================
// Helper function to generate instruction with "RegReg4bIm" data format.
static void IsaUtil_toRR4(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1 = REG_MASK & fields.m_r1;
	uint16_t r2 = REG_MASK & fields.m_r2;
	uint16_t imm = IMM4_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT6(r2) | TO_BIT0(imm);

	// Immediate format requires flag (implied by immediate in assembly).
	instr |= TO_BIT5(0x1);
}

//==============================================================================
// Generate binary ISA instruction from given fields.
RetErr_e IsaUtil_genInstr(uint16_t& instr, Instr_t const& fields) {
	// Result of the process.
	RetErr_e retErr = RET_ERR_NONE; // GUILTY till innocent

	// Extract details for ease of processing.
	string flags = fields.m_flags;
	bool hasImm  = fields.m_imm != INSTR_NO_IMM;

	// Format base instruction.
	switch (fields.m_opcode) {
		case INSTR_SHR:
			if (hasImm) {IsaUtil_toRR4(instr, fields);}
			else        {IsaUtil_toRRR(instr, fields);}
			break;
		default:
			// Unknown instruction- "error" out.
			retErr = RET_ERR_ERROR;
			break;
	}

	// Remove flag prefix (as able to).
	size_t prefixIdx = flags.find(FLAG_PREFIX);
	if (prefixIdx != string::npos) {flags = flags.substr(++prefixIdx);}

	// Format flags.
	for (char flag : flags) {
		switch(flag) {
			case ARITH_FLAG: instr |= TO_BIT4(0x1); break;
			default:
				// Unknown flag- "error" out,
				retErr = RET_ERR_ERROR;
				break;
		}
	}

	// Return result of the process.
	return retErr;
}

//==============================================================================
// Converts lex token into specific instruction opcode.
InstrType_e IsaUtil_asInstr(LexToken_e const tkn) {
	// Instruction for return.
	InstrType_e retInstr = INSTR_INVALID; // guilty till proven innocent

	// Determine matching instruction (switch in case tokens not contiguous).
	switch(tkn) {
		case TOKEN_KW_SHR: retInstr = INSTR_SHR; break;
		default: retInstr = INSTR_INVALID; break; // No matching instruction
	}

	// Return translated instruction.
	return retInstr;
}

//==============================================================================
// Confirms if flag is valid for specified instruction.
bool IsaUtil_isValidFlag(InstrType_e const instr, uint8_t const flag) {
	// Boolean to return.
	bool retBool = false; // GUILTY till innocent

	// Check flag against instruction's available flags.
	switch (instr) {
	    case INSTR_SHR: retBool = (SHR_FLAGS.find(flag) != string::npos); break;
		default:        retBool = false; // no flags supported
	}

	// Allow "%" exception- ease user's need to format their flags.
	if (flag == FLAG_PREFIX) {retBool = true;}

	// Return result of flag analysis.
	return retBool;
}

//==============================================================================
// Converts register token string into an integer.
RetErr_e IsaUtil_toReg(std::string const& regStr, uint8_t& regInt) {
	// Result of the process.
	RetErr_e retErr = RET_ERR_NONE; // INNOCENT till guilty

	// Remove register prefix (as able to).
	string regNum    = regStr;
	size_t prefixIdx = regStr.find(REG_PREFIX);
	if (prefixIdx != string::npos) {regNum = regStr.substr(++prefixIdx);}

	// Attempt to get the integer register value.
	int32_t rawInt;
	try                  {rawInt = (stol(regNum));}
	catch (exception &e) {retErr = RET_ERR_ERROR; e.~exception();}
	regInt = (uint8_t)(rawInt);

	// Ensure integer is valid as a register.
	if (IN_UINT3(rawInt) == 0) {retErr = RET_ERR_ERROR;}

	// Return the result of the process.
	return retErr;
}

//==============================================================================
// Converts immediate token string into an Imm_t type.
RetErr_e IsaUtil_toImm(std::string const& immStr, Imm_t& immInt) {
	// Result of the process.
	RetErr_e retErr = RET_ERR_NONE; // INNOCENT till guilty

	// Determine/set what kind of immediate is being converted.
	immInt.m_isBin = (immStr.find(HEX_PREFIX, 0) != string::npos);
	int base       = (immInt.m_isBin) ? HEX_BASE : DEC_BASE;

	// Attempt to get the integer immediate value.
	try                  {immInt.m_val = stol(immStr, 0, base);}
	catch (exception &e) {retErr = RET_ERR_ERROR; e.~exception();}

	// Return the result of the process.
	return retErr;
}

//==============================================================================
// Confirms if immediate is within valid range for specified instruction.
bool IsaUtil_isValidImm(InstrType_e const instr, Imm_t const& immInt) {
	// Result of check to return.
	bool retBool = false; // GUILTY till innocent

	// Extract immediate data for analysis.
	int32_t val = immInt.m_val;

	// Use instruction to determine check/result.
	switch (instr) {
		case INSTR_SHR: retBool = IN_UINT4(val); break;
		default: retBool = false; // instruction doesn't support immediate
	}

	// Return result.
	return retBool;
}
