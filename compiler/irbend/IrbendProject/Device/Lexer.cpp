/*
 * Lexer.cpp: Class containing language specific lexing rules/support.
 */

#include "Device/Print.h"
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
			IS('%')      TO(LEX_LOOP_SYMBOL)
			IS('-')      TO(LEX_IS_MINUS)
			IN('0','9')  TO(LEX_LOOP_INT)
			IS('/')      TO(LEX_IS_SLASH)
			IS('=')      TO(LEX_IS_EQUALS)
			IS('{')      TO(LEX_IS_LCURLY)
			IS('}')      TO(LEX_IS_RCURLY)
			IS(',')      TO(LEX_IS_COMMA)
			IS('d')      TO(LEX_KW_D)
			IS('g')      TO(LEX_KW_G)
			IS('i')      TO(LEX_KW_I)
			break;
		case LEX_LOOP_SYMBOL:
			LABEL        TO(LEX_LOOP_SYMBOL)     // greedy grab symbol name
			ELSE         TO(TOKEN_SYMBOL)
			break;
		case LEX_IS_MINUS:
			IN('0','9')  TO(LEX_LOOP_INT)        // negation of integer value
			break;
		case LEX_LOOP_INT:
			IN('0','9')  TO(LEX_LOOP_INT)        // greedy grab integer
			ELSE         TO(TOKEN_INT_LIT)
			break;
		case LEX_IS_SLASH:
			IS('/')      TO(LEX_LOOP_COMMENT)
			break;
		case LEX_LOOP_COMMENT:
			IS('\n')     TO(TOKEN_COMMENT)
			ELSE         TO(LEX_LOOP_COMMENT)    // greedy grab comment
			break;
		case LEX_IS_EQUALS:                      // (operators/formatters)
			ELSE         TO(TOKEN_EQUALS)
			break;
		case LEX_IS_LCURLY:
			ELSE         TO(TOKEN_L_CURLY)
			break;
		case LEX_IS_RCURLY:
			ELSE         TO(TOKEN_R_CURLY)
			break;
		case LEX_IS_COMMA:
			ELSE         TO(TOKEN_COMMA)
			break;
		case LEX_KW_D:                           // ("d" keywords)
			IS('e')      TO(LEX_KW_DE)
			break;
		case LEX_KW_DE:
			IS('c')      TO(LEX_KW_DEC)
			IS('f')      TO(LEX_KW_DEF)
			break;
		case LEX_KW_DEC:
			LABEL        TO(LEX_ERROR)
			ELSE         TO(TOKEN_KW_DEC)
			break;
		case LEX_KW_DEF:
			LABEL        TO(LEX_ERROR)
			ELSE         TO(TOKEN_KW_DEF)
			break;
		case LEX_KW_G:                           // ("g" keywords)
			IS('l')      TO(LEX_KW_GL)
			break;
		case LEX_KW_GL:
			IS('o')      TO(LEX_KW_GLO)
			break;
		case LEX_KW_GLO:
			IS('b')      TO(LEX_KW_GLOB)
			break;
		case LEX_KW_GLOB:
			LABEL        TO(LEX_ERROR)
			ELSE         TO(TOKEN_KW_GLOB)
			break;
		case LEX_KW_I:                           // ("i" keywords)
			IS('1')      TO(LEX_KW_I1)
			IS('8')      TO(LEX_KW_I8)
			break;
		case LEX_KW_I1:
			IS('6')      TO(LEX_KW_I16)
			break;
		case LEX_KW_I8:
			LABEL        TO(LEX_ERROR)
			ELSE         TO(TOKEN_I8)
			break;
		case LEX_KW_I16:
			LABEL        TO(LEX_ERROR)
			ELSE         TO(TOKEN_I16)
			break;
		default:                                 // (No matching "from" state)
			// Start at non-state? compiler bug.
			Print::inst().bug("Lexed from invalid starting state");
			Terminate_silent(RET_ASSERT);
			break;
	}

	// Return state that would be transitioned to for the given inputs.
	return retState;
}
