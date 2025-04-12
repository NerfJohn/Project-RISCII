/*
 * LitNode.cpp: Node representing literal/constant value.
 */

#include "Common/Device/Terminate.h"
#include "Domain/ParseState_e.h"

#include "Ds/LitNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
LitNode::LitNode(std::stack<IBuildItem*>& actStack) {
	// Init members.
	m_cType.m_type    = PRIMITIVE_LIT;
	m_cType.m_isArray = false;
	m_tkns            = {};
	m_type            = PARSE_ACT_LIT;
	m_file            = "";
	m_line            = 0;

	// Claim as many items as possible (ie broken from within).
	bool doBreak = false;
	while (actStack.size() > 0) {
		// Get next item.
		IBuildItem* item = actStack.top();
		IF_NULL(item, "LitNode() null item");

		// Claim item if supposed to be part of node.
		switch (item->m_type) {
			case LEX_TKN_CHR_LIT:
			case LEX_TKN_INT_LIT:
			case LEX_TKN_STR_LIT:
				// Save literal for processing.
				m_tkns.push_front((LexToken*)(item));
				break;
			case LEX_TKN_LCURLY:
			case LEX_TKN_RCURLY:
			case LEX_TKN_COMMA:
				// Array "literal".
				m_cType.m_isArray = true;
				delete item;
				break;
			default:
				// Unrecognized item- end of literal.
				doBreak = true;
				break;
		}

		// Consume any parsed tokens.
		if (doBreak == false) {actStack.pop();}

		// Break out as directed.
		if (doBreak) {break;}
	}
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
LitNode::~LitNode(void) {
	// Delete held tokens.
	for (LexToken* tkn : m_tkns) {delete tkn;}
}
