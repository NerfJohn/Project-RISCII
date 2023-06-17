/*
 * AsmMaker.cpp
 */

#include "MsgLog.h"
#include "AsmMaker.h"

using namespace std;

// TODO
constexpr InstrType_e IS_SPACER 	= (InstrType_e)(-1);
constexpr InstrType_e IS_COMMENT	= (InstrType_e)(-2);
constexpr InstrType_e IS_MACRO 		= (InstrType_e)(-3);
constexpr InstrType_e IS_LABEL 		= (InstrType_e)(-4);

// TODO
std::string AsmMaker::FUNC_PREFIX = "f_";
std::string AsmMaker::RET_PREFIX = "ret_";

// TODO
std::string AsmMaker::instrToString(InstrType_e instr) {
	// Return string-ified version of instruction.
	switch (instr) {
		case INSTR_HLT: return "HLT"; break;
		case INSTR_NOP: return "NOP"; break;
		case INSTR_JAL: return "JAL"; break;
		case INSTR_BRC: return "BRC"; break;
		case INSTR_LDR: return "LDR"; break;
		case INSTR_JPR: return "JPR"; break;
		case INSTR_JLR: return "JLR"; break;
		case INSTR_STR: return "STR"; break;
		case INSTR_ADD: return "ADD"; break;
		case INSTR_SUB: return "SUB"; break;
		case INSTR_LBI: return "LBI"; break;
		case INSTR_XOR: return "XOR"; break;
		case INSTR_SHL: return "SHL"; break;
		case INSTR_SHR: return "SHR"; break;
		case INSTR_ORR: return "ORR"; break;
		case INSTR_AND: return "AND"; break;
		default:
			MsgLog::logASSERT("Bad AsmMaker::instrToString() value="
					+ to_string((int)(instr)));
	}

	// (Appease the compiler.)
	return "";
}

// TODO
std::string AsmMaker::ccToString(InstrFlag_e flag) {
	// String to return.
	string retStr;

	// Add each flag if present.
	if (flag & 0x4) {retStr += "n";}
	if (flag & 0x2) {retStr += "z";}
	if (flag & 0x1) {retStr += "p";}

	// Return string-ified condition code.
	return retStr;
}

// TODO
std::string AsmMaker::regToString(RegName_e reg) {
	// Return string-ified version of register.
	switch(reg) {
		case REG_0: return "$0"; break;
		case REG_1: return "$1"; break;
		case REG_2: return "$2"; break;
		case REG_3: return "$3"; break;
		case REG_SP: return "$sp"; break;
		case REG_FP: return "$fp"; break;
		case REG_AC: return "$ac"; break;
		case REG_RA: return "$ra"; break;
		default:
			MsgLog::logASSERT("Bad AsmMaker::regToString() value="
					+ to_string((int)(reg)));
	}

	// (Appease the compiler.)
	return "";
}

// TODO
void AsmMaker::genInstr(instr_t instrData) {
	// Record instruction.
	m_instrList.push_back(instrData);

	// Bookkeeping.
	m_numInstrs++;
}

// TODO
void AsmMaker::addSpacer(void) {
	m_instrList.push_back({.type=IS_SPACER});
}

// TODO
void AsmMaker::addLabel(std::string label) {
	m_instrList.push_back({.type=IS_LABEL, .cmt=label});
}

// TODO
void AsmMaker::addComment(std::string cmt) {
	m_instrList.push_back({.type=IS_COMMENT, .cmt = cmt});
}

// TODO
void AsmMaker::genCall(std::string funcName) {
	this->genToLabel(AsmMaker::FUNC_PREFIX + funcName);
}

// TODO
void AsmMaker::genToRet(std::string funcName) {
	this->genToLabel(AsmMaker::RET_PREFIX + funcName);
}

// TODO
void AsmMaker::genToLabel(std::string labelName) {
	// Record macro.
	m_instrList.push_back({.type=IS_MACRO,
		.cmt = labelName});

	// Bookkeeping.
	m_numMacros++;
}

// TODO
std::string AsmMaker::getNewLabel(void) {
	// Ensure we're not repeating numbers/outputting bad ones.
	if (m_nextLabelNum < 0) {
		MsgLog::logASSERT("Ran out of label numbers- rework assembler");
	}

	// Otherwise, return new label.
	m_nextLabelNum++;
	return "L" + to_string(m_nextLabelNum - 1);
}

// TODO
void AsmMaker::createAsmFile(std::string filename) {
	// TODO- For now- just print out results.
	for (instr_t entry: m_instrList) {
		string instr;
		if (entry.type == IS_COMMENT) {instr += ";" + entry.cmt;}
		else if (entry.type == IS_MACRO) {instr += "_to " + entry.cmt;}
		else if (entry.type == IS_LABEL) {instr += entry.cmt + ":";}
		else if (entry.type == IS_SPACER) {instr += "";}
		else {
			instr += this->instrToString(entry.type);
			if (entry.flg > FLAG_NONE) {instr += "\t-0x" + to_string((int)entry.flg);}
			if (entry.r1 > BAD_REG) {instr += "\t" + this->regToString(entry.r1);}
			if (entry.r2 > BAD_REG) {instr += "\t" + this->regToString(entry.r2);}
			if (entry.r3 > BAD_REG) {instr += "\t" + this->regToString(entry.r3);}
			if (entry.imm != 0x10000) {instr += "\t" + to_string((int)entry.imm);}
			if (entry.cmt.size()) {instr += "\t;" + entry.cmt;}
		}

		MsgLog::logINFO(instr);
	}
}

// TODO
void AsmMaker::createHexFile(std::string filename) {/* TODO */}
