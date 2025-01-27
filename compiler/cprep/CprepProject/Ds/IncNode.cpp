/*
 * IncNode.cpp: Node representing include pre-processing directive.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"

#include "Ds/IncNode.h"

using namespace std;

//==============================================================================
// TODO
IncNode::IncNode(std::stack<ABuildItem*>& itemStack) {
	// (Init members.)
	m_reqFname = nullptr;
	m_file     = "";
	m_line     = 0;

	// Parse/pop until unable (or broken from within).
	bool isDone = false;
	while(itemStack.size() > 0) {
		// Get the next token.
		ABuildItem* item = itemStack.top();
		IF_NULL(item, "IncNode() with null item");

		// Save item to appropriate "slot" (trusting parser here for contents).
		switch ((int)(item->getType())) { // 'int' to validly see extensions
			case TOKEN_LINE_FILE:
				// Save record of included file for later.
				m_reqFname = dynamic_cast<ItemToken*>(item);
				break;
			case TOKEN_KW_INCLUDE:
				// Keyword ends sequence- unused, but origin is useful.
				m_file = item->m_file;
				m_line = item->m_line;
				isDone = true;
				delete   item;
				break;
			default:
				// Parser screwed up!
				Terminate_assert("TextNode() with unknown item");
				break;
		}

		// Item consumed from stack.
		itemStack.pop();

		// Break-out once sequence condition is met.
		if (isDone) {break;}
	}

	// Ensure required items are present.
	IF_NULL(m_reqFname, "IncNode() empty filename");

	// (Log creation.)
	Print::inst().log(LOG_DEBUG, m_file, m_line, "IncNode created");
}

//==============================================================================
// TODO
BuildType_e IncNode::getType(void) {return BUILD_INC;} // "IncNode" item

//==============================================================================
// General destructor- public to allow for generic node deletion.
IncNode::~IncNode(void) {
	// Delete underlying items.
	delete m_reqFname;
}
