/*
 * SubStepLexFile.cpp: Substep to lex file as a series of tokens.
 */

#include "Device/File.h"
#include "Device/Lexer.h"
#include "Device/Print.h"
#include "Domain/LexState_e.h"
#include "Util/ModelUtil.h"

#include "State/SubStepLexFile.h"

using namespace std;

//==============================================================================

// Definitions for expected characters.
#define CHAR_NEWLINE (0x0A)

// Definitions for expected keywords.
#define KW_SHR       ("SHR")

// (Targeted macros to simplify the parsing process.)
#define KEYWORD(x,y) (0 == buffer.compare(x)) {tkn = (y);}

//==============================================================================
// Helper function to parse "names" specific to assembly language syntax.
static void SubStepLexFile_parseName(std::string const& buffer,
		                             LexState_e& state,
		                             LexToken_e& tkn) {
	// By default, names are NOT tokens // TODO- until they're labels
	tkn = TOKEN_INVALID;

	// Set token as keyword as applicable.
	if KEYWORD(KW_SHR, TOKEN_KW_SHR);

	// Set state based on process.
	if (tkn == TOKEN_INVALID) {state = LEX_ERROR;}
}

//==============================================================================
// Executes sub-process to lex opened file into a series of file tokens.
RetErr_e SubStepLexFile_execute(DataModel_t& model,
		                        std::vector<FileToken_t>& tkns) {
	// Result of the process.
	RetErr_e retErr = RET_ERR_NONE; // INNOCENT till guilty

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
			if (lexState == LEX_NAME) {
				SubStepLexFile_parseName(lexBuffer, lexState, lexTkn);
			}

			// Handle buffer- append or clear if not yet a token.
			if (lexTkn   == TOKEN_INVALID) {lexBuffer += File::inst().pop();}
			if (lexState == LEX_START)     {lexBuffer.clear();}

			// Record lexical errors.
			if (lexState == LEX_ERROR) {
				// Mark error in log.
				string errStr = string("Unrecognized sequence '") +
						        lexBuffer                         +
								"'";
				Print::inst().log(LOG_ERROR, file, line, errStr);
				ModelUtil_recordError(model, RET_BAD_LEX);

				// Mark error locally.
				retErr = RET_ERR_ERROR;
			}

			// Keep line count updated.
			if (peekByte == CHAR_NEWLINE) {
				// (Debug-able details).
				string dbgStr = to_string(tkns.size()) + " tokens thru line";
				Print::inst().log(LOG_DEBUG, file, line, dbgStr);

				// Update line count.
				line++;
			}

			// If error was hit, break out to return.
			if (retErr) {break;}
		}

		// Save meaningful tokens to given list.
		if ((retErr == RET_ERR_NONE) && (lexTkn != TOKEN_COMMENT)) {
			// Create file token.
			FileToken_t newTkn;
			newTkn.m_lexTkn = lexTkn;
			newTkn.m_rawStr = lexBuffer;
			newTkn.m_file   = file;
			newTkn.m_line   = line;

			// Save to list.
			tkns.push_back(newTkn);
		}

		// If error was hit, break out to return.
		if (retErr) {break;}
	}

	// (Final debug report on lexing.)
	string dbgStr = to_string(tkns.size()) + " tokens total";
	Print::inst().log(LOG_DEBUG, file, dbgStr);

	// Return result of the process.
	return retErr;
}

