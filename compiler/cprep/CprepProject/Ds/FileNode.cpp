/*
 * FileNode.cpp: Node representing one file/translation unit.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"

#include "Ds/FileNode.h"

using namespace std;

//==============================================================================
// TODO
FileNode::FileNode(std::stack<ABuildItem*>& itemStack) {
	// (Init members.)
	m_reqNodes = {};
	m_file     = "---";
	m_line     = 0;

	// Parse/pop until unable (or broken from within).
	bool isDone = false;
	while(itemStack.size() > 0) {
		// Get the next token.
		ABuildItem* item = itemStack.top();
		IF_NULL(item, "FileNode() null stack item");

		// Save item to appropriate "slot" (trusting parser here for contents).
		switch ((int)(item->getType())) { // 'int' to validly see extensions
			case BUILD_TEXT:
			case BUILD_INC:
				// Contents of file/node.
				m_reqNodes.push_front(dynamic_cast<AAstNode*>(item));
				break;
			default:
				// Parser screwed up!
				Terminate_assert("FileNode() with unknown item");
				break;
		}

		// Inclusive continue / Exclusive breakout.
		if (isDone) {break;}
		else        {itemStack.pop();}
	}

	// Ensure required items are present.
	for (AAstNode* tkn : m_reqNodes) {IF_NULL(tkn, "FileNode() null child");}

	// Init origin info.
	if (m_reqNodes.size() > 0) {
		m_file = m_reqNodes.front()->m_file;
		m_line = m_reqNodes.front()->m_line;
	}

	// (Log creation.)
	Print::inst().log(LOG_DEBUG, m_file, m_line, "FileNode created");
}

//==============================================================================
// TODO
BuildType_e FileNode::getType(void) {return BUILD_FILE;} // "FileNode" item

//==============================================================================
// General destructor- public to allow for generic node deletion.
FileNode::~FileNode(void) {
	// Delete underlying items.
	while(m_reqNodes.size() > 0) {
		delete m_reqNodes.front();
		m_reqNodes.pop_front();
	}
}
