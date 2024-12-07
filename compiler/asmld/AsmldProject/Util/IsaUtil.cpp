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
#define IN_INT5(x)   ((-16 <= (x)) && ((x) <= 15))
#define IN_INT6(x)   ((-32 <= (x)) && ((x) <= 31))
#define IN_INT8(x)   ((-128 <= (x)) && ((x) <= 127))
#define IN_INT16(x)  ((-32768 <= (x)) && ((x) <= 32767))
#define IN_UINT3(x)  ((0 <= (x)) && ((x) <= 7))
#define IN_UINT4(x)  ((0 <= (x)) && ((x) <= 15))
#define IN_UINT5(x)  ((0 <= (x)) && ((x) <= 31))
#define IN_UINT6(x)  ((0 <= (x)) && ((x) <= 63))
#define IN_UINT8(x)  ((0 <= (x)) && ((x) <= 255))
#define IN_UINT16(x) ((0 <= (x)) && ((x) <= 65535))

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
#define SHIFT_FLAG  's'
#define COND_N_FLAG 'n'
#define COND_Z_FLAG 'z'
#define COND_P_FLAG 'p'
#define COND_C_FLAG 'c'
#define RTI_FLAG    'r'

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
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1  = REG_MASK & fields.m_r1;
	uint16_t r2  = REG_MASK & fields.m_r2;
	uint16_t imm = IMM4_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT6(r2) | TO_BIT0(imm);

	// Immediate format requires flag (implied by immediate in assembly).
	instr |= TO_BIT5(0x1);
}

//==============================================================================
// Helper function to generate instruction with "RegReg5bIm" data format.
static void IsaUtil_toRR5(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1  = REG_MASK & fields.m_r1;
	uint16_t r2  = REG_MASK & fields.m_r2;
	uint16_t imm = IMM5_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT6(r2) | TO_BIT0(imm);

	// Immediate format requires flag (implied by immediate in assembly).
	instr |= TO_BIT5(0x1);
}

//==============================================================================
// Helper function to generate instruction with "Reg8bIm" data format.
static void IsaUtil_toR8(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1 = REG_MASK & fields.m_r1;
	uint16_t imm = IMM8_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT0(imm);
}

//==============================================================================
// Helper function to generate instruction with "memory" data format.
static void IsaUtil_toMem(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1  = REG_MASK & fields.m_r1;
	uint16_t r2  = REG_MASK & fields.m_r2;
	uint16_t imm = IMM6_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT6(r2) | TO_BIT0(imm);
}

//==============================================================================
// Helper function to generate instruction with "branch" data format.
static void IsaUtil_toBranch(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t imm = IMM8_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT0(imm);
}

//==============================================================================
// Helper function to generate instruction with "jump" data format.
static void IsaUtil_toJump(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1  = REG_MASK & fields.m_r1;
	uint16_t imm = IMM5_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT6(r1) | TO_BIT0(imm);
}

//==============================================================================
// Helper function to generate instruction with "link" data format.
static void IsaUtil_toLink(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);
	uint16_t r1  = REG_MASK & fields.m_r1;
	uint16_t r2  = REG_MASK & fields.m_r2;
	uint16_t imm = IMM5_MASK & (uint16_t)(fields.m_imm);

	// Format instruction.
	instr = TO_BIT12(op) | TO_BIT9(r1) | TO_BIT6(r2) | TO_BIT0(imm);
}

