/*
 * StepParseFile.cpp: Step to parse tokens into AST (saved in model).
 */

#include "Common/Device/Parser.h"
#include "Common/Device/Print.h"
#include "Common/Util/Msg.h"
#include "Common/Util/ParserDefs.h"
#include "Domain/ParseState_e.h"
#include "Ds/FileNode.h"
#include "Ds/FuncNode.h"
#include "Ds/VarNode.h"
#include "Util/AppUtil.h"

#include "State/StepParseFile.h"

using namespace std;

//==============================================================================

// Lists of states non-terminal parse states can be broken down into.
LIST(SEQ_EP_EXCL)   = {};

LIST(SEQ_FILE_ITEM) = {PARSE_FILE, PARSE_PRED_BEGIN,
		               LEX_TKN_ID, PARSE_TYPE_START};
LIST(SEQ_FILE_END)  = {PARSE_ACT_FILE, LEX_TKN_EOF};

LIST(SEQ_TYPE_PTR)  = {PARSE_TYPE_PTR, PARSER_THIS};

LIST(SEQ_PRED_VDEC) = {PARSE_ACT_VDEC, PARSER_THIS};
LIST(SEQ_PRED_FPAR) = {PARSE_PRED_FEND,  LEX_TKN_RPAREN,
		               PARSE_PARS_BEGIN, LEX_TKN_LPAREN};
LIST(SEQ_PRED_FDEC) = {PARSE_ACT_FDEC, PARSER_THIS};
LIST(SEQ_PRED_FDEF) = {PARSE_ACT_FDEC, LEX_TKN_RCURLY, LEX_TKN_LCURLY};

LIST(SEQ_PARS_NEW)  = {PARSE_PARS_CONT, PARSE_ACT_VDEC,
		               LEX_TKN_ID,      PARSE_TYPE_START};
LIST(SEQ_PARS_NEXT) = {PARSE_PARS_CONT,  PARSE_ACT_VDEC, LEX_TKN_ID,
		               PARSE_TYPE_START, LEX_TKN_COMMA};

//==============================================================================
// User-specific functions the Parser relies on for parsing.
RetErr_e parseState(int state, int tkn, std::vector<int> const*& seq) {
	// Let the games begin!
	switch (state) {
		case PARSE_FILE:
			TKN_IS  (LEX_TKN_EOF,       SEQ_FILE_END);   // end of parsing
			TKN_IS  (LEX_TKN_INT,       SEQ_FILE_ITEM);
			TKN_IS  (LEX_TKN_VOID,      SEQ_FILE_ITEM);
			break;
		case PARSE_TYPE_START:
			TKN_IS  (LEX_TKN_INT,       SEQ_TYPE_PTR);
			TKN_IS  (LEX_TKN_VOID,      SEQ_TYPE_PTR);
			break;
		case PARSE_TYPE_PTR:
			TKN_IS  (LEX_TKN_STAR,      SEQ_TYPE_PTR);   // greedy grab ptrs
			TKN_ELSE(                   SEQ_EP_EXCL);
			break;
		case PARSE_PRED_BEGIN:
			TKN_IS  (LEX_TKN_SEMICOLON, SEQ_PRED_VDEC);  // var decl
			TKN_IS  (LEX_TKN_LPAREN,    SEQ_PRED_FPAR);  // func decl/def
	        break;
		case PARSE_PRED_FEND:
			TKN_IS  (LEX_TKN_SEMICOLON, SEQ_PRED_FDEC);
			TKN_IS  (LEX_TKN_LCURLY,    SEQ_PRED_FDEF);
			break;
		case PARSE_PARS_BEGIN:
			TKN_IS  (LEX_TKN_RPAREN,    SEQ_EP_EXCL);    // empty params
			TKN_IS  (LEX_TKN_INT,       SEQ_PARS_NEW);
			TKN_IS  (LEX_TKN_VOID,      SEQ_PARS_NEW);
			break;
		case PARSE_PARS_CONT:
			TKN_IS  (LEX_TKN_RPAREN,    SEQ_EP_EXCL);
			TKN_IS  (LEX_TKN_COMMA,     SEQ_PARS_NEXT);
			break;
		default:
			// Top can't be broken down- bug!
			AppUtil_exitBug("parseState() bad state");
			break;
	}

	// No condition taken? No proper breakdown.
	return RET_ERR_ERROR;
}

