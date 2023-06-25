/*
 * AsmMaker.cpp
 */

#include <iomanip>
#include <sstream>
#include <unordered_map>
#include "MsgLog.h"

#include "AsmMaker.h"

using namespace std;

// Internal definitions for ease of processing instructions.
constexpr InstrType_e IS_SPACER 	= (InstrType_e)(-1);
constexpr InstrType_e IS_COMMENT	= (InstrType_e)(-2);
constexpr InstrType_e IS_MACRO 		= (InstrType_e)(-3);
constexpr InstrType_e IS_LABEL 		= (InstrType_e)(-4);

// Common prefixes used for labels in an assembly file.
std::string AsmMaker::FUNC_PREFIX = "f_";
std::string AsmMaker::RET_PREFIX = "ret_";

// Helper functions to generate hexadecimal values for the output.
uint16_t andShl(int val, int msk, int off) {
	return (uint16_t)((val & msk) << off);
}
uint16_t instrD(int op, int r1, int r2, int f, int r3) {
	return andShl(op, 0xf, 12) +
		   andShl(r1, 0x7, 9) +
		   andShl(r2, 0x7, 6) +
		   andShl(f, 0x1, 5) +
		   andShl(r3, 0x1f, 0);
}
uint16_t instrI(int op, int r1, int of) {
	return andShl(op, 0xf, 12) +
		   andShl(r1, 0x7, 9) +
		   andShl(of, 0x1ff, 0);
}
uint16_t instrO(int op, int r1, int r2, int of) {
	return andShl(op, 0xf, 12) +
		   andShl(r1, 0x7, 9) +
		   andShl(r2, 0x7, 6) +
		   andShl(of, 0x3f, 0);
}

// String-ifier for ease of use of instruction type enum in printed messages.
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

// String-ifier for ease of use of condition codes in printed messages.
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

