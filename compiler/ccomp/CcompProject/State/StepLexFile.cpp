/*
 * StepLexFile.cpp: Step to lex file into series of tokens (into model).
 */

#include "Common/Device/Lexer.h"
#include "Common/Device/Print.h"
#include "Common/Util/LexerDefs.h"
#include "Common/Util/Msg.h"
#include "Domain/LexState_e.h"
#include "Domain/RetCode_e.h"
#include "Util/AppUtil.h"

#include "State/StepLexFile.h"

using namespace std;

//==============================================================================
// User-specific function Lexer relies on for lexing.
int nextState(int state, std::string const& buffer, uint8_t chr) {
	// Flags to request additional processing.
	bool isKword = false;
	bool isOp    = false;

	// "And so it begins." ~ King Theoden.
	switch (state) {
		case LEX_START:                          // start of all lex paths
			EOFILE(          LEX_TKN_EOF);       // end of all files
			WS    (          LEX_START);         // ignore whitespace
			IS    ('*',      LEX_LOOP_OP);
			IS    ('(',      LEX_LOOP_OP);
			IS    (')',      LEX_LOOP_OP);
			IS    ('{',      LEX_LOOP_OP);
			IS    ('}',      LEX_LOOP_OP);
			// TODO- is # -> literals paths
			LABEL (          LEX_LOOP_KWORD);    // checked AFTER # checks
			break;
		case LEX_LOOP_KWORD:
			LABEL (          LEX_LOOP_KWORD);
			isKword = true;                      // else... process keyword
			break;
		case LEX_LOOP_OP:
			// TODO- is op predicate -> loop
			isOp = true;                         // else... process operator
			break;
		default:
			// Start at non-state? Source code bug!
			AppUtil_exitBug("nextState() lexing with non-state");
			break; // Does NOT return
	}

	// Identify specific keywords/operators.
	if (isKword == true) {
		// First check against keywords.
		ISWORD("int", LEX_TKN_INT);

		// No matches? Must be a variable then.
		return LEX_TKN_ID;
	}
	else if (isOp == true) {
		// First check against operators.
		ISWORD("*", LEX_TKN_STAR);
		ISWORD("(", LEX_TKN_LPAREN);
		ISWORD(")", LEX_TKN_RPAREN);
		ISWORD("{", LEX_TKN_LCURLY);
		ISWORD("}", LEX_TKN_RCURLY);

		// No matches? Unknown operator.
		return LEXER_ERROR;
	}

	// Reached end? State/chr pair must have no matches.
	return LEXER_ERROR;
}

//==============================================================================
// User-specific function Lexer relies on for lexing.
bool isTkn(int state) {
	// Bounds check against "TKN" group.
	return ((LEX_TKN_MIN <= state) && (state <= LEX_TKN_MAX));
}

//==============================================================================
// Lexes given file into tokens- saving the list to the data model.
void StepLexFile_execute(DataModel_t& model, std::string const& fname) {
	// (Keep debug informed.)
	Print_log(LOG_DEBUG, Msg() + "Lexing '" + fname + "'");

	// Prep lexer.
	Lexer tkns(fname);
	if (tkns.fileOpened() == false) {
		Print_log(LOG_ERROR, Msg() + "Unable to read '" + fname + "'");
		AppUtil_error(model, RET_NO_READ);
	}

	// Lex file into sequence of tokens.
	if (tkns.fileOpened() == true) {
		do {
			// Lex token.
			tkns.lex();

			// Create/save tokens (nix comments- no programmable value).
			if ((isTkn(tkns.m_tknCode)) && (tkns.m_tknCode != LEX_TKN_CMMNT)) {
				// (Keep debug informed.)
				string dbg = Msg() + "Found token '" + tkns.m_value + "'";
				Print_log(LOG_DEBUG, fname, tkns.m_line, dbg);

				// Save token.
				Ptr<LexToken> newTkn(new LexToken(tkns.m_value,
						                          tkns.m_tknCode,
											      fname,
											      tkns.m_line));
				if (newTkn.isNull()) {AppUtil_exitBug("LexToken() failed");}
				model.m_tkns.push(newTkn);
			}

			// Catch/break-out for errors.
			if (tkns.m_tknCode == LEXER_ERROR) {
				string err = Msg() + "Unknown sequence '" + tkns.m_value + "'";
				Print_log(LOG_ERROR, fname, tkns.m_line, err);
				AppUtil_error(model, RET_BAD_LEX);
				break;
			}
		} while (tkns.m_tknCode != LEX_TKN_EOF);
	}

	// (Keep debug informed.)
	Print_log(LOG_DEBUG, fname, Msg() + model.m_tkns.size() + " tokens found");
}
