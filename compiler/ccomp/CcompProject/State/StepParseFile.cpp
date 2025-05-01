/*
 * StepParseFile.cpp: Step to parse tokens into AST (saved in model).
 */

#include "Common/Device/Parser.h"
#include "Common/Device/Print.h"
#include "Common/Util/Msg.h"
#include "Common/Util/ParserDefs.h"
#include "Domain/ParseState_e.h"
#include "Util/AppUtil.h"

#include "State/StepParseFile.h"

using namespace std;

//==============================================================================

// Lists of states non-terminal parse states can be broken down into.

// TODO- rework when adding other decls/inits.
LIST(SEQ_FILE_TODO) = {PARSE_FILE, PARSE_ACT_FDEC, LEX_TKN_ID, PARSER_THIS};
LIST(SEQ_FILE_END)  = {PARSE_ACT_FILE, LEX_TKN_EOF};

//==============================================================================
// User-specific functions the Parser relies on for parsing.
RetErr_e parseState(int state, int tkn, std::vector<int> const*& seq) {
	// Let the games begin!
	switch (state) {
		case PARSE_FILE:
			TKN_IS(LEX_TKN_EOF,   SEQ_FILE_END);   // end of parsing
			TKN_IS(LEX_TKN_INT,   SEQ_FILE_TODO);
			TKN_IS(LEX_TKN_CHAR,  SEQ_FILE_TODO);
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
			// TODO
			Print_cli(Msg() + "NODE " + parser.m_actCode);

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
		Print_cli("TODO- save AST");
	}

	// (Final debug report on lexing.)
	string dbgStr = Msg() + numNodes + " nodes created";
	Print_log(LOG_DEBUG, fname, dbgStr);
}
