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
#include "Util/StrUtil.h"

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
			IS    (byte,'#',     LEX_LOOP_HASH);
			IS    (byte,'"',     LEX_LOOP_INC_FILE);
			IS    (byte,'_',     LEX_LOOP_DEF_WORD);
			IN    (byte,'a','z', LEX_LOOP_DEF_WORD);
			IN    (byte,'A','Z', LEX_LOOP_DEF_WORD);
			IS    (byte,' ',     LEX_LOOP_PRE_WS);
			IS    (byte,'\r',    LEX_LOOP_PRE_WS);
			IS    (byte,'\t',    LEX_LOOP_PRE_WS);
			IS    (byte,'\n',    LEX_END_TEXT);
			ELSE  (              LEX_LOOP_TEXT);
			break;
		case LEX_LOOP_PRE_WS:
			EOFILE(byte,         LEX_TKN_TEXT);
			IS    (byte,'#',     LEX_LOOP_HASH);
			IS    (byte,'"',     LEX_LOOP_INC_FILE);
			IS    (byte,'_',     LEX_LOOP_DEF_WORD);
			IN    (byte,'a','z', LEX_LOOP_DEF_WORD);
			IN    (byte,'A','Z', LEX_LOOP_DEF_WORD);
			IS    (byte,' ',     LEX_LOOP_PRE_WS);
			IS    (byte,'\r',    LEX_LOOP_PRE_WS);
			IS    (byte,'\t',    LEX_LOOP_PRE_WS);
			IS    (byte,'\n',    LEX_END_TEXT);
			ELSE  (              LEX_LOOP_TEXT);
			break;
		case LEX_LOOP_TEXT:
			EOFILE(byte,         LEX_TKN_TEXT);      // EOF is separate token
			IS    (byte,'\n',    LEX_END_TEXT);
			ELSE  (              LEX_LOOP_TEXT);
			break;
		case LEX_LOOP_HASH:
			IN    (byte,'a','z', LEX_LOOP_HASH);
			doKword = true;                          // ELSE- identify keyword
			break;
		case LEX_LOOP_INC_FILE:
			EOFILE(byte,         LEX_TKN_TEXT);      // incomplete- source text
			IS    (byte,'"',     LEX_LOOP_INC_END);  // grab full file include
			ELSE  (              LEX_LOOP_INC_FILE);
			break;
		case LEX_LOOP_INC_END:
			EOFILE(byte,         LEX_TKN_FILE);
			IS    (byte,' ',     LEX_LOOP_INC_END);
			IS    (byte,'\r',    LEX_LOOP_INC_END);
			IS    (byte,'\t',    LEX_LOOP_INC_END);
			IS    (byte,'\n',    LEX_END_FILE);
			ELSE  (              LEX_LOOP_TEXT);     // ope- just source text
			break;
		case LEX_LOOP_DEF_WORD:
			EOFILE(byte,         LEX_TKN_DEF);       // EOF is separate token
			LABEL (byte,         LEX_LOOP_DEF_WORD);
			IS    (byte,' ',     LEX_LOOP_DEF_END);
			IS    (byte,'\r',    LEX_LOOP_DEF_END);
			IS    (byte,'\t',    LEX_LOOP_DEF_END);
			IS    (byte,'\n',    LEX_END_DEF);
			ELSE  (              LEX_LOOP_TEXT);     // ope- just source text
			break;
		case LEX_LOOP_DEF_END:
			EOFILE(byte,         LEX_TKN_DEF);
			IS    (byte,' ',     LEX_LOOP_DEF_END);
			IS    (byte,'\r',    LEX_LOOP_DEF_END);
			IS    (byte,'\t',    LEX_LOOP_DEF_END);
			IS    (byte,'\n',    LEX_END_DEF);
			ELSE  (              LEX_LOOP_TEXT);     // ope- just source text
			break;
		case LEX_END_TEXT:
			ELSE  (              LEX_TKN_TEXT);      // last state to include \n
			break;
		case LEX_END_FILE:
			ELSE  (              LEX_TKN_FILE);      // last state to include \n
			break;
		case LEX_END_DEF:
			ELSE  (              LEX_TKN_DEF);       // last state to include \n
		default:                                     // (No matching state)
			// Start at non-state? compiler bug.
			Terminate::inst().assert("nextState() with non-state");
			break;
	}

	// Identify keyword as called for.
	if (doKword) {
		// Get core keyword.
		string trimBuf = buffer;
		StrUtil_trim(trimBuf);

		// Determine/return.
		KWORD(trimBuf, "#include", LEX_TKN_INCLUDE);
		KWORD(trimBuf, "#define",  LEX_TKN_DEFINE);
		KWORD(trimBuf, "#ifdef",   LEX_TKN_IFDEF);
		KWORD(trimBuf, "#ifndef",  LEX_TKN_IFNDEF);
		KWORD(trimBuf, "#endif",   LEX_TKN_ENDIF);
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

	// (Note entrance into sub-step.)
	Print::inst().log(LOG_DEBUG, fname, "Beginning lexing...");

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
