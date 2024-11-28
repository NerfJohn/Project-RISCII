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
			WSPACE       TO(LEX_START)           // skip token "spacers"
			ENDOF        TO(TOKEN_EOF)           // last token scanned
			IS(';')      TO(LEX_LOOP_COMMENT)
			IS('%')      TO(LEX_HANDLE_PERCENT)
			IS('$')      TO(LEX_HANDLE_DOLLAR)
			IS('-')      TO(LEX_HANDLE_MINUS)
			IS('0')      TO(LEX_HANDLE_ZERO)
			IN('1','9')  TO(LEX_LOOP_DECIMAL)
			IS('.')      TO(LEX_LOOP_DIRECTIVE)  // covers all directives
			LABEL        TO(LEX_LOOP_NAME)       // covers labels/keywords
			break;
		case LEX_LOOP_COMMENT:
			IS('\n')     TO(TOKEN_COMMENT)
			ENDOF        TO(TOKEN_COMMENT)       // ensure EOF is seen
			ELSE         TO(LEX_LOOP_COMMENT)    // greedy grab comments
			break;
		case LEX_HANDLE_PERCENT:
			IN('a','z')  TO(LEX_LOOP_FLAGS)
			break;
		case LEX_LOOP_FLAGS:
			IN('a','z')  TO(LEX_LOOP_FLAGS)      // greedy grab flags
			ELSE         TO(TOKEN_FLAGS)
			break;
		case LEX_HANDLE_DOLLAR:
			IN('0','9')  TO(LEX_LOOP_REGISTER)
			break;
		case LEX_LOOP_REGISTER:
			IN('0','9')  TO(LEX_LOOP_REGISTER)   // greedy grab register #
			ELSE         TO(TOKEN_REGISTER)
			break;
		case LEX_HANDLE_MINUS:
			IN('0','9')  TO(LEX_LOOP_DECIMAL)    // -[0-9]...
			break;
		case LEX_HANDLE_ZERO:
			IS('x')      TO(LEX_HANDLE_HEX)      // 0x...
			IN('0','9')  TO(LEX_LOOP_DECIMAL)    // 0[0-9]...
			break;
		case LEX_HANDLE_HEX:
			HEX          TO(LEX_LOOP_HEX)        // 0x[hex num]...
			break;
		case LEX_LOOP_DECIMAL:
			IN('0','9')  TO(LEX_LOOP_DECIMAL)    // greedy grab decimals
			ELSE         TO(TOKEN_IMMEDIATE)
			break;
		case LEX_LOOP_HEX:
			HEX          TO(LEX_LOOP_HEX)        // greedy grab hex
			ELSE         TO(TOKEN_IMMEDIATE)
			break;
		case LEX_LOOP_NAME:
			LABEL        TO(LEX_LOOP_NAME)       // greedy grab label chars
			ELSE         TO(LEX_NAME)            // signal "label" is ready
			break;
		case LEX_LOOP_DIRECTIVE:
			IN('a','z')  TO(LEX_LOOP_DIRECTIVE)
			ELSE         TO(LEX_DIRECTIVE)
			break;
		default:                                 // (No matching "from" state)
			// Start at non-state? compiler bug.
			Terminate_assert("Lexed from invalid starting state");
			break;
	}

	// Return state that would be transitioned to for the given inputs.
	return retState;
}
