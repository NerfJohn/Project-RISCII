/*
 * DeclNode.cpp: Represents declaration of a label/named address.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/ModelUtil.h"

#include "Ds/DeclNode.h"

using namespace std;

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
	if (m_sym->m_numRefs == 1) {
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

	// (Log address realization).
	string dbgStr = string("Addr '") + m_sym->m_name + "' = " + to_string(addr);
	Print::inst().log(LOG_DEBUG, m_sym->m_file, m_sym->m_line, dbgStr);
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
DeclNode::~DeclNode(void) {
	// Free required/known items.
	delete m_reqLabel;

	// Free token as needed (ie common helper logic).
	this->freeSymbol(m_sym);
}
