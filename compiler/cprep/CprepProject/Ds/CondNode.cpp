/*
 * CondNode.cpp: Node representing conditionally included section of file.
 */

#include "Common/Device/Terminate.h"
#include "Common/Ds/LexToken.h"
#include "Domain/ParseState_e.h"
#include "Util/StrUtil.h"

#include "Ds/CondNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
CondNode::CondNode(std::stack<IBuildItem*>& actStack) {
	// Init members.
	m_reqIfDefined = false;
	m_reqDef       = "";
	m_reqNodes     = {};
	m_type         = PARSE_ACT_COND;
	m_file         = "";
	m_line         = 0;

	// Claim as many items as possible (ie broken from within).
	bool doBreak = false;
	while (actStack.size() > 0) {
		// Get next item.
		IBuildItem* item = actStack.top();
		IF_NULL(item, "CondNode() null item");

		// Claim item if supposed to be part of node.
		switch (item->m_type) {
			case LEX_TKN_ENDIF:
				// End of conditional- of no use.
				break;
			case PARSE_ACT_TEXT:
			case PARSE_ACT_INCL:
			case PARSE_ACT_DEF:
			case PARSE_ACT_COND:
				// Child node- add as such.
				m_reqNodes.push_front((IAstNode*)(item));
				break;
			case LEX_TKN_DEF:
				// Defined name- save name (w/o formatting).
				m_reqDef = ((LexToken*)(item))->m_rawData;
				StrUtil_trim(m_reqDef);
				delete item;
				break;
			case LEX_TKN_IFDEF:
			case LEX_TKN_IFNDEF:
				// Keyword- use to define position/direction/signal break out.
				m_reqIfDefined = (item->m_type == LEX_TKN_IFDEF);
				m_file         = item->m_file;
				m_line         = item->m_line;
				doBreak        = true;
				delete item;
				break;
			default:
				// Shouldn't be here- bug!
				Terminate::inst().assert("CondNode() bad item");
				break;
		}

		// Consume any parsed tokens.
		actStack.pop();

		// Break out as directed.
		if (doBreak) {break;}
	}

	// Ensure defined name was caught.
	if (m_reqDef.size() == 0) {Terminate::inst().assert("DefNode() no name");}
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
CondNode::~CondNode(void) {
	// Delete underlying nodes.
	for (IAstNode* child : m_reqNodes) {delete child;}
}