// String-ifier for ease of use of register enum in printed messages.
std::string AsmMaker::regToString(RegName_e reg) {
	// Return string-ified version of register.
	switch(reg) {
		case REG_0: return "$r0"; break;
		case REG_1: return "$r1"; break;
		case REG_2: return "$r2"; break;
		case REG_3: return "$r3"; break;
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

// Saves given instruction (via appending) for later translation.
void AsmMaker::genInstr(instr_t instrData) {
	// Record instruction.
	m_instrList.push_back(instrData);

	// Bookkeeping.
	m_numInstrs++;
}

// Helper function to add formatting/labels to the assembly/hex.
void AsmMaker::addSpacer(void) {
	m_instrList.push_back({.type=IS_SPACER});
}

// Helper function to add formatting/labels to the assembly/hex.
void AsmMaker::addLabel(std::string label) {
	m_instrList.push_back({.type=IS_LABEL, .cmt=label + ":"});
}

// Helper function to add formatting/labels to the assembly/hex.
void AsmMaker::addComment(std::string cmt) {
	m_instrList.push_back({.type=IS_COMMENT, .cmt = "; " + cmt});
}

// Helper function to generate special code cases in the assembly/hex.
void AsmMaker::genCall(std::string funcName) {
	this->genToLabel(AsmMaker::FUNC_PREFIX + funcName);
}

// Helper function to generate special code cases in the assembly/hex.
void AsmMaker::genToRet(std::string funcName) {
	this->genToLabel(AsmMaker::RET_PREFIX + funcName);
}

// Helper function to generate special code cases in the assembly/hex.
void AsmMaker::genToLabel(std::string labelName) {
	// Record macro.
	m_instrList.push_back({.type=IS_MACRO,
		.cmt = "_to    " + labelName});

	// Bookkeeping.
	m_numMacros++;
}

// Helper function to get a new, unique label.
std::string AsmMaker::getNewLabel(void) {
	// Ensure we're not repeating numbers/outputting bad ones.
	if (m_nextLabelNum < 0) {
		MsgLog::logASSERT("Ran out of label numbers- rework assembler");
	}

	// Otherwise, return new label.
	m_nextLabelNum++;
	return "L" + to_string(m_nextLabelNum - 1);
}

// Function to create the assembly file version of the generated input.
void AsmMaker::createAsmFile(std::string filename) {
	// Prepare file to be written.
	FILE* fptr = fopen((filename + ".asm").c_str(), "w");
	if (!fptr) {MsgLog::logERR("Failed to open \"" + filename + ".asm\"");}

	// Write header.
	fprintf(fptr, "; ================================================\n");
	fprintf(fptr, "; %s\n",(filename + ".asm").c_str());
	fprintf(fptr, "; ================================================\n");
	fprintf(fptr, "\n");

	// Write each line to the file.
	for (instr_t entry: m_instrList) {
		// Assume line is a pre-processed comment.
		string nextLine = entry.cmt;

		// Manually process string if an instruction.
		if (entry.type >= INSTR_HLT) {
			// Start the string with the instruction name.
			nextLine = this->instrToString(entry.type);

			// Append flags as needed.
			if ((entry.type == INSTR_SHR) && (entry.flg == FLAG_ARITH)) {
				nextLine += "a";
			}
			else if ((entry.type == INSTR_LBI) && (entry.flg == FLAG_SHIFT)) {
				nextLine += "s";
			}
			else {
				// Append condition codes as needed
				nextLine += this->ccToString(entry.flg);
			}

			// Ensure line is padded for formatting (6 chars + 1 spacer).
			while (nextLine.size() % 7) {nextLine += " ";}

			// Add registers as requested.
			if (entry.r1 != BAD_REG) {
				nextLine += this->regToString(entry.r1) + "    ";
			}
			if (entry.r2 != BAD_REG) {
				nextLine += this->regToString(entry.r2) + "    ";
			}
			if (entry.r3 != BAD_REG) {
				nextLine += this->regToString(entry.r3) + "    ";
			}

			// Add immediate as requested.
			if (entry.imm < 0x10000) {nextLine += to_string(entry.imm);}

			// Add comment if present.
			if (entry.cmt.size() > 0) {
				// Pad for formatting (all instruction fit in 28 chars).
				while (nextLine.size() % 28) {nextLine += " ";}

				// Add comment.
				nextLine += "; " + entry.cmt;
			}
		}

		// Write the line.
		fprintf(fptr, nextLine.c_str());
		fprintf(fptr, "\n");
	}

	// Finished writing- close the file.
	fclose(fptr);
}

// Function to create the hex file version of the generated input.
void AsmMaker::createHexFile(std::string filename) {

	// Phase 1: Label Address Realization //

	// Scan the list to computer label addresses.
	unordered_map<string, uint16_t> labelMap;
	uint16_t curAddr = 0;
	for (instr_t entry: m_instrList) {
		// Process based on type.
		InstrType_e type = entry.type;
		if (type >= INSTR_HLT) {curAddr += sizeof(uint16_t);}
		else if (type == IS_MACRO) {curAddr += 3 * sizeof(uint16_t);}
		else if (type == IS_LABEL) {
			labelMap["_to    " + entry.cmt] = curAddr;

			MsgLog::logINFO("Label \"" + entry.cmt +
					"\" assigned to address " + to_string(curAddr));
		}
		/* Other lines are irrelevant */
	}

	// Phase 2: Generate Hex file //

	// Hex-ify and write each instruction/macro.
	vector<uint16_t> hexInstrs;
	for (instr_t entry: m_instrList) {
		// Process based on type.
		InstrType_e type = entry.type;
		if (type == IS_MACRO) {
			// Get target address.
			uint16_t addr = labelMap[entry.cmt + ":"];

			// Generate three instructions.
			hexInstrs.push_back(instrI(INSTR_LBI, REG_RA, addr >> 8));
			hexInstrs.push_back(instrI(INSTR_LBI, REG_RA,
					0x100 + (addr & 0xff)));
			hexInstrs.push_back(instrO(INSTR_JLR, REG_RA, REG_RA, 0));
		}
		else if (type >= INSTR_HLT) {
			// Generate based on type.
			int flag;
			switch (type) {
				case INSTR_AND:
				case INSTR_ORR:
				case INSTR_XOR:
				case INSTR_SHL:
				case INSTR_ADD:
				case INSTR_SUB:
					// Use of immediate changes format.
					if (entry.r3 == BAD_REG) {
						hexInstrs.push_back(instrD(type, entry.r1, entry.r2,
								0x1, entry.imm));
					}
					else {
						hexInstrs.push_back(instrD(type, entry.r1, entry.r2,
								0x0, entry.r3));
					}
					break;
				case INSTR_SHR:
					// Use of immediate changes format.
					flag = entry.flg << 4;
					if (entry.r3 == BAD_REG) {
						hexInstrs.push_back(instrD(type, entry.r1, entry.r2,
								0x1, flag + entry.imm));
					}
					else {
						hexInstrs.push_back(instrD(type, entry.r1, entry.r2,
								0x0, flag + entry.r3));
					}
					break;
				case INSTR_LBI:
					flag = entry.flg << 8;
					hexInstrs.push_back(instrI(type, entry.r1,
							flag + (entry.imm & 0xff)));
					break;
				case INSTR_JAL:
					hexInstrs.push_back(instrI(type, entry.r1, entry.imm));
					break;
				case INSTR_LDR:
				case INSTR_STR:
				case INSTR_JLR:
					hexInstrs.push_back(instrO(type, entry.r1,
							entry.r2, entry.imm));
					break;
				case INSTR_BRC:
					hexInstrs.push_back(instrI(type, entry.flg, entry.imm));
					break;
				case INSTR_JPR:
					hexInstrs.push_back(instrO(type, 0, entry.r1, entry.imm));
					break;
				case INSTR_NOP:
				case INSTR_HLT:
					hexInstrs.push_back(andShl(type, 0xf, 12));
					break;
				default:
					MsgLog::logASSERT("Bad type in createHexFile()= " +
							to_string((int)type));
			}
		}
		/* Other lines are irrelevant */
	}

	// Phase 3: Write Hex file //

	// Prepare file to be written.
	FILE* fptr = fopen((filename + ".hex").c_str(), "w");
	if (!fptr) {MsgLog::logERR("Failed to open \"" + filename + ".hex\"");}

	// Write each instruction (one byte per line- MSB first).
	for (uint16_t instr: hexInstrs) {
		// Convert to hex string.
		stringstream ss;
		ss << std::setfill('0') << std::setw(4) << std::hex << instr;

		// Write instruction to file.
		fprintf(fptr, "%s\n",ss.str().substr(0,2).c_str());
		fprintf(fptr, "%s\n",ss.str().substr(2,2).c_str());
	}

	// Finished writing- close the file.
	fclose(fptr);
}