//==============================================================================
// User-specific functions the Parser relies on for parsing.
bool isAct(int state) {
	// Bounds check against "ACT" group.
	return ((PARSE_ACT_MIN <= state) && (state <= PARSE_ACT_MAX));
}

//==============================================================================
// Helper function to handle node creation/saving.
static void StepParseFile_createNode(Parser& parser) {
	// Create new node.
	IBuildItem* ptr = nullptr;
	switch(parser.m_actCode) {
		case PARSE_ACT_FILE: ptr = new FileNode(parser.m_actStack); break;
		case PARSE_ACT_VDEC: ptr = new VarNode(parser.m_actStack);  break;
		case PARSE_ACT_FDEC: ptr = new FuncNode(parser.m_actStack); break;
		default:
			// No node- bug!
			AppUtil_exitBug("createNode() no matching node");
			break;
	}
	Ptr<IBuildItem> node(ptr);
	if (node.isNull()) {AppUtil_exitBug("createNode() null node made");}

	// (Stay informative.)
	string dbg = Msg() + "created node #" + parser.m_actCode;
	Print_log(LOG_DEBUG, node->m_file, node->m_line, dbg);

	// Re-add node (to be included in larger nodes).
	parser.m_actStack.push(node);
}

//==============================================================================
// Parses model's tokens into AST, saving it to the model.
void StepParseFile_execute(DataModel_t& model) {
	// Get filename (for logging).
	if (model.m_tkns.size() == 0) {AppUtil_exitBug("parseFile() no tokens");}
	string fname = model.m_tkns.front()->m_file;

	// (Keep debug informed.)
	Print_log(LOG_DEBUG, Msg() + "Parsing '" + fname + "'");

	// Prep parser.
	Parser parser(model.m_tkns);

	// Parse file into nodes/AST.
	uint32_t numNodes = 0; // (track for debugging.)
	do {
		// Parse into node.
		parser.parse();

		// Create node.
		if (isAct(parser.m_actCode) == true) {
			// Create.
			StepParseFile_createNode(parser);

			// Bookkeeping.
			numNodes++;
		}

		// Break out early for errors.
		Ptr<LexToken> tkn = parser.m_actStack.top().toType<LexToken>();
		if (parser.m_actCode == PARSE_ERR_MISMATCH) {
			string err = Msg() + "Mismatch token '" + tkn->m_rawData + "'";
			Print_log(LOG_ERROR, fname, tkn->m_line, err);
			AppUtil_error(model, RET_MIS_TKN);
			break;
		}
		if (parser.m_actCode == PARSE_ERR_UNEXPECTED) {
			string err = Msg() + "Unexpected token '" + tkn->m_rawData + "'";
			Print_log(LOG_ERROR, fname, tkn->m_line, err);
			AppUtil_error(model, RET_UEX_TKN);
			break;
		}
	} while (parser.m_actCode != PARSE_ACT_FILE);

	// Confirm results of parsing.
	if (APP_OK == true) {
		// Sanity Check.
		if (parser.m_actStack.size() != 1) {
			AppUtil_exitBug("parseFile() bad act stack size");
		}
		if (parser.m_actStack.top().isNull()) {
			AppUtil_exitBug("parseFile() bad act stack top");
		}

		// Save.
		model.m_ast = parser.m_actStack.top().toType<IAstNode>();
	}

	// (Final debug report on lexing.)
	string dbgStr = Msg() + numNodes + " nodes created";
	Print_log(LOG_DEBUG, fname, dbgStr);
}
