/*
 * FileNode.cpp: Node representing entire file/translation unit.
 */

#include "Domain/ParseState_e.h"
#include "Util/AppUtil.h"

#include "Ds/FileNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
FileNode::FileNode(std::stack<Ptr<IBuildItem>>& actStack) {
	// Init members.
	m_elements = {};

	m_type = PARSE_ACT_FILE;
	m_file = "";
	m_line = 1;              // files- regardless of elements, being at line 1

	// "Final" node- consumes all items.
	while (actStack.size() > 0) {
		// Get next item.
		Ptr<IBuildItem> item = actStack.top();

		// Claim (as applicable).
		switch(item->m_type) {
			case PARSE_ACT_FDEC:
				// Save (undo stack inversion).
				m_elements.push_front(item.toType<IAstNode>());
				break;
			case LEX_TKN_EOF:
				// Use as file basis.
				m_file = item->m_file;
				break;
			default:
				// Unknown type- bug!
				AppUtil_exitBug("FileNode() bad type");
				break;
		}

		// To next item.
		actStack.pop();
	}
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
FileNode::~FileNode(void) {/* no actions */}
