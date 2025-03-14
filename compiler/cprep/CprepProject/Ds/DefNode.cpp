/*
 * DefNode.cpp: Node representing preprocessor (only) variable definition.
 */

#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Ds/LexToken.h"
#include "Common/Util/InfoUtil.h"
#include "Common/Util/StrUtil.h"
#include "Domain/ParseState_e.h"
#include "Domain/RetCode_e.h"

#include "Ds/DefNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
DefNode::DefNode(std::stack<IBuildItem*>& actStack) {
	// Init members.
	m_reqDef   = "";
	m_defSym   = "";
	m_type     = PARSE_ACT_DEF;
	m_file     = "";
	m_line     = 0;

	// Claim as many items as possible (ie broken from within).
	bool doBreak = false;
	while (actStack.size() > 0) {
		// Get next item.
		IBuildItem* item = actStack.top();
		IF_NULL(item, "DefNode() null item");

		// Claim item if supposed to be part of node.
		switch (item->m_type) {
			case LEX_TKN_DEF:
				// Defined name- save name (w/o formatting).
				m_reqDef = ((LexToken*)(item))->m_rawData;
				StrUtil_trim(m_reqDef);
				break;
			case LEX_TKN_DEFINE:
				// Keyword- use to define position/signal break out.
				m_file = item->m_file;
				m_line = item->m_line;
				doBreak = true;
				break;
			default:
				// Shouldn't be here- bug!
				Terminate::inst().assert("DefNode() bad item");
				break;
		}

		// Consume any parsed tokens.
		actStack.pop();
		delete item;

		// Break out as directed.
		if (doBreak) {break;}
	}

	// Ensure defined name was caught.
	if (m_reqDef.size() == 0) {Terminate::inst().assert("DefNode() no name");}
}

//==============================================================================
// Verifies definitions/conditions are correct.
void DefNode::checkDefines(DataModel_t& model) {
	// Variable can only be defined once (per source file).
	string* sym = (string*)(model.m_defs.get(m_reqDef));
	if (sym != nullptr) {
		// Already defined!
		string errStr = string("Re-def '") +
				        m_reqDef           +
						"' (first at "     +
						*sym               +
						")";
		Print::inst().log(LOG_ERROR, m_file, m_line, errStr);
		InfoUtil_recordError(model.m_summary, RET_TWO_DEF);
	}
	else {
		// Otherwise, add to definitions.
		string dbgStr = string("Defined '") + m_reqDef + "'";
		Print::inst().log(LOG_DEBUG, m_file, m_line, dbgStr);
		m_defSym = m_file + ":" + to_string(m_line);
		model.m_defs.addLocal(m_reqDef, &m_defSym);
	}
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
DefNode::~DefNode(void) {/* no actions */}
