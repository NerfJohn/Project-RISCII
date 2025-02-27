/*
 * InclNode.cpp: Node representing file inclusion.
 */

#include "Common/Device/Terminate.h"
#include "Common/Ds/LexToken.h"
#include "Domain/ParseState_e.h"
#include "Util/StrUtil.h"

#include "Ds/InclNode.h"

using namespace std;

//==============================================================================

// Definitions for creating node.
#define FORMAT_CHAR '"'

//==============================================================================
// Constructor- creates node from action stack items.
InclNode::InclNode(std::stack<IBuildItem*>& actStack) {
	// Init members.
	m_reqFname = "";
	m_type     = PARSE_ACT_INCL;
	m_file     = "";
	m_line     = 0;

	// Claim as many items as possible (ie broken from within).
	bool doBreak = false;
	while (actStack.size() > 0) {
		// Get next item.
		IBuildItem* item = actStack.top();
		IF_NULL(item, "InclNode() null item");

		// Claim item if supposed to be part of node.
		switch (item->m_type) {
			case LEX_TKN_FILE:
				// Include file- save filename (w/o formatting).
				m_reqFname = ((LexToken*)(item))->m_rawData;
				StrUtil_trim(m_reqFname);
				StrUtil_rmAll(m_reqFname, FORMAT_CHAR);
				break;
			case LEX_TKN_INCLUDE:
				// Keyword- use to define position/signal break out.
				m_file = item->m_file;
				m_line = item->m_line;
				doBreak = true;
				break;
			default:
				// Shouldn't be here- bug!
				Terminate::inst().assert("InclNode() bad item");
				break;
		}

		// Consume any parsed tokens.
		actStack.pop();
		delete item;

		// Break out as directed.
		if (doBreak) {break;}
	}

	// Possible "file" was empty literal- no checks to do.
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
InclNode::~InclNode(void) {/* no actions */}
