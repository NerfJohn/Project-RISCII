/*
 * FileNode.cpp: Node representing one pre-processed/parsed file.
 */

#include "Common/Device/Terminate.h"
#include "Domain/ParseState_e.h"

#include "Ds/FileNode.h"

using namespace std;

//==============================================================================
// Constructor- creates node from action stack items.
FileNode::FileNode(std::stack<IBuildItem*>& actStack) {
	// Init members.
	m_reqNodes = {};
	m_type     = PARSE_ACT_FILE;
	m_file     = "";
	m_line     = 1;              // files tend to begin with line 1...

	// "Final" node- parse everything off stack.
	while (actStack.size() > 0) {
		// Get next item.
		IBuildItem* item = actStack.top();
		IF_NULL(item, "FileNode() null item");

		// Claim item if supposed to be part of node.
		switch (item->m_type) {
			case PARSE_ACT_TEXT:
			case PARSE_ACT_INCL:
			case PARSE_ACT_DEF:
			case PARSE_ACT_COND:
				// Child node- add as such.
				m_reqNodes.push_front((IAstNode*)(item));
				break;
			case LEX_TKN_EOF:
				// Guaranteed token- use as basis for filename.
				m_file = item->m_file;
				break;
			default:
				// Not supposed to be here- bug!
				Terminate::inst().assert("FileNode() bad item type");
				break;
		}

		// FileNode should encompass all leftover items.
		actStack.pop();
	}

	// (Loop above confirmed m_reqNodes doesn't have null ptrs.)
}

//==============================================================================
// Locate included files, adding them to the model for processing.
void FileNode::findIncludes(DataModel_t& model) {
	// Forward call to children (in order).
	for (IAstNode* child : m_reqNodes) {child->findIncludes(model);}
}

//==============================================================================
// TODO
void FileNode::checkDefines(DataModel_t& model) {
	// Forward call to children (in order).
	for (IAstNode* child : m_reqNodes) {child->checkDefines(model);}
}

//==============================================================================
// TODO
void FileNode::writeText(DataModel_t& model) {
	// Forward call to children (in order).
	for (IAstNode* child : m_reqNodes) {child->writeText(model);}
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
FileNode::~FileNode(void) {
	// Delete underlying nodes.
	for (IAstNode* child : m_reqNodes) {delete child;}
}
