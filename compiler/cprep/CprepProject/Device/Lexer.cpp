/*
 * Lexer.cpp: Functions containing language specific lexing rules/support.
 */

#include "Device/Terminate.h"

#include "Device/Lexer.h"

using namespace std;

//==============================================================================

// Definitions for creating state transition macros.
#define _IS(x)   ((x) == byte)
#define _IN(x,y) (((x) <= byte) && (byte <= (y)))

// Definitions for easing reading transition conditions.
#define IS(x)   if (_IS(x))
#define IN(x,y) if (_IN((x),(y)))
#define LABEL   if (_IN('a', 'z') || _IN('A', 'Z') || _IN('0', '9') || _IS('_'))
#define HEX     if (_IN('a', 'f') || _IN('A', 'F') || _IN('0', '9'))
#define WSPACE  if (_IS(' ') || _IS('\t') || _IS('\n') || _IS('\r'))
#define ENDOF   if (_IS(0xFF))
#define ELSE    if (true)

// Definitions for easing reading state transitions.
#define TO(x)   {retState = (LexState_e)(x); break;}

//==============================================================================
// Returns the given state as a LexToken. Returns TOKEN_INVALID if not a token.
LexToken_e Lexer_asToken(LexState_e const state) {
	// Returned token- GUILTY till innocent.
	LexToken_e retTkn = TOKEN_INVALID;

	// Recast to token if within range.
	LexToken_e asTkn = (LexToken_e)(state);
	if ((TOKEN_LEX_MIN <= asTkn) && (asTkn <= TOKEN_LEX_MAX)) {retTkn = asTkn;}

	// Return result of cast.
	return retTkn;
}

//==============================================================================
// Returns next lex state for given current lex state and input.
LexState_e Lexer_nextState(LexState_e const state, uint8_t const byte) {
	// GUILTY till innocent.
	LexState_e retState = LEX_ERROR;

	// "And so it begins" - King Theoden.
	switch (state) {
		case LEX_START:                          // (Start of all lex paths)
			ENDOF        TO(TOKEN_EOF)           // last token scanned
			IS('\n')     TO(LEX_FOUND_TEXT)      // 1) newline -> source text
			WSPACE       TO(LEX_START)           // 2) other wspace -> prefix
			IS('"')      TO(LEX_LOOP_FILENAME)
			IS('#')      TO(LEX_LOOP_DIRECTIVE)
			ELSE         TO(LEX_LOOP_TEXT)
			break;
		case LEX_LOOP_TEXT:
			IS('\n')     TO(LEX_FOUND_TEXT)      // grab newline before ending
			ENDOF        TO(TOKEN_LINE_TEXT)     // EOF- end without newline
			ELSE         TO(LEX_LOOP_TEXT)
			break;
		case LEX_LOOP_FILENAME:
			IS('"')      TO(LEX_LOOP_FILE_FIN)   // enclosed file- now must end
			IS('\n')     TO(LEX_FOUND_TEXT)      // just source text
			ENDOF        TO(TOKEN_LINE_TEXT)     // just source text
			ELSE         TO(LEX_LOOP_FILENAME)
			break;
		case LEX_LOOP_FILE_FIN:
			IS('\n')     TO(LEX_FOUND_FILE)      // grab newline before ending
			ENDOF        TO(TOKEN_LINE_FILE)     // EOF- end without newline
			WSPACE       TO(LEX_LOOP_FILE_FIN)   // (other wspace- postfix)
			ELSE         TO(LEX_LOOP_TEXT)       // poisoned- just source text
			break;
		case LEX_LOOP_DIRECTIVE:
			IN('a', 'z') TO(LEX_LOOP_DIRECTIVE)  // keywords use low alphabet
			ELSE         TO(LEX_DIRECTIVE)       // mismatch -> end w/o char
			break;
		case LEX_FOUND_TEXT:
			ELSE         TO(TOKEN_LINE_TEXT)     // grab newline -> end
			break;
		case LEX_FOUND_FILE:
			ELSE         TO(TOKEN_LINE_FILE)     // grab newline -> end
			break;
		default:                                 // (No matching "from" state)
			// Start at non-state? compiler bug.
			Terminate_assert("Lexed from invalid starting state");
			break;
	}

	// Return state that would be transitioned to for the given inputs.
	return retState;
}
