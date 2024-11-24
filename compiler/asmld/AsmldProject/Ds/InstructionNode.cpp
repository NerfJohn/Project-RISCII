/*
 * InstructionNode.cpp: Represents single ISA instruction in assembly.
 */

#include <set>
#include "Device/File.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/IsaUtil.h"
#include "Util/ModelUtil.h"

#include "Ds/InstructionNode.h"

using namespace std;

//==============================================================================
// Helper function to warning about repeated flags in instructions.
std::string InstructionNode::getRepeats(std::string const& str) {
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
// Constructor called by parser. Builds itself directly from action stack.
InstructionNode::InstructionNode(BuildStack_t& itemStack) {
	// (Init members that are set later.)
	m_procOp = INSTR_INVALID;

	// Parse all items from the stack (ie trusting parser knows its grammar).
	while (itemStack.size() > 0) {
		// (Sanity check item.)
		if (itemStack.top() == nullptr) {
			Terminate_assert("Null token while creating InstrNode");
		}

		// Move items based on type (ie REALLY trusting parser...).
		switch (itemStack.top()->m_lexTkn) {
			case TOKEN_FLAGS:
				m_itemFlag = move(itemStack.top());
				break;
			case TOKEN_REGISTER:
				m_itemRegs.push_back(move(itemStack.top()));
				break;
			case TOKEN_IMMEDIATE:
				m_itemImm = move(itemStack.top());
				break;
			default:
				// Assume other token is the opcode.
				m_itemOp = move(itemStack.top());
				break;
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
					  "Instruction node created");
}

//==============================================================================
// Runs local analytics on node's data.
void InstructionNode::doLocalAnalysis(DataModel_t& model) {
	// Analyze the opcode- saving the type in the process.
	m_procOp = IsaUtil_asInstr(m_itemOp->m_lexTkn); // ptr pre-checked by ctor
	if (m_procOp == INSTR_INVALID) {Terminate_assert("Analyzed bad opcode");}

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
			if (IsaUtil_isValidFlag(m_procOp, chr) == false) {
				string errStr = string("Invalid flag '") + chr + "'";
				Print::inst().log(LOG_ERROR, file, line, errStr);
				ModelUtil_recordError(model, RET_BAD_FLG);
			}
		}
	}

	// Validate each register on record.
	for (shared_ptr<ItemToken> itemReg : m_itemRegs) {
		// Get common vars.
		string   raw  = itemReg->m_rawStr; // ptr pre-checked by ctor
		string   file = itemReg->m_file;
		uint32_t line = itemReg->m_line;

		// Analyze register value.
		uint8_t regInt;
		if (IsaUtil_toReg(raw, regInt) == RET_ERR_ERROR) {
			string errStr = string("Invalid register '") + raw + "'";
			Print::inst().log(LOG_ERROR, file, line, errStr);
			ModelUtil_recordError(model, RET_BAD_REG);
		}

		// Save analyzed value (may as well use converted value).
		m_procRegs.push_back(regInt);
	}

	// If there'a an immediate, validate the value.
	if (m_itemImm != nullptr) {
		// Get common vars.
		string   raw  = m_itemImm->m_rawStr;
		string   file = m_itemImm->m_file;
		uint32_t line = m_itemImm->m_line;

		// Analyze the immediate- saving its value for later assembling.
		if (IsaUtil_toImm(raw, m_procImm) == RET_ERR_ERROR) {
			Terminate_assert("Analyzed invalid immediate");
		}
		string dbgStr = string("'") + raw + "' ~ " + to_string(m_procImm.m_val);
		Print::inst().log(LOG_DEBUG, file, line, dbgStr);

		// Validate value- saving it for future generation.
		if (IsaUtil_isValidImm(m_procOp, m_procImm) == false) {
			string errStr = string("Invalid immediate '") + raw + "'";
			Print::inst().log(LOG_ERROR, file, line, errStr);
			ModelUtil_recordError(model, RET_BAD_IMM);
		}
	}
}
