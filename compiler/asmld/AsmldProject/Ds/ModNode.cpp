/*
 * ModNode.cpp: Represents modifier of a label/named address.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/ModelUtil.h"

#include "Ds/ModNode.h"

using namespace std;

//==============================================================================
// Constructor called by parser. Builds itself directly from action stack.
ModNode::ModNode(std::stack<ItemToken*>& itemStack) {
	// (Init members.)
	m_reqType  = nullptr;
	m_reqLabel = nullptr;

	// Parse all items from the stack.
	while (itemStack.size() > 0) {
		// Get next item.
		ItemToken* item = itemStack.top();
		IF_NULL(item, "ModNode() with null item");

		// "Take" item in appropriate spot (REALLY trusting parser, here).
		switch (item->m_lexTkn) {
			case TOKEN_LABEL:        m_reqLabel = item; break;
			default: /* directive */ m_reqType  = item; break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// Ensure required items are present.
	IF_NULL(m_reqType,  "ModNode() without directive");
	IF_NULL(m_reqLabel, "ModNode() without label");

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
					  m_reqType->m_file,
					  m_reqType->m_line,
					  "ModNode created");
}

//==============================================================================
// Handles local links/symbols- modifying and linking to local symbols.
void ModNode::localLink(DataModel_t& model, SymTable& table) {
	// Get specified, LOCAL symbol.
	Symbol_t* sym = nullptr;
	if (table.getSym(m_reqLabel->m_rawStr, sym) == RET_ERR_ERROR) {
		string errStr = string("Local no-def '") + m_reqLabel->m_rawStr + "'";
		Print::inst().log(LOG_ERROR,
				          m_reqLabel->m_file,
						  m_reqLabel->m_line,
						  errStr);
		ModelUtil_recordError(model, RET_L_NODEF);
	}

	// Apply modifier (as applicable).
	if (sym != nullptr) {
		// Get common vars.
		string   raw  = m_reqType->m_rawStr;
		string   file = m_reqType->m_file;
		uint32_t line = m_reqType->m_line;

		// Prep for modification.
		bool   prev   = false;
		string dbgStr = "";

		// Modify based on directive.
		switch (m_reqType->m_lexTkn) {
			case TOKEN_KW_GLOBAL:
				prev = sym->m_isGlobal;
				sym->m_isGlobal = true;
				dbgStr = string("'") + m_reqLabel->m_rawStr + "' = global";
				break;
			case TOKEN_KW_WEAK:
				prev = sym->m_isWeak;
				sym->m_isWeak = true;
				dbgStr = string("'") + m_reqLabel->m_rawStr + "' = weak";
				break;
			default:
				// Unknown directive? Bug!
				Terminate_assert("localLink() with unknown mod");
				break;
		}

		// (Log modification.)
		Print::inst().log(LOG_DEBUG, file, line, dbgStr);

		// Warn user if modification is redundant.
		if (prev) {
			Print::inst().log(LOG_WARNING, file, line, "Repeated modifier");
			ModelUtil_recordWarn(model);
		}
	}
}

//==============================================================================
// Finishing program checks- requesting deletions as needed to slim program.
CleanAction_e ModNode::globalClean(DataModel_t& model) {
	// All actions already done- delete the node.
	return CLEAN_DELETE;
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
ModNode::~ModNode(void) {
	// Free required tokens (ie always initialized).
	delete m_reqType;
	delete m_reqLabel;
}
