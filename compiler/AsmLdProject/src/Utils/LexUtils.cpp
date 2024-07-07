/*
 * LexUtils.cpp
 *
 * "Helper functions for lexing an input assembly language file"
 */

#include "Utils/LexUtils.h"

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
#define EOF     if (_IS(0xFF))
#define ELSE    if (true)

// Definitions for easing reading state transitions.
#define TO(x)   {retState = (x); break;}
#define HAVE(x) {retState = (LexState_e)(x); break;}

//==============================================================================
// Returns the given state as a LexToken. Returns TOKEN_INVALID if not a token.
LexToken_e LexUtils_asLexToken(LexState_e state) {
	// Can cast it if it's within a lex token's range.
	LexToken_e stateAsToken = (LexToken_e)(state);
	if ((TOKEN_LEX_MIN <= stateAsToken) && (stateAsToken <= TOKEN_LEX_MAX)) {
		return stateAsToken;
	}

	// Otherwise, it becomes an unknown/invalid token type.
	return TOKEN_INVALID;
}

//==============================================================================
// Returns next lex state for given current lex state and input.
LexState_e LexUtils_nextState(LexState_e state, uint8_t byte) {
	// Assume state transition failed until proven otherwise.
	LexState_e retState = LEX_ERROR;

	// "And so it begins" - King Theoden.
	switch (state) {
		case LEX_START:                          // start of all tokens
			WSPACE       TO(LEX_START)           // skip token "spacers"
			EOF          HAVE(TOKEN_EOF)         // last token to scan
			IS('/')      TO(LEX_SLASH_HANDLE)
			IS('%')      TO(LEX_FLAG_START)
			IS('$')      TO(LEX_REG_START)
			IS('0')      TO(LEX_ZERO_HANDLE)
			IS('-')      TO(LEX_MINUS_HANDLE)
			IN('1', '9') TO(LEX_DECIMAL_LOOP)
			IS('S')      TO(LEX_S)
			break;
		case LEX_SLASH_HANDLE:                   // /...
			IS('/')      TO(LEX_COMMENT_LOOP)
			break;
		case LEX_COMMENT_LOOP:                   // greedy grab comment
			IS('\n')     HAVE(TOKEN_COMMENT)
			EOF          HAVE(TOKEN_COMMENT)
			ELSE         TO(LEX_DECIMAL_LOOP)
			break;
		case LEX_FLAG_START:                     // %...
			IN('a', 'z') TO(LEX_FLAG_LOOP)
			break;
		case LEX_FLAG_LOOP:                      // greedy grab flags
			IN('a', 'z') TO(LEX_FLAG_LOOP)
			ELSE         HAVE(TOKEN_FLAGS)
			break;
		case LEX_REG_START:                      // $...
			IN('0', '9') TO(LEX_REG_LOOP)
			break;
		case LEX_REG_LOOP:                       // greedy grab reg #s
			IN('0', '9') TO(LEX_REG_LOOP)
			ELSE         HAVE(TOKEN_REGISTER)
			break;
		case LEX_ZERO_HANDLE:                    // 0: 0x or decimal?
			IS('x')      TO(LEX_HEX_START)
			IN('0', '9') TO(LEX_DECIMAL_LOOP)
			ELSE         HAVE(TOKEN_IMMEDIATE)
			break;
		case LEX_MINUS_HANDLE:                   // -...
			IN('0', '9') TO(LEX_DECIMAL_LOOP)
			break;
		case LEX_HEX_START:                      // 0x...
			HEX          TO(LEX_HEX_LOOP)
			break;
		case LEX_HEX_LOOP:                       // greedy grab hex digits
			HEX          TO(LEX_HEX_LOOP)
			ELSE         HAVE(TOKEN_IMMEDIATE)
			break;
		case LEX_DECIMAL_LOOP:                   // greedy grab decimal digits
			IN('0', '9') TO(LEX_DECIMAL_LOOP)
			ELSE         HAVE(TOKEN_IMMEDIATE)
			break;
		case LEX_S:                              // "S" keywords
			IS('H')      TO(LEX_SH)
			break;
		case LEX_SH:
			IS('R')      TO(LEX_SHR)
			break;
		case LEX_SHR:
			LABEL        TO(LEX_ERROR)
			ELSE         HAVE(TOKEN_SHR)
			break;
		default:
			// Transition from non-transition state- error.
			retState = LEX_ERROR;
	}

	// Return state that would be transitioned to for the given inputs.
	return retState;
}

