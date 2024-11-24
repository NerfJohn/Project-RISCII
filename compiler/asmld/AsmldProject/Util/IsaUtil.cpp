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
	try                  {regInt = (uint8_t)(stol(regNum));}
	catch (exception &e) {retErr = RET_ERR_ERROR; e.~exception();}

	// Ensure integer is valid as a register.
	if (IN_UINT3(regInt) == 0) {retErr = RET_ERR_ERROR;}

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
