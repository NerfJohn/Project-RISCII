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
// Helper function to warn about repeated flags in instruction.
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
// Constructor called by parser. Builds itself directly from action stack.
InstrNode::InstrNode(std::stack<ItemToken*>& itemStack) {
	// (Init members.)
	m_reqOp   = nullptr;
	m_optFlag = nullptr;
	m_optRegs = {};
	m_optImm  = nullptr;

	// Parse all items from the stack.
	while (itemStack.size() > 0) {
		// Get next item.
		ItemToken* item = itemStack.top();
		IF_NULL(item, "InstrNode() with null item");

		// "Take" item in appropriate spot (REALLY trusting parser, here).
		switch (item->m_lexTkn) {
			case TOKEN_FLAGS:     m_optFlag = item;           break;
			case TOKEN_REGISTER:  m_optRegs.push_front(item); break;
			case TOKEN_IMMEDIATE: m_optImm  = item;           break;
			default: /* opcode */ m_reqOp   = item;           break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// Ensure required items are present.
	IF_NULL(m_reqOp, "InstrNode() without opcode");

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
			          m_reqOp->m_file,
					  m_reqOp->m_line,
					  "InstrNode created");
}

//==============================================================================
// Analyzes node- validating local arguments/symbols.
void InstrNode::localAnalyze(DataModel_t& model, SymTable& table) {
	// Validate opcode.
	InstrType_e procOp = IsaUtil_asInstr(m_reqOp->m_lexTkn);
	if (procOp == INSTR_INVALID) {Terminate_assert("analyze() bad opcode");}

	// Validate flags as present.
	if (m_optFlag != nullptr) {
		// Get common vars.
		string   raw  = m_optFlag->m_rawStr;
		string   file = m_optFlag->m_file;
		uint32_t line = m_optFlag->m_line;

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

	// Validate registers as present.
	for (ItemToken* item : m_optRegs) {
		IF_NULL(item, "analyze() null instr reg");
		this->getReg(model, *item);                // ignore val for now
	}

	// Validate immediate as present.
	if (m_optImm != nullptr) {
		this->getImm(model, *m_optImm, *m_reqOp);
	}

	// Claim any open/unpaired labels to "program space" instruction.
	this->pairLabels(model, *m_reqOp, ADDR_TEXT);
}

//==============================================================================
// Analyze program- generating addresses for each symbol.
void InstrNode::imageAddress(DataModel_t& model) {
	// Instruction takes 1 word in text section.
	model.m_textSize += ISA_WORD_BYTES;
}

//==============================================================================
// Assembles program- generating binary values in the data model.
void InstrNode::imageAssemble(DataModel_t& model) {
	// Instruction to create (+broken out fields).
	uint16_t binInstr = 0x0000;
	Instr_t  fields;

	// Prep register values for populating fields.
	vector<uint8_t> regInts;
	for (ItemToken* item : m_optRegs) {
		IF_NULL(item,"assemble() null instr reg");
		regInts.push_back(this->getReg(model, *item));
	}
	size_t numRegs = regInts.size();

	// Add fields.
	fields.m_opcode = IsaUtil_asInstr(m_reqOp->m_lexTkn);
	if (m_optFlag != nullptr) {fields.m_flags = m_optFlag->m_rawStr;}
	if (numRegs > 0) {fields.m_r1 = regInts[0];}
	if (numRegs > 1) {fields.m_r2 = regInts[1];}
	if (numRegs > 2) {fields.m_r3 = regInts[2];}
	if (m_optImm != nullptr) {
		fields.m_imm = this->getImm(model, *m_optImm, *m_reqOp);
	}

	// "Fields... Assemble!".
	RetErr_e retErr = IsaUtil_genInstr(binInstr, fields);
	if (retErr) {Terminate_assert("assemble() failed instr");}

	// Add instruction to text section.
	model.m_textVals.push_back(binInstr);
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
InstrNode::~InstrNode(void) {
	// Free required items (ie known to need freeing).
	delete m_reqOp;

	// Free other items as necessary.
	for (ItemToken* item : m_optRegs) {if (item != nullptr) {delete item;}    }
	if (m_optFlag != nullptr)                               {delete m_optFlag;}
	if (m_optImm  != nullptr)                               {delete m_optImm; }
}
