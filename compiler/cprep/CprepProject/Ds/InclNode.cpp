/*
 * InclNode.cpp: Node representing file inclusion.
 */

#include "Common/Device/File.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Ds/LexToken.h"
#include "Common/Util/InfoUtil.h"
#include "Common/Util/StrUtil.h"
#include "Domain/ParseState_e.h"
#include "Domain/RetCode_e.h"

#include "Ds/InclNode.h"

using namespace std;

//==============================================================================

// Definitions for creating node.
#define FORMAT_CHAR '"'

//==============================================================================
// Helper function to check if file exists.
bool InclNode::fileExists(std::string const& fname) {
	// Check existence by attempting to open (read to no overwrite data).
	RetErr_e retErr = File::inst().open(fname, FILE_OP_READ);
	File::inst().close();

	// Return based on result of opening.
	return (retErr == RET_ERR_NONE);
}

//==============================================================================
// Constructor- creates node from action stack items.
InclNode::InclNode(std::stack<IBuildItem*>& actStack) {
	// Init members.
	m_reqFname = "";
	m_finFname = "";
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
// Locate included files, adding them to the model for processing.
void InclNode::findIncludes(DataModel_t& model) {
	// 1st: Attempt to find file within origin's original directory.
	string incPath    = m_file;
	StrUtil_asDir(incPath);
	incPath          += m_reqFname;
	bool isValidPath  = this->fileExists(incPath);

	// 2nd: Attempt to find file within included directories.
	if (isValidPath == false) {
		// Check each directory (in order of record).
		for (string dir : model.m_iDirs) {
			// Determine path.
			incPath      = dir + m_reqFname;
			isValidPath  = this->fileExists(incPath);

			// Stop search if found.
			if (isValidPath) {break;}
		}
	}

	// Found? Schedule for parsing if it hasn't been already- otherwise, error.
	if (isValidPath == false) {
		string errStr = string("No such file '") + m_reqFname + "'";
		Print::inst().log(LOG_ERROR, m_file, m_line, errStr);
		InfoUtil_recordError(model.m_summary, RET_BAD_INC);
	}
	else if (model.m_iAsts.get(incPath) == nullptr) {
		string dbgStr = string("Including '") + incPath + "'";
		Print::inst().log(LOG_DEBUG, m_file, m_line, dbgStr);
		model.m_incPaths.push(incPath);
	}

	// In any case- save the final path.
	m_finFname = incPath;
}

//==============================================================================
// TODO
void InclNode::checkDefines(DataModel_t& model) {
	// Ensure nesting limit is met.
	if (model.m_nestsLeft > 0) {
		// Expand search to included AST.
		model.m_nestsLeft--;
		IAstNode* ast = (IAstNode*)(model.m_iAsts.get(m_finFname));
		IF_NULL(ast, "checkDefines() null ast");
		ast->checkDefines(model);
		model.m_nestsLeft++;
	}
	else {
		// Nesting has gone too far.
		Print::inst().log(LOG_ERROR, m_file, m_line, "Nesting limit reached");
		InfoUtil_recordError(model.m_summary, RET_NO_NEST);
	}
}

//==============================================================================
// TODO
void InclNode::writeText(DataModel_t& model) {
	// Expand search to included AST.
	IAstNode* ast = (IAstNode*)(model.m_iAsts.get(m_finFname));
	IF_NULL(ast, "writeText() null ast");
	ast->writeText(model);
}

//==============================================================================
// Std destructor- deletes underlying nodes/tokens.
InclNode::~InclNode(void) {/* no actions */}
