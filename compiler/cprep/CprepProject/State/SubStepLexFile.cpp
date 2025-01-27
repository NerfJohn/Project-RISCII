/*
 * SubStepLexFile.cpp: Substep to lex file as a series of tokens.
 */

#include "Device/File.h"
#include "Device/Lexer.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/ModelUtil.h"
#include "Util/StrUtil.h"

#include "State/SubStepLexFile.h"

using namespace std;

//==============================================================================

// Definitions for expected characters.
#define CHAR_NEWLINE (0x0A)

// Definitions for cprep.exe keywords.
#define KW_INCLUDE ("#include")

// (Targeted macros to simplify the parsing process.)
#define KEYWORD(x,y) (0 == buffer.compare(x)) {tkn = (y);}

//==============================================================================
// Helper function to parse "directives" specific to assembly language syntax.
static void SubStepLexFile_parseDirective(std::string& buffer,
		                                  LexState_e& state,
		                                  LexToken_e& tkn) {
	// Either a directive or an error.
	tkn = TOKEN_INVALID;

	// Directives don't have/need extra whitespace.
	StrUtil_trim(buffer);

	// Set token as keyword as applicable.
	if KEYWORD(KW_INCLUDE,   TOKEN_KW_INCLUDE);

	// Set state based on process.
	if (tkn == TOKEN_INVALID) {state = LEX_ERROR;}
}

//==============================================================================
// Executes sub-process to lex opened file into a series of file tokens.
void SubStepLexFile_execute(DataModel_t& model, std::queue<ItemToken*>& tkns) {
	// Commonly referenced variables during lexing.
	string   file = File::inst().getName(); // name of lexed file
	uint32_t line = 1;                      // files start with line 1

	// Lex file until EOF token has been acquired.
	LexToken_e lexTkn = TOKEN_INVALID; // among other things, not EOF
	while (lexTkn != TOKEN_EOF) {
		// Prep for next token.
		LexState_e lexState  = LEX_START;
		string     lexBuffer = "";
		           lexTkn    = TOKEN_INVALID;

		// Scan bytes until a token is found.
		while (lexTkn == TOKEN_INVALID) {
			// Get the next state.
			uint8_t peekByte = File::inst().peek();
			        lexState = Lexer_nextState(lexState, peekByte);
			        lexTkn   = Lexer_asToken(lexState);

			// Handle special lex cases requiring user aid.
			if (lexState == LEX_DIRECTIVE) {
				SubStepLexFile_parseDirective(lexBuffer, lexState, lexTkn);
			}

			// Handle buffer- append or clear if not yet a token.
			if (lexTkn   == TOKEN_INVALID) {lexBuffer += File::inst().pop();}
			// NOTE: Removed "whitespace clear" condition- "copy" capturing. //

			// Record lexical errors.
			if (lexState == LEX_ERROR) {
				string errStr = string("Unrecognized sequence '") +
						        lexBuffer                         +
								"'";
				Print::inst().log(LOG_ERROR, file, line, errStr);
				ModelUtil_recordError(model, RET_BAD_LEX);
			}

			// Keep line count updated (though only when newline is "popped").
			if ((peekByte == CHAR_NEWLINE) && (lexTkn == TOKEN_INVALID)) {
				// (Debug-able details).
				string dbgStr = to_string(tkns.size()) + " tokens thru line";
				Print::inst().log(LOG_DEBUG, file, line, dbgStr);

				// Update line count.
				line++;
			}

			// If error was hit, break out to return.
			if (model.m_numErrs > 0) {break;}
		}

		// Save meaningful tokens to given list.
		if (model.m_numErrs == 0) {
			// Create file token.
			ItemToken* newTkn = new ItemToken();
			if (newTkn == nullptr) {Terminate_assert("Couldn't create token");}

			// (Additional processing to properly locate "to newline" tokens.)
			bool isEol = ((lexBuffer.size() > 0) &&
					      (lexBuffer.back() == CHAR_NEWLINE));

			// Populate with lexed data.
			newTkn->m_lexTkn = lexTkn;
			newTkn->m_rawStr = lexBuffer;
			newTkn->m_file   = file;
			newTkn->m_line   = (isEol) ? line - 1 : line;

			// Save to list.
			tkns.push(move(newTkn)); // (pass ownership)
		}

		// If error was hit, break out to return.
		if (model.m_numErrs > 0) {break;}
	}

	// (Final debug report on lexing.)
	string dbgStr = to_string(tkns.size()) + " tokens total";
	Print::inst().log(LOG_DEBUG, file, dbgStr);
}
