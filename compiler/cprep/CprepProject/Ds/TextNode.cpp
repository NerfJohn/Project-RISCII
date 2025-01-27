/*
 * TextNode.cpp: Node representing raw text to be copied into result file.
 */

#include "Device/Print.h"
#include "Device/Terminate.h"

#include "Ds/TextNode.h"

using namespace std;

//==============================================================================
// TODO
TextNode::TextNode(std::stack<ABuildItem*>& itemStack) {
	// (Init members.)
	m_reqTxt = {};
	m_file   = "---";
	m_line   = 0;

	// Parse/pop until unable (or broken from within).
	bool isDone = false;
	while(itemStack.size() > 0) {
		// Get the next token.
		ABuildItem* item = itemStack.top();
		IF_NULL(item, "TextNode() null stack item");

		// Save item to appropriate "slot" (trusting parser here for contents).
		switch ((int)(item->getType())) { // 'int' to validly see extensions
			case TOKEN_LINE_TEXT:
			case TOKEN_LINE_FILE:
				// non-preprocess text- save verbatim.
				m_reqTxt.push_front(dynamic_cast<ItemToken*>(item));
				break;
			default:
				// not raw text- finished capturing.
				isDone = true;
				break;
		}

		// Inclusive continue / Exclusive breakout.
		if (isDone) {break;}
		else        {itemStack.pop();}
	}

	// Ensure required items are present.
	for (ItemToken* tkn : m_reqTxt) {IF_NULL(tkn, "TextNode() null token");}

	// Init origin info.
	if (m_reqTxt.size() > 0) {
		m_file = m_reqTxt.front()->m_file;
		m_line = m_reqTxt.front()->m_line;
	}

	// (Log creation.)
	Print::inst().log(LOG_DEBUG, m_file, m_line, "TextNode created");
}

//==============================================================================
// TODO
BuildType_e TextNode::getType(void) {return BUILD_TEXT;} // "TextNode" item

//==============================================================================
// General destructor- public to allow for generic node deletion.
TextNode::~TextNode(void) {
	// Delete underlying items.
	while(m_reqTxt.size() > 0) {
		delete m_reqTxt.front();
		m_reqTxt.pop_front();
	}
}
