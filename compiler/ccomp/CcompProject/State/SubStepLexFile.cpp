/*
 * SubStepLexFile.cpp: Substep to lex opened file into list of tokens.
 */

#include "Common/Device/File.h"
#include "Common/Device/Lexer.h"
#include "Common/Device/Print.h"
#include "Common/Device/Terminate.h"
#include "Common/Util/InfoUtil.h"
#include "Common/Util/LexerDefs.h"
#include "Domain/LexState_e.h"
#include "Domain/RetCode_e.h"

#include "State/SubStepLexFile.h"

using namespace std;

//==============================================================================
// Helper callback function to determine lex state transitions.
static int SubStepLexFile_nextState(int     const  state,
		                            string  const& buffer,
									uint8_t const  byte) {
	// Flag indicating string processing is needed.
	bool doKword = false;

	// "And so it begins" - King Theoden.
	switch (state) {
		case LEX_START:                              // (Start of all lex paths)
			EOFILE(byte,         LEX_TKN_EOF);       // end of all files
			WS    (byte,         LEX_START);         // ignore whitespace
			IS    (byte,'\'',    LEX_CHR_CHR);       // start of char literal
			IS    (byte,';',     LEX_IS_SEMICOLON);
			IS    (byte,'=',     LEX_IS_ASSIGN);
			IS    (byte,'-',     LEX_IS_MINUS);
			IS    (byte,',',     LEX_IS_COMMA);
			IS    (byte,'*',     LEX_IS_STAR);
			IS    (byte,'[',     LEX_IS_LSQUARE);
			IS    (byte,']',     LEX_IS_RSQUARE);
			IS    (byte,'{',     LEX_IS_LCURLY);
			IS    (byte,'}',     LEX_IS_RCURLY);
			IS    (byte,'0',     LEX_HEX_ZERO);      // zero may be hex...
			IN    (byte,'1','9', LEX_LOOP_INT);      // ...nums are decimal...
			IS    (byte,'"',     LEX_STR_STR);       // start of a str literal
			LABEL (byte,         LEX_LOOP_KWORD);    // ...else, keyword
			break;
		case LEX_CHR_CHR:
			IS    (byte,'\\',    LEX_CHR_ESC);       // escape- extra char
			IS    (byte,'\n',    LEX_ERROR);         // char must be one line
			EOFILE(byte,         LEX_ERROR);         // unfinished char
			ELSE  (              LEX_CHR_END);
			break;
		case LEX_CHR_ESC:
			IS    (byte,'\\',    LEX_CHR_END);
			IS    (byte,'\'',    LEX_CHR_END);
			IS    (byte,'0',     LEX_CHR_END);
			IS    (byte,'n',     LEX_CHR_END);
			IS    (byte,'r',     LEX_CHR_END);
			IS    (byte,'t',     LEX_CHR_END);
			break;
		case LEX_CHR_END:
			IS    (byte,'\'',    LEX_IS_CHR_LIT);    // must close char literal
			break;
		case LEX_HEX_ZERO:
			IS    (byte,'x',     LEX_HEX_X);
			IN    (byte,'0','9', LEX_LOOP_INT);
			ELSE  (              LEX_TKN_INT_LIT);
			break;
		case LEX_HEX_X:
			HEX   (byte,         LEX_HEX_HEX);
			break;
		case LEX_HEX_HEX:
			HEX   (byte,         LEX_HEX_HEX);
			ELSE  (              LEX_TKN_INT_LIT);
			break;
		case LEX_STR_STR:
			IS    (byte,'"',     LEX_STR_END);
			IS    (byte,'\\',    LEX_STR_ESC);
			IS    (byte,'\n',    LEX_ERROR);         // string must be one line
			EOFILE(byte,         LEX_ERROR);         // unfinished string
			ELSE  (              LEX_STR_STR);
			break;
		case LEX_STR_ESC:
			IS    (byte,'"',     LEX_STR_STR);
			IS    (byte,'\\',    LEX_STR_STR);
			IS    (byte,'0',     LEX_STR_STR);
			IS    (byte,'n',     LEX_STR_STR);
			IS    (byte,'r',     LEX_STR_STR);
			IS    (byte,'t',     LEX_STR_STR);
			break;
		case LEX_STR_END:
			ELSE  (              LEX_TKN_STR_LIT);
			break;
		case LEX_LOOP_KWORD:
			LABEL (byte,         LEX_LOOP_KWORD);    // greedy grab chars
			doKword = true;
			break;
		case LEX_LOOP_INT:
			IN    (byte,'0','9', LEX_LOOP_INT);      // greedy grab chars
			ELSE  (              LEX_TKN_INT_LIT);
			break;
		case LEX_IS_SEMICOLON:
			ELSE  (              LEX_TKN_SEMICOLON);
			break;
		case LEX_IS_ASSIGN:
			ELSE  (              LEX_TKN_ASSIGN);
			break;
		case LEX_IS_MINUS:
			IS    (byte,'0',     LEX_HEX_ZERO);      // "negative hex"?
			IN    (byte,'1','9', LEX_LOOP_INT);      // "negative decimal"
			ELSE  (              LEX_TKN_MINUS);
			break;
		case LEX_IS_COMMA:
			ELSE  (              LEX_TKN_COMMA);
			break;
		case LEX_IS_STAR:
			ELSE  (              LEX_TKN_STAR);
			break;
		case LEX_IS_LSQUARE:
			ELSE  (              LEX_TKN_LSQUARE);
			break;
		case LEX_IS_RSQUARE:
			ELSE  (              LEX_TKN_RSQUARE);
			break;
		case LEX_IS_LCURLY:
			ELSE  (              LEX_TKN_LCURLY);
			break;
		case LEX_IS_RCURLY:
			ELSE  (              LEX_TKN_RCURLY);
			break;
		case LEX_IS_CHR_LIT:
			ELSE  (              LEX_TKN_CHR_LIT);
			break;
		default:                                     // (No matching state)
			// Start at non-state? compiler bug.
			Terminate::inst().assert("nextState() with non-state");
			break;
	}

	// Identify keyword as called for.
	if (doKword) {
		// Attempt keyword match first.
		KWORD(buffer, "char",     LEX_TKN_CHAR);
		KWORD(buffer, "int",      LEX_TKN_INT);
		KWORD(buffer, "void",     LEX_TKN_VOID);
		KWORD(buffer, "unsigned", LEX_TKN_UNSIGNED);
		KWORD(buffer, "volatile", LEX_TKN_VOLATILE);

		// Otherwise, it's an identifier.
		return LEX_TKN_ID;
	}

	// If reached, then 1+ conditions were not met.
	return LEX_ERROR;
}

