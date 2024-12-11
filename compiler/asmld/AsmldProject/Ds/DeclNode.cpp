/*
 * DeclNode.cpp: Represents declaration of a label/named address.
 */

#include "Device/File.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/IsaUtil.h"
#include "Util/ModelUtil.h"

#include "Ds/DeclNode.h"

using namespace std;

//==============================================================================

// Definitions for interpreting reference counts.
#define NO_REF_CNT (1)

//==============================================================================
// Constructor called by parser. Builds itself directly from action stack.
DeclNode::DeclNode(std::stack<ItemToken*>& itemStack) {
	// (Init members.)
	m_reqLabel = nullptr;
	m_sym      = {};

	// Parse all items from the stack.
	while (itemStack.size() > 0) {
		// Get next item.
		ItemToken* item = itemStack.top();
		IF_NULL(item, "DeclNode() with null item");

		// "Take" item in appropriate spot (REALLY trusting parser, here).
		switch (item->m_lexTkn) {
			case TOKEN_LABEL:    m_reqLabel = item; break;
			default: /* colon */ delete       item; break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// Ensure required items are present.
	IF_NULL(m_reqLabel, "DeclNode() without label");

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
					  m_reqLabel->m_file,
					  m_reqLabel->m_line,
					  "DeclNode created");
}

//==============================================================================
// Analyzes node- validating local arguments/symbols.
void DeclNode::localAnalyze(DataModel_t& model, SymTable& table) {
	// Get common vars.
	string   raw  = m_reqLabel->m_rawStr;
	string   file = m_reqLabel->m_file;
	uint32_t line = m_reqLabel->m_line;

	// Create new symbol.
	m_sym = new Symbol_t();
	IF_NULL(m_sym, "new symbol() failed");
	m_sym->m_name = raw;
	m_sym->m_file = file;
	m_sym->m_line = line;

	// (Log creation.)
	Print::inst().log(LOG_DEBUG, file, line, string("Decl '") + raw + "'");

	// Attempt to add new definition to the table.
	if (table.addSym(raw, m_sym)== RET_ERR_ERROR) {
		// Already defined- get other definition log.
		table.getSym(raw, m_sym);
		IF_NULL(m_sym, "analyze() got null symbol");

		// Log error.
		string errStr = string("Local re-def '") +
				        raw                      +
						"' (first at "           +
						m_sym->m_file            +
						"/"                      +
						to_string(m_sym->m_line) +
						")";
		Print::inst().log(LOG_ERROR, file, line, errStr);
		ModelUtil_recordError(model, RET_L_REDEF);
	}

	// Save "anchor point" of program to model for later.
	if (m_reqLabel->m_rawStr.compare(SYM_START_NAME) == 0) {
		model.m_start = this;
	}

	// Allow label to be paired to other nodes.
	model.m_openLabels.push_back(m_sym);
}

//==============================================================================
// Handles local links/symbols- modifying and linking to local symbols.
void DeclNode::localLink(DataModel_t& model, SymTable& table) {
	// Ensure label is tied to an address space (ie paired).
	IF_NULL(m_sym, "localLink() with null decl symbol");
	if (m_sym->m_space == ADDR_INVALID) {
		string errStr = string("Unpaired label '") + m_sym->m_name + "'";
		Print::inst().log(LOG_ERROR, m_sym->m_file, m_sym->m_line, errStr);
		ModelUtil_recordError(model, RET_NO_PAIR);
	}
}

//==============================================================================
// Finishing program checks- requesting deletions as needed to slim program.
CleanAction_e DeclNode::globalClean(DataModel_t& model) {
	// Warn user if unused (ie only reference is its own pairing).
	IF_NULL(m_sym, "clean() with null decl symbol");
	if (m_sym->m_numRefs == NO_REF_CNT) {
		string wrnStr = "Unused '" + m_sym->m_name + "'";
		Print::inst().log(LOG_WARNING, m_sym->m_file, m_sym->m_line, wrnStr);
		ModelUtil_recordWarn(model);
	}

	// Regardless, keep declaration around.
	return CLEAN_KEEP;
}

//==============================================================================
// Analyze program- generating addresses for each symbol.
void DeclNode::imageAddress(DataModel_t& model) {
	// Set symbol's address based on it's address space.
	IF_NULL(m_sym, "address() with null decl symbol");
	uint32_t addr = 0;
	switch (m_sym->m_space) {
		case ADDR_TEXT: addr = model.m_textSize;                   break;
		case ADDR_DATA: addr = model.m_dataSize;                   break;
		case ADDR_BSS:  addr = model.m_dataSize + model.m_bssSize; break;
		default:
			// Unknown address space- bug.
			Terminate_assert("address() unknown address space");
	}
	m_sym->m_addr = (uint16_t)(addr);

	// If debugging, add space for breakpoint for global text addresses.
	if ((model.m_doDbg)               &&
		(m_sym->m_space == ADDR_TEXT) &&
		(m_sym->m_isGlobal)) {
		model.m_textSize += ISA_WORD_BYTES;
	}

	// (Log address realization).
	string dbgStr = string("Addr '") + m_sym->m_name + "' = " + to_string(addr);
	Print::inst().log(LOG_DEBUG, m_sym->m_file, m_sym->m_line, dbgStr);
}

//==============================================================================
// Assembles program- generating binary values in the data model.
void DeclNode::imageAssemble(DataModel_t& model) {
	// Add (disabled) breakpoint if debugging global text address.
	if ((model.m_doDbg)               &&
		(m_sym->m_space == ADDR_TEXT) &&
		(m_sym->m_isGlobal)) {
		// Create breakpoint.
		Instr_t  fields;
		uint16_t instr  = 0;
		fields.m_opcode = INSTR_NOP;
		if (IsaUtil_genInstr(instr, fields) == RET_ERR_ERROR) {
			Terminate_assert("assemble() failed for breakpoint");
		}

		// Save as part of text section.
		model.m_textVals.push_back(instr);
	}
}

//==============================================================================
// Writes debug table information to (assumed open) binary image.
void DeclNode::optPrintDebug(void) {
	// Debug captures all data and global text addresses.
	IF_NULL(m_sym, "printDebug() with null decl symbol");
	bool isData = (m_sym->m_space == ADDR_DATA) | (m_sym->m_space == ADDR_BSS);
	bool isGlbl = m_sym->m_isGlobal & (m_sym->m_space == ADDR_TEXT);

	// Add symbol's name, space, and address in CSV format.
	if (isData | isGlbl) {
		string spaceText = (isData) ? "DATA" : "TEXT";
		string symStr = m_sym->m_name             +
				        ","                       +
						spaceText                 +
						","                       +
						to_string(m_sym->m_addr)  +
						"\n";
		File::inst().write(symStr);
	}
}

//==============================================================================
// Determines if node (+related section nodes) should be removed.
CleanAction_e DeclNode::optRemoveLabel(DataModel_t& model) {
	// Result of the process.
	CleanAction_e retAct = CLEAN_KEEP;

	// Set labeled section for removal accordingly.
	IF_NULL(m_sym, "removeLabel() with null decl symbol");
	bool hasNoRefs = m_sym->m_numRefs == NO_REF_CNT;
	switch (m_sym->m_space) {
		case ADDR_TEXT: model.m_rmText = hasNoRefs; break;
		case ADDR_DATA: model.m_rmData = hasNoRefs; break;
		case ADDR_BSS:  model.m_rmBss  = hasNoRefs; break;
		default:
			Terminate_assert("removeLabel() with unknown space");
	}

	// Remove label itself if not referenced.
	if (hasNoRefs) {
		// Setup label for removal.
		retAct = CLEAN_DELETE;

		// Log deletion.
		string dbgStr = string("Removing '") + m_reqLabel->m_rawStr + "'";
		Print::inst().log(LOG_DEBUG,
				          m_reqLabel->m_file,
						  m_reqLabel->m_line,
						  dbgStr);
	}

	// Return result of process.
	return retAct;
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
DeclNode::~DeclNode(void) {
	// Free required/known items.
	delete m_reqLabel;

	// Free token as needed (ie common helper logic).
	this->freeSymbol(m_sym);
}
