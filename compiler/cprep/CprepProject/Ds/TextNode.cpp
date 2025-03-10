/*
 * TextNode.cpp: Node representing non-pre-processor related source text.
 */

#include "Common/Device/File.h"
#include "Common/Device/Terminate.h"
#include "Common/Ds/LexToken.h"
#include "Domain/ParseState_e.h"

#include "Ds/TextNode.h"

using namespace std;

//==============================================================================
// Helper function to detect conditional keyword just under top of stack.
static bool nextItemCond(stack<IBuildItem*>& actStack) {
	// Result of operation.
	bool retBool = false;

	// Only attempt if "next" exists.
	if (actStack.size() > 2) {
		// Get "next" item.
		IBuildItem* tmp  = actStack.top();
		actStack.pop();
		IBuildItem* next = actStack.top();
		actStack.push(tmp);

		// Determine proper result.
		IF_NULL(next, "nextItemCond() null item");
		retBool = ((next->m_type == LEX_TKN_IFDEF) ||
				   (next->m_type == LEX_TKN_IFNDEF));
	}

	// Return result.
	return retBool;
}

//==============================================================================
// Constructor- creates node from action stack items.
TextNode::TextNode(std::stack<IBuildItem*>& actStack) {
	// Init members.
	m_reqText  = "";
	m_type     = PARSE_ACT_TEXT;
	m_file     = "";
	m_line     = 0;

	// Claim as many items as possible (ie broken from within).
	bool doBreak = false;
	while (actStack.size() > 0) {
		// Get next item.
		IBuildItem* item = actStack.top();
		IF_NULL(item, "TextNode() null item");

		// Claim item if supposed to be part of node.
		switch (item->m_type) {
			case LEX_TKN_TEXT:
			case LEX_TKN_FILE:
				// Source text- append into single "chunk" (backwards- stack).
				m_reqText = ((LexToken*)(item))->m_rawData + m_reqText;
				m_file = item->m_file;
				m_line = item->m_line;
				break;
			case LEX_TKN_DEF:
				// Definition- only add if not part of a conditional.
				if (nextItemCond(actStack)) {doBreak = true;}
				else {
					m_reqText = ((LexToken*)(item))->m_rawData + m_reqText;
					m_file = item->m_file;
					m_line = item->m_line;
				}
				break;
			default:
				// Not source text- break out!
				doBreak = true;
				break;
		}

		// Bookkeeping.
		if (doBreak) {break;}                       // stop claiming...
		else         {actStack.pop(); delete item;} // ...or continue
	}

	// Ensure node contains at least SOME text.
	if (m_reqText.size() == 0) {Terminate::inst().assert("TextNode() empty");}
}

//==============================================================================
// TODO
void TextNode::writeText(DataModel_t& model) {
	// Write source text to new file.
	File::inst().write(m_reqText);
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
TextNode::~TextNode(void) {/* no actions */}
