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
	m_itemType  = nullptr;
	m_itemLabel = nullptr;

	// Parse all items from the stack (ie trusting parser knows its grammar).
	while (itemStack.size() > 0) {
		// (Sanity check item.)
		ItemToken* tkn = itemStack.top();
		if (tkn == nullptr) {Terminate_assert("Null token in ModNode stack");}

		// Take ownership of items based on type (ie REALLY trusting parser...).
		switch (tkn->m_lexTkn) {
			case TOKEN_LABEL:       m_itemLabel = move(tkn); break;
			default: /* modifier */ m_itemType = move(tkn);  break;
		}

		// Item saved/moved- remove from stack.
		itemStack.pop();
	}

	// (Sanity check pertinent details.)
	if (m_itemType == nullptr)  {Terminate_assert("ModNode has no directive");}
	if (m_itemLabel == nullptr) {Terminate_assert("ModNode has no label");}

	// (Log creation.)
	Print::inst().log(LOG_DEBUG,
					  m_itemType->m_file,
					  m_itemType->m_line,
					  "ModNode created");
}

//==============================================================================
// Analyzes and links labels/symbols at the local level.
void ModNode::doLocalLinking(DataModel_t& model, SymTable& syms) {
	// Get common vars (ptr pre-checked in ctor).
	string   raw  = m_itemLabel->m_rawStr;
	string   file = m_itemLabel->m_file;
	uint32_t line = m_itemLabel->m_line;

	// Get modified symbol.
	Symbol_t* sym = nullptr;
	RetErr_e retErr = syms.getSym(raw, sym);
	if (retErr) {
		string errStr = string("Local no-def '") + raw + "'";
		Print::inst().log(LOG_ERROR, file, line, errStr);
		ModelUtil_recordError(model, RET_L_NODEF);
	}

	// Modify symbol appropriately.
	if (retErr == RET_ERR_NONE) {
		// Prep for modification.
		bool   prevBool = false;
		string dbgStr   = "";
		if (sym == nullptr) {Terminate_assert("Modified null node");}

		// Modify (ptr pre-checked in ctor).
		switch (m_itemType->m_lexTkn) {
			case TOKEN_KW_GLOBAL:
				prevBool = sym->m_isGlobal;
				sym->m_isGlobal = true;
				dbgStr = string("'") + raw + "' globalized";
				Print::inst().log(LOG_DEBUG, file, line, dbgStr);
				break;
			default:
				// Unknown modifier. Bug!
				Terminate_assert("Modified with unknown modifier");
				break;
		}

		// Warn if modification is repeated.
		if (prevBool) {
			Print::inst().log(LOG_WARNING, file, line, "Repeated modifier");
			ModelUtil_recordWarn(model);
		}
	}
}

//==============================================================================
// Adds node to model's overall program data structures.
void ModNode::addToProgram(DataModel_t& model) {
	// Modifier already accounted for- instead, just free the memory.
	delete this;
}

//==============================================================================
// General destructor- ensures all memory is freed on deletion.
ModNode::~ModNode(void) {
	// Free tokens (ptrs pre-checked by ctor).
	delete m_itemType;
	delete m_itemLabel;
}
