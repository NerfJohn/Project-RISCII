/*
 * SubStepLexFile.cpp: Substep to lex file as a series of tokens.
 */

#include "Device/File.h"
#include "Device/Lexer.h"
#include "Device/Print.h"
#include "Device/Terminate.h"
#include "Util/ModelUtil.h"

#include "State/SubStepLexFile.h"

using namespace std;

//==============================================================================

// Definitions for expected characters.
#define CHAR_NEWLINE (0x0A)

// Definitions for expected keywords.
#define KW_DATA      (".data")
#define KW_BSS       (".bss")
#define KW_GLOBAL    (".global")
#define KW_WEAK      (".weak")
#define KW_LA        ("_la")
#define KW_AND       ("AND")
#define KW_ORR       ("ORR")
#define KW_XOR       ("XOR")
#define KW_SHL       ("SHL")
#define KW_SHR       ("SHR")
#define KW_ADD       ("ADD")
#define KW_SUB       ("SUB")
#define KW_LBI       ("LBI")
#define KW_LDR       ("LDR")
#define KW_STR       ("STR")
#define KW_BRC       ("BRC")
#define KW_JPR       ("JPR")
#define KW_JLR       ("JLR")
#define KW_SWP       ("SWP")
#define KW_NOP       ("NOP")
#define KW_HLT       ("HLT")

// (Targeted macros to simplify the parsing process.)
#define KEYWORD(x,y) (0 == buffer.compare(x)) {tkn = (y);}

//==============================================================================
// Helper function to parse "names" specific to assembly language syntax.
static void SubStepLexFile_parseName(std::string const& buffer,
		                             LexState_e& state,
		                             LexToken_e& tkn) {
	// By default, names are labels.
	tkn = TOKEN_LABEL;

	// Set token as keyword as applicable.
	if KEYWORD(KW_LA,  TOKEN_KW_LA);
	if KEYWORD(KW_AND, TOKEN_KW_AND);
	if KEYWORD(KW_ORR, TOKEN_KW_ORR);
	if KEYWORD(KW_XOR, TOKEN_KW_XOR);
	if KEYWORD(KW_SHL, TOKEN_KW_SHL);
	if KEYWORD(KW_SHR, TOKEN_KW_SHR);
	if KEYWORD(KW_ADD, TOKEN_KW_ADD);
	if KEYWORD(KW_SUB, TOKEN_KW_SUB);
	if KEYWORD(KW_LBI, TOKEN_KW_LBI);
	if KEYWORD(KW_LDR, TOKEN_KW_LDR);
	if KEYWORD(KW_STR, TOKEN_KW_STR);
	if KEYWORD(KW_BRC, TOKEN_KW_BRC);
	if KEYWORD(KW_JPR, TOKEN_KW_JPR);
	if KEYWORD(KW_JLR, TOKEN_KW_JLR);
	if KEYWORD(KW_SWP, TOKEN_KW_SWP);
	if KEYWORD(KW_NOP, TOKEN_KW_NOP);
	if KEYWORD(KW_HLT, TOKEN_KW_HLT);
}

//==============================================================================
// Helper function to parse "directives" specific to assembly language syntax.
static void SubStepLexFile_parseDirective(std::string const& buffer,
		                                  LexState_e& state,
		                                  LexToken_e& tkn) {
	// Either a directive or an error.
	tkn = TOKEN_INVALID;

	// Set token as keyword as applicable.
	if KEYWORD(KW_DATA,   TOKEN_KW_DATA);
	if KEYWORD(KW_BSS,    TOKEN_KW_BSS);
	if KEYWORD(KW_GLOBAL, TOKEN_KW_GLOBAL);
	if KEYWORD(KW_WEAK,   TOKEN_KW_WEAK);

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
			if (lexState == LEX_NAME) {
				SubStepLexFile_parseName(lexBuffer, lexState, lexTkn);
			}
			if (lexState == LEX_DIRECTIVE) {
				SubStepLexFile_parseDirective(lexBuffer, lexState, lexTkn);
			}

			// Handle buffer- append or clear if not yet a token.
			if (lexTkn   == TOKEN_INVALID) {lexBuffer += File::inst().pop();}
			if (lexState == LEX_START)     {lexBuffer.clear();}

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
		if ((model.m_numErrs == 0) && (lexTkn != TOKEN_COMMENT)) {
			// Create file token.
			ItemToken* newTkn = new ItemToken();
			if (newTkn == nullptr) {Terminate_assert("Couldn't create token");}

			// Populate with lexed data.
			newTkn->m_lexTkn = lexTkn;
			newTkn->m_rawStr = lexBuffer;
			newTkn->m_file   = file;
			newTkn->m_line   = line;

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

