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
	m_itemLabel = nullptr;
	m_sym       = nullptr;

	// Parse all items from the stack (ie trusting parser knows its grammar).
	while (itemStack.size() > 0) {
		// (Sanity check item.)
		ItemToken* tkn = itemStack.top();
		if (tkn == nullptr) {Terminate_assert("Null token in LabelNode stack");}

		// Take ownership of items based on type (ie REALLY trusting parser...).
		switch (tkn->m_lexTkn) {
			case TOKEN_LABEL:    m_itemLabel = move(tkn); break;
			default:
				// Likely colon/formatter- no longer needed.
				delete tkn;
				break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// (Sanity check pertinent details.)
	if (m_itemLabel == nullptr) {Terminate_assert("LabelNode has no label");}

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
					  m_itemLabel->m_file,
					  m_itemLabel->m_line,
					  "DeclNode created");
}

//==============================================================================
// Runs local analytics and record-keeping on node's data.
void DeclNode::doLocalAnalysis(DataModel_t& model, SymTable& syms) {
	// Get common vars (ptr pre-checked by ctor).
	string   raw  = m_itemLabel->m_rawStr;
	string   file = m_itemLabel->m_file;
	uint32_t line = m_itemLabel->m_line;

	// Create/save a new symbol.
	RetErr_e retErr = syms.define(*m_itemLabel, m_sym);
	if (m_sym == nullptr) {Terminate_assert("Defined null symbol");}

	// (Log creation.)
	Print::inst().log(LOG_DEBUG, file, line, string("Decl '") + raw + "'");

	// Record error if symbol was already defined (locally).
	if (retErr) {
		string errStr = string("Local re-def '") +
				        m_itemLabel->m_rawStr    +
						"' (first at "           +
						m_sym->m_file            +
						"/"                      +
						to_string(m_sym->m_line) +
						")";
		Print::inst().log(LOG_ERROR, file, line, errStr);
		ModelUtil_recordError(model, RET_L_REDEF);
	}

	// Put symbol (ptr) in model to allow it to be paired.
	model.m_openLabels.push_back(m_sym);
}

//==============================================================================
// Analyzes and links labels/symbols at the local level.
void DeclNode::doLocalLinking(DataModel_t& model, SymTable& syms) {
	// Ensure symbol has a given address space.
	if (m_sym == nullptr) {Terminate_assert("Local linked null symbol");}
	if (m_sym->m_space == ADDR_INVALID) {
		string errStr = string("Unpaired label '") + m_sym->m_name + "'";
		Print::inst().log(LOG_ERROR, m_sym->m_file, m_sym->m_line, errStr);
		ModelUtil_recordError(model, RET_NO_PAIR);
	}
}

//==============================================================================
// Adds node to model's overall program data structures.
void DeclNode::addToProgram(DataModel_t& model) {
	// (Sanity check.)
	if (m_sym == nullptr)  {Terminate_assert("Adding with null symbol");}

	// Add symbol to global table as applicable.
	if (m_sym->m_isGlobal) {
		// Attempt to add to global table.
		RetErr_e retErr = model.m_gSyms.addSym(m_sym->m_name, m_sym);
		if (retErr) {
			model.m_gSyms.getSym(m_sym->m_name, m_sym);
			string errStr = string("Global re-def '") +
					        m_itemLabel->m_rawStr     +
							"' (first at "            +
							m_sym->m_file             +
							"/"                       +
							to_string(m_sym->m_line)  +
							")";
			Print::inst().log(LOG_ERROR,
					          m_itemLabel->m_file,
							  m_itemLabel->m_line,
							  errStr);
			ModelUtil_recordError(model, RET_G_REDEF);
		}
	}

	// Warn if (locally) unused.
	if ((m_sym->m_isGlobal == false) && (m_sym->m_numRefs <= 1)) {
		string wrnStr = "Local unused '" + m_sym->m_name + "'";
		Print::inst().log(LOG_WARNING,
				          m_itemLabel->m_file,
						  m_itemLabel->m_line,
						  wrnStr);
		ModelUtil_recordWarn(model);
	}

	// Otherwise, add node to global program.
	model.m_nodes.push_back(this);
}

//==============================================================================
// Runs last (iterable) checks on global program prior to modification.
void DeclNode::cleanProgram(DataModel_t& model) {
	// Warn if (globally) unused.
	if (m_sym->m_isGlobal && (m_sym->m_numRefs <= 1)) {
		string wrnStr = "Global unused '" + m_sym->m_name + "'";
		Print::inst().log(LOG_WARNING,
				          m_itemLabel->m_file,
						  m_itemLabel->m_line,
						  wrnStr);
		ModelUtil_recordWarn(model);
	}
}

//==============================================================================
// Computes address-related data for model and node.
void DeclNode::genAddresses(DataModel_t& model) {
	// (Sanity check.)
	if (m_sym == nullptr) {Terminate_assert("Addressed null symbol");}

	// Set symbol's address based on it's address space.
	uint32_t addr;
	switch (m_sym->m_space) {
		case ADDR_TEXT: addr = model.m_textSize; break;
		case ADDR_DATA: addr = model.m_dataSize; break;
		default:
			// Unknown address space- bug.
			Terminate_assert("Addressed symbol with no address space");
	}
	m_sym->m_addr = (uint16_t)(addr);

	// (Log address realization).
	string dbgStr = string("Addr '")      +
			        m_itemLabel->m_rawStr +
					"' = "                +
					to_string(addr);
	Print::inst().log(LOG_DEBUG,
			          m_itemLabel->m_file,
			          m_itemLabel->m_line,
					  dbgStr);
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
DeclNode::~DeclNode(void) {
	// Free token (pre-checked by ctor).
	delete m_itemLabel;

	// Free symbol.
	this->freeSymbol(m_sym);
}
