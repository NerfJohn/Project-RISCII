/*
 * FileNode.cpp: Node representing entire file/translation unit.
 */

#include "Common/Util/Msg.h"
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
			case PARSE_ACT_VDEC:
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
// Analyzes node- prepping information to be checked by check().
void FileNode::analyze(DataModel_t& model) {
	// (Stay informative.)
	Print_log(LOG_DEBUG, Msg() + "Analyzing '" + m_file + "'");

	// Analyze each element (in order).
	model.m_symCnt = 0;
	model.m_syms.scopePush();
	for (Ptr<IAstNode> elm : m_elements) {elm->analyze(model);}
	model.m_syms.scopePop();

	// (Keep staying informative.)
	Print_log(LOG_DEBUG, Msg() + model.m_symCnt + " symbols created");
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
FileNode::~FileNode(void) {/* no actions */}
