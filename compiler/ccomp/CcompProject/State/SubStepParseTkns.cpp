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
#include "Ds/FileNode.h"
#include "Ds/LitNode.h"
#include "Ds/VDeclNode.h"

#include "State/SubStepParseTkns.h"

using namespace std;

//==============================================================================

// Lists of states non-terminal parse states can be broken into.
LIST(SEQ_EPSILON)    = {};

LIST(SEQ_FILE_END)   = {PARSE_ACT_FILE};
LIST(SEQ_FILE_DECL)  = {PARSE_FILE,
		                PARSE_PRED_STRT,
						LEX_TKN_ID,
						PARSE_TYPE_STRT};

LIST(SEQ_TYPE_PRE)   = {PARSE_TYPE_PRE};
LIST(SEQ_TYPE_PST)   = {PARSE_TYPE_PST};
LIST(SEQ_TYPE_PTR)   = {PARSE_TYPE_PTR};

LIST(SEQ_LIT_HEAD)   = {PARSE_LIT_HEAD};
LIST(SEQ_LIT_NEXT)   = {PARSE_LIT_NEXT};
LIST(SEQ_LIT_LINE)   = {PARSE_LIT_NEXT, PARSE_LIT_ITEM};
LIST(SEQ_LIT_ACT)    = {PARSE_ACT_LIT};

LIST(SEQ_PRED_ARR)   = {PARSE_PRED_VDEC, LEX_TKN_RSQUARE, LEX_TKN_INT_LIT};
LIST(SEQ_PRED_VDEC)  = {PARSE_ACT_VDEC};
LIST(SEQ_PRED_VNIT)  = {PARSE_ACT_VDEC, LEX_TKN_SEMICOLON, PARSE_LIT_STRT};

//==============================================================================
// Helper callback function to determine parse state breakdown.
static ParseOp_e SubStepParseTkns_parse(std::vector<int> const** states,
		                                  int                    state,
										  int                    tkn) {
	// Let the games begin!
	switch(state) {
		case PARSE_FILE:
			WITH(tkn, LEX_TKN_EOF,       states, SEQ_FILE_END);
			ONLY(tkn, LEX_TKN_UNSIGNED,  states, SEQ_FILE_DECL);
			ONLY(tkn, LEX_TKN_VOLATILE,  states, SEQ_FILE_DECL);
			ONLY(tkn, LEX_TKN_INT,       states, SEQ_FILE_DECL);
			ONLY(tkn, LEX_TKN_CHAR,      states, SEQ_FILE_DECL);
			ONLY(tkn, LEX_TKN_VOID,      states, SEQ_FILE_DECL);
			break;
		case PARSE_TYPE_STRT:                                   // type states
			WITH(tkn, LEX_TKN_UNSIGNED,  states, SEQ_TYPE_PRE);
			WITH(tkn, LEX_TKN_VOLATILE,  states, SEQ_TYPE_PRE);
			WITH(tkn, LEX_TKN_INT,       states, SEQ_TYPE_PST);
			WITH(tkn, LEX_TKN_CHAR,      states, SEQ_TYPE_PST);
			WITH(tkn, LEX_TKN_VOID,      states, SEQ_TYPE_PTR);
			break;
		case PARSE_TYPE_PRE:
			WITH(tkn, LEX_TKN_UNSIGNED,  states, SEQ_TYPE_PRE);
			WITH(tkn, LEX_TKN_VOLATILE,  states, SEQ_TYPE_PRE);
			WITH(tkn, LEX_TKN_INT,       states, SEQ_TYPE_PST);
			WITH(tkn, LEX_TKN_CHAR,      states, SEQ_TYPE_PST);
			break;
		case PARSE_TYPE_PST:
			WITH(tkn, LEX_TKN_UNSIGNED,  states, SEQ_TYPE_PST);
			WITH(tkn, LEX_TKN_VOLATILE,  states, SEQ_TYPE_PST);
			WITH(tkn, LEX_TKN_STAR,      states, SEQ_TYPE_PTR);
			EONL(                        states, SEQ_EPSILON);
			break;
		case PARSE_TYPE_PTR:
			WITH(tkn, LEX_TKN_STAR,      states, SEQ_TYPE_PTR);
			EONL(                        states, SEQ_EPSILON);
			break;
		case PARSE_LIT_STRT:                                    // lit states
			WITH(tkn, LEX_TKN_INT_LIT,   states, SEQ_LIT_ACT);
			WITH(tkn, LEX_TKN_CHR_LIT,   states, SEQ_LIT_ACT);
			WITH(tkn, LEX_TKN_STR_LIT,   states, SEQ_LIT_ACT);
			WITH(tkn, LEX_TKN_LCURLY,    states, SEQ_LIT_HEAD);
			break;
		case PARSE_LIT_HEAD:
			WITH(tkn, LEX_TKN_INT_LIT,   states, SEQ_LIT_NEXT);
			WITH(tkn, LEX_TKN_CHR_LIT,   states, SEQ_LIT_NEXT);
			WITH(tkn, LEX_TKN_RCURLY,    states, SEQ_LIT_ACT);
			break;
		case PARSE_LIT_ITEM:
			WITH(tkn, LEX_TKN_INT_LIT,   states, SEQ_EPSILON);
			WITH(tkn, LEX_TKN_CHR_LIT,   states, SEQ_EPSILON);
			break;
		case PARSE_LIT_NEXT:
			WITH(tkn, LEX_TKN_COMMA,     states, SEQ_LIT_LINE);
			WITH(tkn, LEX_TKN_RCURLY,    states, SEQ_LIT_ACT);
			break;
		case PARSE_PRED_STRT:                                   // pred states
			WITH(tkn, LEX_TKN_LSQUARE,   states, SEQ_PRED_ARR);
			WITH(tkn, LEX_TKN_SEMICOLON, states, SEQ_PRED_VDEC);
			WITH(tkn, LEX_TKN_ASSIGN,    states, SEQ_PRED_VNIT);
			break;
		case PARSE_PRED_VDEC:
			WITH(tkn, LEX_TKN_SEMICOLON, states, SEQ_PRED_VDEC);
			WITH(tkn, LEX_TKN_ASSIGN,    states, SEQ_PRED_VNIT);
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
static IAstNode* SubStepPareTkns_newNode(Parser& parser, DataModel_t& model) {
	// Create new node.
	switch (parser.m_actCode) {
		case PARSE_ACT_FILE:
			return new FileNode(parser.m_actStack);
			break;
		case PARSE_ACT_LIT:
			return new LitNode(parser.m_actStack);
			break;
		case PARSE_ACT_VDEC:
			return new VDeclNode(parser.m_actStack, model); // address warns
			break;
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
			IAstNode* newNode = SubStepPareTkns_newNode(parser, model);
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
	if (model.m_summary.m_numErrs == 0) {
		if (parser.m_actStack.size() != 1) {
			Terminate::inst().assert("parse() bad act stack size");
		}
		IF_NULL(parser.m_actStack.top(), "parse() bad act stack top");
	}

	// (Final debug report on lexing.)
	string dbgStr = to_string(nodeCnt) + " nodes created";
	Print::inst().log(LOG_DEBUG, fname, dbgStr);

	// Return ptr to AST (ie top node).
	return (IAstNode*)(parser.m_actStack.top());
}
