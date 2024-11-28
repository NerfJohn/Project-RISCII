/*
 * InstrNode.cpp: Represents single ISA instruction in assembly.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/IsaUtil.h"
#include "Util/ModelUtil.h"

#include "Ds/InstrNode.h"

using namespace std;

//==============================================================================
// Helper function to warning about repeated flags in instructions.
std::string InstrNode::getRepeats(std::string const& str) {
	// Strings for tracking seen/repeated chars.
	string seen;
	string repeats;

	// Look at each char in the given string.
	for (size_t i = 0; i < str.size(); i++) {
		// Extract character.
		uint8_t chr = str.at(i);

		// Save repeated chars ONCE to repeat list.
		if ((seen.find(chr)    != string::npos) &&
			(repeats.find(chr) == string::npos)) {
			repeats += chr;
		}

		// Char now seen.
		seen += chr;
	}

	// Return repeated chars.
	return repeats;
}

//==============================================================================
// Helper function to extract register values (quickly- minimal checks).
void InstrNode::extractRegs(std::vector<uint8_t>& regInts) {
	// Extract each register value (ptrs pre-checked by ctor).
	for (ItemToken* tkn : m_itemRegs) {
		// Value extracted.
		uint8_t regInt;

		// Extract.
		if (IsaUtil_toReg(tkn->m_rawStr, regInt) == RET_ERR_ERROR) {
			Terminate_assert("Extracted bad register value");
		}

		// Save.
		regInts.push_back(regInt);
	}
}

//==============================================================================
// Constructor called by parser. Builds itself directly from action stack.
InstrNode::InstrNode(std::stack<ItemToken*>& itemStack) {
	// (Init members.)
	m_itemOp   = nullptr;
	m_itemFlag = nullptr;
	m_itemRegs = {};
	m_itemImm  = nullptr;

	// Parse all items from the stack (ie trusting parser knows its grammar).
	while (itemStack.size() > 0) {
		// (Sanity check item.)
		ItemToken* tkn = itemStack.top();
		if (tkn == nullptr) {Terminate_assert("Null token in InstrNode stack");}

		// Take ownership of items based on type (ie REALLY trusting parser...).
		switch (tkn->m_lexTkn) {
			case TOKEN_FLAGS:     m_itemFlag = move(tkn);           break;
			case TOKEN_REGISTER:  m_itemRegs.push_front(move(tkn)); break;
			case TOKEN_IMMEDIATE: m_itemImm = move(tkn);            break;
			default: /* opcode */ m_itemOp = move(tkn);             break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// (Sanity check pertinent details.)
	if (m_itemOp == nullptr) {Terminate_assert("InstrNode has no opcode");}

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
					  m_itemOp->m_file,
					  m_itemOp->m_line,
					  "InstrNode created");
}

//==============================================================================
// Runs local analytics on node's data.
void InstrNode::doLocalAnalysis(DataModel_t& model) {
	// Analyze the opcode (ptr pre-checked by ctor).
	InstrType_e procOp = IsaUtil_asInstr(m_itemOp->m_lexTkn);
	if (procOp == INSTR_INVALID) {Terminate_assert("Analyzed bad opcode");}

	// If there are flags, check they are valid.
	if (m_itemFlag != nullptr) {
		// Get common vars.
		string   raw  = m_itemFlag->m_rawStr;
		string   file = m_itemFlag->m_file;
		uint32_t line = m_itemFlag->m_line;

		// Warn about any repeated flags.
		string repeats = this->getRepeats(raw);
		for (char chr : repeats) {
			string warnStr = string("Flag '") + chr + "' repeated";
			Print::inst().log(LOG_WARNING, file, line, warnStr);
			ModelUtil_recordWarn(model);
		}

		// Validate each flag for the instruction.
		for (char chr : raw) {
			if (IsaUtil_isValidFlag(procOp, chr) == false) {
				string errStr = string("Invalid flag '") + chr + "'";
				Print::inst().log(LOG_ERROR, file, line, errStr);
				ModelUtil_recordError(model, RET_BAD_FLG);
			}
		}
	}

	// Validate each register on record (ptrs pre-checked by ctor).
	for (ItemToken* itemReg : m_itemRegs) {
		// Get common vars.
		string   raw  = itemReg->m_rawStr;
		string   file = itemReg->m_file;
		uint32_t line = itemReg->m_line;

		// Analyze register value.
		uint8_t regInt;
		if (IsaUtil_toReg(raw, regInt) == RET_ERR_ERROR) {
			string errStr = string("Invalid register '") + raw + "'";
			Print::inst().log(LOG_ERROR, file, line, errStr);
			ModelUtil_recordError(model, RET_BAD_REG);
		}
	}

	// If there'a an immediate, validate the value.
	if (m_itemImm != nullptr) {this->validateImm(model, *m_itemImm, *m_itemOp);}
}

//==============================================================================
// Computes address-related data for model and node.
void InstrNode::genAddresses(DataModel_t& model) {
	// Instruction takes 1 word in text section.
	model.m_textSize += ISA_WORD_BYTES;
}

//==============================================================================
// Assembles the node, adding its binary data to the model.
void InstrNode::genAssemble(DataModel_t& model) {
	// Instruction to create (+broken out fields).
	uint16_t binInstr = 0x0000;
	Instr_t  fields;

	// Prep register values for populating fields.
	vector<uint8_t> regInts;
	this->extractRegs(regInts);
	size_t numRegs = regInts.size();

	// Add fields.
	fields.m_opcode = IsaUtil_asInstr(m_itemOp->m_lexTkn);
	if (m_itemFlag != nullptr) {fields.m_flags = m_itemFlag->m_rawStr;}
	if (numRegs > 0) {fields.m_r1 = regInts[0];}
	if (numRegs > 1) {fields.m_r2 = regInts[1];}
	if (numRegs > 2) {fields.m_r3 = regInts[2];}
	if (m_itemImm != nullptr) {fields.m_imm = this->getImmVal(*m_itemImm);}

	// "Fields... Assemble!".
	RetErr_e retErr = IsaUtil_genInstr(binInstr, fields);
	if (retErr) {Terminate_assert("Couldn't assemble instruction");}

	// Add instruction to text section.
	model.m_textVals.push_back(binInstr);
}
