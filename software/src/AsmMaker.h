/*
 * AsmMaker.h
 */

#ifndef ASMMAKER_H_
#define ASMMAKER_H_

#include <string>
#include <vector>

// Enum defining available instructions (with their opcode value).
enum InstrType_e {
	BAD_INSTR = -1,
	INSTR_HLT = 0,
	INSTR_NOP = 1,
	INSTR_JAL = 2,
	INSTR_BRC = 3,
	INSTR_LDR = 4,
	INSTR_JPR = 5,
	INSTR_JLR = 6,
	INSTR_STR = 7,
	INSTR_ADD = 8,
	INSTR_SUB = 9,
	INSTR_LBI = 10,
	INSTR_XOR = 11,
	INSTR_SHL = 12,
	INSTR_SHR = 13,
	INSTR_ORR = 14,
	INSTR_AND = 15,
};

// Enum defining register in register file (with their field value).
enum RegName_e {
	BAD_REG	= -1,
	REG_0 	= 0,
	REG_1 	= 1,
	REG_2 	= 2,
	REG_3 	= 3,
	REG_SP 	= 4, // Stack pointer
	REG_FP 	= 5, // Frame pointer
	REG_AC 	= 6, // Accumulator
	REG_RA 	= 7, // Return Address/Spare Register
};

// Enum defining valid instruction flags (with their field value).
enum InstrFlag_e {
	FLAG_NONE	= 0x0,
	FLAG_ARITH 	= 0x1,
	FLAG_SHIFT 	= 0x1,
	FLAG_P		= 0x1,
	FLAG_Z		= 0x2,
	FLAG_ZP		= 0x3,
	FLAG_N		= 0x4,
	FLAG_NP		= 0x5,
	FLAG_NZ		= 0x6,
	FLAG_NZP	= 0x7,
};

// Struct defining possible fields of an assembly instruction.
typedef struct instr {
	InstrType_e type	= BAD_INSTR;
	InstrFlag_e flg		= FLAG_NONE;
	RegName_e r1		= BAD_REG;
	RegName_e r2		= BAD_REG;
	RegName_e r3		= BAD_REG;
	int imm				= 0x10000;
	std::string cmt;
} instr_t;

// Class to handle translation of instructions into assembly or hexadecimal.
class AsmMaker {
public:
	// Common prefixes used for labels in an assembly file.
	static std::string FUNC_PREFIX;
	static std::string RET_PREFIX;

	// Saves given instruction (via appending) for later translation.
	void genInstr(instr_t instrData);

	// Helper functions to add formatting/labels to the assembly/hex.
	void addSpacer(void);
	void addComment(std::string cmt);
	void addLabel(std::string label);

	// Helper functions to generate special code cases in the assembly/hex.
	void genCall(std::string funcName);
	void genToRet(std::string funcName);
	void genToLabel(std::string labelName);

	// Helper function to get a new, unique label.
	std::string getNewLabel(void);

	// Functions to create the desired file based on the generated input.
	void createAsmFile(std::string filename);
	void createHexFile(std::string filename);

	// Getter for the number of instructions/labels/etc currently stored.
	int numLines(void) {return m_instrList.size();}

	// String-ifiers for ease of use of enums in printed messages.
	std::string instrToString(InstrType_e instr);
	std::string ccToString(InstrFlag_e flag);
	std::string regToString(RegName_e reg);

	// Getters for the number of instructions/macros currently stored.
	std::uint32_t numInstrs(void) {return m_numInstrs;}
	std::uint32_t numMacros(void) {return m_numMacros;}

private:
	// Next number available for use in generating unique labels.
	int m_nextLabelNum = 0;

	// Ordered list of instructions/labels/etc to be translated.
	std::vector<instr_t> m_instrList;

	// Counters for valuable/pertinent data.
	std::uint32_t m_numInstrs = 0;
	std::uint32_t m_numMacros = 0;
};

#endif /* ASMMAKER_H_ */
