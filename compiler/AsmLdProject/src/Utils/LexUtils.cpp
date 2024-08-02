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
			IS(';')      TO(LEX_COMMENT_LOOP)
			IS('%')      TO(LEX_FLAG_START)
			IS('$')      TO(LEX_REG_START)
			IS('0')      TO(LEX_ZERO_HANDLE)
			IS('-')      TO(LEX_MINUS_HANDLE)
			IN('1', '9') TO(LEX_DECIMAL_LOOP)
			IS('"')      TO(LEX_STR_LOOP)
			IS(':')      TO(LEX_COLON_FOUND)
			IS('{')      TO(LEX_LCURLY_FOUND)
			IS('}')      TO(LEX_RCURLY_FOUND)
			IS('_')      TO(LEX__)
			IS('.')      TO(LEX_d)
			IS('A')      TO(LEX_A)
			IS('O')      TO(LEX_O)
			IS('S')      TO(LEX_S)
			IS('X')      TO(LEX_X)
			LABEL        TO(LEX_LABEL_LOOP)      // check after specifics chars
			break;
		case LEX_COMMENT_LOOP:                   // greedy grab comment
			IS('\n')     TO(LEX_COMMENT_NEWLINE)
			EOF          HAVE(TOKEN_COMMENT)
			ELSE         TO(LEX_COMMENT_LOOP)
			break;
		case LEX_COMMENT_NEWLINE:                // end of comment with newline
			ELSE         HAVE(TOKEN_COMMENT)
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
		case LEX_LABEL_LOOP:                     // greedy grab label chars
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN_LABEL)
			break;
		case LEX_STR_LOOP:                       // string lit- loop over chars
			IS('\\')     TO(LEX_STR_ESC)
			IS('"')      TO(LEX_STR_END)
			IS('\n')     TO(LEX_ERROR)
			EOF          TO(LEX_ERROR)
			ELSE         TO(LEX_STR_LOOP)
			break;
		case LEX_STR_ESC:                        // string escaped char
			IS('\n')     TO(LEX_ERROR)
			EOF          TO(LEX_ERROR)
			ELSE         TO(LEX_STR_LOOP)
			break;
		case LEX_STR_END:                        // valid string literal found
			ELSE         HAVE(TOKEN_STR_LIT)
			break;
		case LEX_COLON_FOUND:                    // guaranteed colon token
			ELSE         HAVE(TOKEN_COLON)
			break;
		case LEX_LCURLY_FOUND:                   // guaranteed left curly token
			ELSE         HAVE(TOKEN_LCURLY)
			break;
		case LEX_RCURLY_FOUND:                   // guaranteed right curly token
			ELSE         HAVE(TOKEN_RCURLY)
			break;
		case LEX__:                              // "_la" keyword
			IS('l')      TO(LEX__l)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX__l:
			IS('a')      TO(LEX__la)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX__la:
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN__la)
			break;
		case LEX_d:                              // ".data/.bss" keywords
			IS('b')      TO(LEX_db)
			IS('d')      TO(LEX_dd)
			break;
		case LEX_db:
			IS('s')      TO(LEX_dbs)
			break;
		case LEX_dbs:
			IS('s')      TO(LEX_dbss)
			break;
		case LEX_dbss:
			ELSE         HAVE(TOKEN_dbss)
			break;
		case LEX_dd:
			IS('a')      TO(LEX_dda)
			break;
		case LEX_dda:
			IS('t')      TO(LEX_ddat)
			break;
		case LEX_ddat:
			IS('a')      TO(LEX_ddata)
			break;
		case LEX_ddata:
			ELSE         HAVE(TOKEN_ddata)
			break;
		case LEX_A:                              // "A" keywords
			IS('N')      TO(LEX_AN)
			IS('D')      TO(LEX_AD)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_AN:
			IS('D')      TO(LEX_AND)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_AD:
			IS('D')      TO(LEX_ADD)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_AND:
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN_AND)
			break;
		case LEX_ADD:
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN_ADD)
			break;
		case LEX_O:                              // "O" keywords
			IS('R')      TO(LEX_OR)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_OR:
			IS('R')      TO(LEX_ORR)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_ORR:
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN_ORR)
			break;
		case LEX_S:                              // "S" keywords
			IS('H')      TO(LEX_SH)
			IS('U')      TO(LEX_SU)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_SH:
			IS('L')      TO(LEX_SHL)
			IS('R')      TO(LEX_SHR)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_SU:
			IS('B')      TO(LEX_SUB)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_SHL:
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN_SHL)
			break;
		case LEX_SHR:
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN_SHR)
			break;
		case LEX_SUB:
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN_SUB)
			break;
		case LEX_X:                              // "X" keywords
			IS('O')      TO(LEX_XO)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_XO:
			IS('R')      TO(LEX_XOR)
			LABEL        TO(LEX_LABEL_LOOP)
			break;
		case LEX_XOR:
			LABEL        TO(LEX_LABEL_LOOP)
			ELSE         HAVE(TOKEN_XOR)
			break;
		default:
			// Transition from non-transition state- error.
			retState = LEX_ERROR;
	}

	// Return state that would be transitioned to for the given inputs.
	return retState;
}

