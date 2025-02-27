/*
 * SubStepParseTkns.cpp: Substep to parsed tokens (of still opened file).
 */

#include "Common/Device/File.h"
#include "Common/Device/Parser.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/InfoUtil.h"
#include "Common/Util/ParserDefs.h"
#include "Domain/LexState_e.h"
#include "Domain/ParseState_e.h"
#include "Domain/RetCode_e.h"
#include "Ds/CondNode.h"
#include "Ds/DefNode.h"
#include "Ds/FileNode.h"
#include "Ds/InclNode.h"
#include "Ds/TextNode.h"

#include "State/SubStepParseTkns.h"

using namespace std;

//==============================================================================

// Lists of states non-terminal parse states can be broken into.
LIST(SEQ_FILE_END) = {PARSE_ACT_FILE};
LIST(SEQ_FILE_TXT) = {PARSE_FILE, PARSE_TEXT};
LIST(SEQ_FILE_INC) = {PARSE_FILE, PARSE_ACT_INCL, LEX_TKN_FILE};
LIST(SEQ_FILE_DEF) = {PARSE_FILE, PARSE_ACT_DEF, LEX_TKN_DEF};
LIST(SEQ_FILE_CND) = {PARSE_FILE, PARSE_ACT_COND, PARSE_COND, LEX_TKN_DEF};

LIST(SEQ_TEXT_LST) = {PARSE_TEXT};
LIST(SEQ_TEXT_END) = {PARSE_ACT_TEXT};

LIST(SEQ_COND_END) = {};
LIST(SEQ_COND_TXT) = {PARSE_COND, PARSE_TEXT};
LIST(SEQ_COND_INC) = {PARSE_COND, PARSE_ACT_INCL, LEX_TKN_FILE};
LIST(SEQ_COND_DEF) = {PARSE_COND, PARSE_ACT_DEF, LEX_TKN_DEF};
LIST(SEQ_COND_CND) = {PARSE_COND, PARSE_ACT_COND, PARSE_COND, LEX_TKN_DEF};

//==============================================================================
// Helper callback function to determine parse state breakdown.
static ParseOp_e SubStepParseTkns_parse(std::vector<int> const** states,
		                                  int                    state,
										  int                    tkn) {
	// Let the games begin!
	switch(state) {
		case PARSE_FILE:
			WITH(tkn, LEX_TKN_EOF,     states, SEQ_FILE_END);
			WITH(tkn, LEX_TKN_TEXT,    states, SEQ_FILE_TXT);
			WITH(tkn, LEX_TKN_FILE,    states, SEQ_FILE_TXT);
			WITH(tkn, LEX_TKN_DEF,     states, SEQ_FILE_TXT);
			WITH(tkn, LEX_TKN_INCLUDE, states, SEQ_FILE_INC);
			WITH(tkn, LEX_TKN_DEFINE,  states, SEQ_FILE_DEF);
			WITH(tkn, LEX_TKN_IFDEF,   states, SEQ_FILE_CND);
			WITH(tkn, LEX_TKN_IFNDEF,  states, SEQ_FILE_CND);
			break;
		case PARSE_TEXT:
			WITH(tkn, LEX_TKN_TEXT,    states, SEQ_TEXT_LST);
			WITH(tkn, LEX_TKN_FILE,    states, SEQ_TEXT_LST);
			WITH(tkn, LEX_TKN_DEF,     states, SEQ_TEXT_LST);
			EONL(                      states, SEQ_TEXT_END);
			break;
		case PARSE_COND:
			WITH(tkn, LEX_TKN_ENDIF,   states, SEQ_COND_END);
			WITH(tkn, LEX_TKN_TEXT,    states, SEQ_COND_TXT);
			WITH(tkn, LEX_TKN_FILE,    states, SEQ_COND_TXT);
			WITH(tkn, LEX_TKN_DEF,     states, SEQ_COND_TXT);
			WITH(tkn, LEX_TKN_INCLUDE, states, SEQ_COND_INC);
			WITH(tkn, LEX_TKN_DEFINE,  states, SEQ_COND_DEF);
			WITH(tkn, LEX_TKN_IFDEF,   states, SEQ_COND_CND);
			WITH(tkn, LEX_TKN_IFNDEF,  states, SEQ_COND_CND);
			break;
		default:
			// Top can't be broken down? compiler bug.
			Terminate::inst().assert("parseState() bad state");
			break;
	}

	// "All states return... wait, why are you here?" "compiler warning."
	return PARSE_OP_ERROR;
}

