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

#include "State/SubStepParseTkns.h"

using namespace std;

//==============================================================================

// Lists of states non-terminal parse states can be broken into.
LIST(SEQ_FILE)     = {PARSE_ACT_FILE};

LIST(SEQ_LST_TEXT) = {PARSE_TEXT};
LIST(SEQ_END_TEXT) = {PARSE_FILE, PARSE_ACT_TEXT};

//==============================================================================
// Helper callback function to determine parse state breakdown.
static ParseOp_e SubStepParseTkns_parse(std::vector<int> const** states,
		                                  int                    state,
										  int                    tkn) {
	// Let the games begin!
	switch(state) {
		case PARSE_FILE:
			WITH(tkn, LEX_TKN_EOF,  states, SEQ_FILE);
			WITH(tkn, LEX_TKN_TEXT, states, SEQ_LST_TEXT);
			WITH(tkn, LEX_TKN_FILE, states, SEQ_LST_TEXT);
			WITH(tkn, LEX_TKN_DEF,  states, SEQ_LST_TEXT);
			break;
		case PARSE_TEXT:
			WITH(tkn, LEX_TKN_TEXT, states, SEQ_LST_TEXT);
			WITH(tkn, LEX_TKN_FILE, states, SEQ_LST_TEXT);
			WITH(tkn, LEX_TKN_DEF,  states, SEQ_LST_TEXT);
			EONL(                   states, SEQ_END_TEXT);
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
// Parses tokens into AST (within model). Does not termiante on error.
void SubStepParseTkns_execute(DataModel_t&                 model,
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
			// TODO
			Print::inst().cli(to_string(parser.m_actCode));

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

	// TODO check/grab sole file node, add to model.

	// (Final debug report on lexing.)
	string dbgStr = to_string(nodeCnt) + " nodes created";
	Print::inst().log(LOG_DEBUG, fname, dbgStr);
}