//==============================================================================
// Helper callback function to identify terminal lex states.
static bool SubStepLexFile_isTkn(int const state) {
	// All "tokens" are under set limit.
	return (state <= LEX_TKN_MAX);
}

//==============================================================================
// Lexes opened file into list of tokens. Does not terminate on error.
void SubStepLexFile_execute(DataModel_t& model, std::queue<LexToken*>& tkns) {
	// Ease of access.
	string fname = File::inst().getName();

	// Prep for lexing.
	LexInit_t initData = {
		.m_nextState = SubStepLexFile_nextState,
		.m_isTkn     = SubStepLexFile_isTkn,
		.m_lexStart  = LEX_START,
		.m_lexError  = LEX_ERROR
	};
	Lexer lexer(initData);

	// Create tokens from lexed file.
	do {
		// Lex token.
		lexer.lex();

		// Create/save token as applicable.
		if (SubStepLexFile_isTkn(lexer.m_tknCode)) {
			// Valid lexing- create...
			LexToken* newTkn = new LexToken();
			IF_NULL(newTkn, "LexToken() failed");

			// ...populate...
			newTkn->m_type    = lexer.m_tknCode; // token: type = specific token
			newTkn->m_rawData = lexer.m_value;
			newTkn->m_file    = fname;
			newTkn->m_line    = lexer.m_line;

			// ...save.
			tkns.push(newTkn);
		}

		// Break out early for lexing error.
		if (lexer.m_tknCode == LEX_ERROR) {
			string errStr = string("Unknown sequence '") + lexer.m_value + "'";
			Print::inst().log(LOG_ERROR, fname, lexer.m_line, errStr);
			InfoUtil_recordError(model.m_summary, RET_BAD_LEX);
			break;
		}
	} while(lexer.m_tknCode != LEX_TKN_EOF);

	// (Final debug report on lexing.)
	string dbgStr = to_string(tkns.size()) + " lex tokens found";
	Print::inst().log(LOG_DEBUG, fname, dbgStr);
}