//==============================================================================
// Helper function to generate instruction with "cmd" data format.
static void IsaUtil_toCmd(uint16_t& instr, Instr_t const& fields) {
	// Implicit 0s for unset bits.
	instr = 0x0000;

	// Size arguments to bit fields.
	uint16_t op  = OPCODE_MASK & (uint16_t)(fields.m_opcode);

	// Format instruction.
	instr = TO_BIT12(op);
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
		case INSTR_AND:
		case INSTR_ORR:
		case INSTR_XOR:
		case INSTR_ADD:
		case INSTR_SUB:
			if (hasImm) {IsaUtil_toRR5(instr, fields);}
			else        {IsaUtil_toRRR(instr, fields);}
			break;
		case INSTR_LBI:
			IsaUtil_toR8(instr, fields);
			break;
		case INSTR_SHL:
		case INSTR_SHR:
			if (hasImm) {IsaUtil_toRR4(instr, fields);}
			else        {IsaUtil_toRRR(instr, fields);}
			break;
		case INSTR_LDR:
		case INSTR_STR:
		case INSTR_SWP:
			IsaUtil_toMem(instr, fields);
			break;
		case INSTR_BRC:
			IsaUtil_toBranch(instr, fields);
			break;
		case INSTR_JPR:
			if (flags.size()) {IsaUtil_toCmd(instr, fields);}
			else              {IsaUtil_toJump(instr, fields);}
			break;
		case INSTR_JLR:
			IsaUtil_toLink(instr, fields);
			break;
		case INSTR_NOP:
		case INSTR_HLT:
			IsaUtil_toCmd(instr, fields);
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
			case ARITH_FLAG:  instr |= TO_BIT4(0x1); break;
			case SHIFT_FLAG:  instr |= TO_BIT8(0x1); break;
			case COND_N_FLAG: instr |= TO_BIT11(0x1); break;
			case COND_Z_FLAG: instr |= TO_BIT10(0x1); break;
			case COND_P_FLAG: instr |= TO_BIT9(0x1); break;
			case COND_C_FLAG: instr |= TO_BIT8(0x1); break;
			case RTI_FLAG:    instr |= TO_BIT5(0x1); break;
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
		case TOKEN_KW_AND: retInstr = INSTR_AND; break;
		case TOKEN_KW_ORR: retInstr = INSTR_ORR; break;
		case TOKEN_KW_XOR: retInstr = INSTR_XOR; break;
		case TOKEN_KW_SHL: retInstr = INSTR_SHL; break;
		case TOKEN_KW_SHR: retInstr = INSTR_SHR; break;
		case TOKEN_KW_ADD: retInstr = INSTR_ADD; break;
		case TOKEN_KW_SUB: retInstr = INSTR_SUB; break;
		case TOKEN_KW_LBI: retInstr = INSTR_LBI; break;
		case TOKEN_KW_LDR: retInstr = INSTR_LDR; break;
		case TOKEN_KW_STR: retInstr = INSTR_STR; break;
		case TOKEN_KW_JPR: retInstr = INSTR_JPR; break;
		case TOKEN_KW_JLR: retInstr = INSTR_JLR; break;
		case TOKEN_KW_SWP: retInstr = INSTR_SWP; break;
		case TOKEN_KW_BRC: retInstr = INSTR_BRC; break;
		case TOKEN_KW_NOP: retInstr = INSTR_NOP; break;
		case TOKEN_KW_HLT: retInstr = INSTR_HLT; break;
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
	    case INSTR_LBI: retBool = (LBI_FLAGS.find(flag) != string::npos); break;
	    case INSTR_BRC: retBool = (BRC_FLAGS.find(flag) != string::npos); break;
	    case INSTR_JPR: retBool = (JPR_FLAGS.find(flag) != string::npos); break;
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
	int32_t val   = immInt.m_val;
	bool    isBin = immInt.m_isBin;

	// Use instruction to determine check/result.
	switch (instr) {
		case INSTR_SHL:
		case INSTR_SHR:
			retBool = IN_UINT4(val);
			break;
		case INSTR_AND:
		case INSTR_ORR:
		case INSTR_XOR:
		case INSTR_ADD:
		case INSTR_SUB:
		case INSTR_JPR:
		case INSTR_JLR:
			retBool = (isBin) ? IN_UINT5(val) : IN_INT5(val);
			break;
		case INSTR_LDR:
		case INSTR_STR:
		case INSTR_SWP:
			retBool = (isBin) ? IN_UINT6(val) : IN_INT6(val);
			break;
		case INSTR_LBI:
		case INSTR_BRC:
			retBool = (isBin) ? IN_UINT8(val) : IN_INT8(val);
			break;
		default: retBool = false; // instruction doesn't support immediate
	}

	// Return result.
	return retBool;
}

//==============================================================================
// Confirms if immediate is within valid range of a word length integer.
bool IsaUtil_isValidWord(Imm_t const& immInt) {
	// Result of check to return.
	bool retBool = false; // GUILTY till innocent

	// Determine if value can fit at all in word.
	int32_t val   = immInt.m_val;
	bool    isBin = immInt.m_isBin;
	retBool = (isBin) ? IN_UINT16(val) : IN_INT16(val);

	// Return result.
	return retBool;
}
