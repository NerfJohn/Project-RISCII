/*
 * FuncNode.cpp: Represents "load address" function (built-in 2 instruction op).
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/IsaUtil.h"

#include "Ds/FuncNode.h"

using namespace std;

//==============================================================================
// Constructor called by parser. Builds itself directly from action stack.
FuncNode::FuncNode(std::stack<ItemToken*>& itemStack) {
	// (Init members.)
	m_reqOp     = nullptr;
	m_reqReg    = nullptr;
	m_reqValue  = nullptr;
	m_sym       = nullptr;

	// Parse all items from the stack.
	while (itemStack.size() > 0) {
		// Get next item.
		ItemToken* item = itemStack.top();
		IF_NULL(item, "FuncNode() with null item");

		// "Take" item in appropriate spot (REALLY trusting parser, here).
		switch (item->m_lexTkn) {
			case TOKEN_LABEL:
			case TOKEN_IMMEDIATE:  m_reqValue = item; break;
			case TOKEN_REGISTER:   m_reqReg   = item; break;
			default: /* keyword */ m_reqOp    = item; break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// Ensure required items are present.
	IF_NULL(m_reqOp,     "FuncNode() without opcode");
	IF_NULL(m_reqReg,    "FuncNode() without register");
	IF_NULL(m_reqValue,  "FuncNode() without value");

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
					  m_reqOp->m_file,
					  m_reqOp->m_line,
					  "FuncNode created");
}

//==============================================================================
// Analyzes node- validating local arguments/symbols.
void FuncNode::localAnalyze(DataModel_t& model, SymTable& table){
	// Validate register.
	this->getReg(model, *m_reqReg); // ignore value for now

	// Validate value (based on type).
	switch (m_reqValue->m_lexTkn) {
		case TOKEN_IMMEDIATE: this->getImm(model, *m_reqValue, *m_reqOp); break;
		case TOKEN_LABEL:     /* validated by lexing */                   break;
		default:
			// Unknown value. Bug!
			Terminate_assert("analyze() func with unknown value");
	}

	// Claim any open labels- "text" space for generated instructions.
	this->pairLabels(model, *m_reqOp, ADDR_TEXT);
}

//==============================================================================
// Handles local links/symbols- modifying and linking to local symbols.
void FuncNode::localLink(DataModel_t& model, SymTable& table) {
	// Link locally (as applicable).
	if (m_reqValue->m_lexTkn == TOKEN_LABEL) {
		this->linkLocal(table, *m_reqValue, m_sym);
	}
}

//==============================================================================
// Handles global links/symbols- finishing overall symbol linkage.
void FuncNode::globalLink(DataModel_t& model) {
	// Link locally (as applicable).
	if (m_reqValue->m_lexTkn == TOKEN_LABEL) {
		this->linkGlobal(model, *m_reqValue, m_sym);
	}
}

//==============================================================================
// Analyze program- generating addresses for each symbol.
void FuncNode::imageAddress(DataModel_t& model) {
	// Function generates two instructions- thus, two text words.
	model.m_textSize += (ISA_WORD_BYTES * 2);
}

//==============================================================================
// Assembles program- generating binary values in the data model.
void FuncNode::imageAssemble(DataModel_t& model) {
	// Get the value to load.
	int32_t value = 0;
	switch (m_reqValue->m_lexTkn) {
		case TOKEN_IMMEDIATE:
			// Immediate- extract value.
			value = this->getImm(model, *m_reqValue, *m_reqOp);
			break;
		case TOKEN_LABEL:
			// Label- get from resolved symbol address.
			IF_NULL(m_sym, "assemble() with null func symbol");
			value = m_sym->m_addr;
			break;
		default:
			// Unknown value. Bug!
			Terminate_assert("assemble() func with unknown value");
	}

	// Prep instruction template to generate 2 LBIs.
	uint16_t instr = 0;
	Instr_t fields;
	fields.m_opcode = INSTR_LBI;
	fields.m_r1     = this->getReg(model, *m_reqReg);

	// Generate/save load of upper byte.
	fields.m_imm = (value >> 8);
	if (IsaUtil_genInstr(instr, fields) == RET_ERR_ERROR) {
		Terminate_assert("assemble() failed func's 1st");
	}
	model.m_textVals.push_back(instr);

	// Generate/save load of lower byte.
	fields.m_imm   = value;
	fields.m_flags = "s";
	if (IsaUtil_genInstr(instr, fields) == RET_ERR_ERROR) {
		Terminate_assert("assemble() failed func's 2nd");
	}
	model.m_textVals.push_back(instr);
}

//==============================================================================
// Determines if node (+related section nodes) should be removed.
CleanAction_e FuncNode::optRemoveLabel(DataModel_t& model) {
	// Remove instruction based on model's data.
	return (model.m_rmText) ? CLEAN_DELETE : CLEAN_KEEP;
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
FuncNode::~FuncNode(void) {
	// Free required tokens (ie always initialized).
	delete m_reqOp;
	delete m_reqReg;
	delete m_reqValue;

	// Free symbol (if defined).
	if (m_sym != nullptr) {this->freeSymbol(m_sym);}
}
