/*
 * FuncNode.cpp: Node representing a function (declaration or definition).
 */

#include "Domain/ParseState_e.h"
#include "Util/AppUtil.h"

#include "Ds/FuncNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
FuncNode::FuncNode(std::stack<Ptr<IBuildItem>>& actStack) {
	// Init members.
	m_typeTkns = {};
	m_name     = Ptr<LexToken>(nullptr);

	m_type = PARSE_ACT_FDEC;
	m_file = "";
	m_line = 0;

	// Claim relevant nodes/tokens.
	bool doBreak = false;
	while (actStack.size() > 0) {
		// Get next item.
		Ptr<IBuildItem> item = actStack.top();

		// Claim (as applicable).
		switch(item->m_type) {
			case LEX_TKN_ID:
				// Func ID.
				m_name = item.toType<LexToken>();
				break;
			case LEX_TKN_INT:
			case LEX_TKN_CHAR:
				// Type + end of def/decl.
				m_typeTkns.push_front(item.toType<LexToken>());
				m_file  = item->m_file;
				m_line  = item->m_line;
				doBreak = true;
				break;
			default:
				// Unknown type- bug!
				AppUtil_exitBug("FuncNode() bad type");
				break;
		}

		// Consume with viewing.
		actStack.pop();

		// Stop when signaled.
		if (doBreak) {break;}
	}

	// Verify critical pieces.
	if (0 == m_typeTkns.size()) {AppUtil_exitBug("FuncNode() no type");}
	if (m_name.isNull())        {AppUtil_exitBug("FuncNode() no name");}
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
FuncNode::~FuncNode(void) {/* no actions */}
