/*
 * FileNode.cpp: Node representing an entire file of source code.
 */

#include "Common/Device/Terminate.h"
#include "Domain/ParseState_e.h"
#include "Ds/FileNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
FileNode::FileNode(std::stack<IBuildItem*>& actStack) {
	// Init members.
	m_decls           = {};
	m_type            = PARSE_ACT_FILE;
	m_file            = "";
	m_line            = 0;

	// Attempt to claim all items (should be able to).
	while (actStack.size() > 0) {
		// Get next item.
		IBuildItem* item = actStack.top();
		IF_NULL(item, "FileNode() null item");

		// Claim item if supposed to be part of node.
		switch (item->m_type) {
			case PARSE_ACT_VDEC:
				// Decl.
				m_decls.push_front((IAstNode*)(item));
				break;
			case LEX_TKN_EOF:
				// Throw away.
				delete item;
				break;
			default:
				// Shouldn't be here- bug!
				Terminate::inst().assert("FileNode() bad item");
				break;
		}

		// Consume any parsed tokens.
		actStack.pop();
	}
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
FileNode::~FileNode(void) {
	// Delete held tokens.
	for (IAstNode* decl : m_decls) {delete decl;}
}