//==============================================================================
// Helper callback function to identify lex tokens.
static bool SubStepParseTkns_isTkn(int state) {
	// All "tokens" are under set limit.
	return (state <= LEX_TKN_MAX);
}

//==============================================================================
// Helper callback functions to identify actions (ie call to create node).
static bool SubStepParseTkns_isAct(int state) {
	// All "actions" within set limits.
	return ((PARSE_ACT_MIN <= state) && (state <= PARSE_ACT_MAX));
}

//==============================================================================
// Helper function to create the specific node found by parser.
static IAstNode* SubStepPareTkns_newNode(Parser& parser) {
	// Create new node.
	switch (parser.m_actCode) {
		case PARSE_ACT_COND: return new CondNode(parser.m_actStack); break;
		case PARSE_ACT_DEF:  return new DefNode(parser.m_actStack);  break;
		case PARSE_ACT_FILE: return new FileNode(parser.m_actStack); break;
		case PARSE_ACT_INCL: return new InclNode(parser.m_actStack); break;
		case PARSE_ACT_TEXT: return new TextNode(parser.m_actStack); break;
		default:
			// No node? fall into assert below.
			break;
	}

	// Didn't return a node? bug!
	Terminate::inst().assert("newNode() bad code");
	return nullptr;
}

//==============================================================================
// Parses tokens into AST. Does not terminate on error.
IAstNode* SubStepParseTkns_execute(DataModel_t&                 model,
		                           std::queue<LexToken*> const& tkns) {
	// Ease of access.
	string fname = File::inst().getName();

	// (Note entrance into sub-step.)
	Print::inst().log(LOG_DEBUG, fname, "Beginning parsing...");

	// Prep for parsing.
	ParserInit_t initData = {
		.m_tkns            = tkns,
		.m_parse           = SubStepParseTkns_parse,
		.m_isTkn           = SubStepParseTkns_isTkn,
		.m_isAct           = SubStepParseTkns_isAct,
		.m_parseStart      = PARSE_FILE,
		.m_parseMismatch   = PARSE_ERR_MISMATCH,
		.m_parseUnexpected = PARSE_ERR_UNEXPECTED
	};
	Parser parser(initData);

	// Create nodes/AST from tokens.
	uint32_t nodeCnt = 0; // (debug bookkeeping)
	do {
		// Parse node.
		parser.parse();

		// Create node.
		if (SubStepParseTkns_isAct(parser.m_actCode)) {
			// Convert stack elements into new node.
			IAstNode* newNode = SubStepPareTkns_newNode(parser);
			parser.m_actStack.push(newNode);

			// Bookkeeping.
			nodeCnt++;
		}

		// Break out early for parsing errors.
		if (parser.m_actCode == PARSE_ERR_MISMATCH) {
			LexToken* tkn = (LexToken*)(parser.m_actStack.top());
			string errStr = "Mismatch token '" + tkn->m_rawData + "'";
			Print::inst().log(LOG_ERROR, fname, tkn->m_line, errStr);
			InfoUtil_recordError(model.m_summary, RET_MIS_TKN);
			break;
		}
		if (parser.m_actCode == PARSE_ERR_UNEXPECTED) {
			LexToken* tkn = (LexToken*)(parser.m_actStack.top());
			string errStr = "Unexpected token '" + tkn->m_rawData + "'";
			Print::inst().log(LOG_ERROR, fname, tkn->m_line, errStr);
			InfoUtil_recordError(model.m_summary, RET_UEX_TKN);
			break;
		}
	} while (parser.m_actCode != PARSE_ACT_FILE);

	// Confirm success of parsing.
	if (parser.m_actStack.size() != 1) {
		Terminate::inst().assert("parse() bad act stack size");
	}
	IF_NULL(parser.m_actStack.top(), "parse() bad act stack top");

	// (Final debug report on lexing.)
	string dbgStr = to_string(nodeCnt) + " nodes created";
	Print::inst().log(LOG_DEBUG, fname, dbgStr);

	// Return ptr to AST (ie top node).
	return (IAstNode*)(parser.m_actStack.top());
}
