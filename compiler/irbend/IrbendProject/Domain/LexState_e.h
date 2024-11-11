/*
 * LexState_e.h: Sub-states used to find programming language tokens.
 */

#ifndef DOMAIN_LEXSTATE_E_H_
#define DOMAIN_LEXSTATE_E_H_

#include "Domain/LexToken_e.h"

/*
 * States of programming language lexer. Extends LexToken_e in terms of values.
 *
 * For brevity, states that correspond to LexToken_e values are not directly
 * given (but instead can be LexToken_e values casted over). Enum focuses on
 * defining "sub states" of lexing (plus an error state).
 */
typedef enum {
	// Real values (LexToken_e extension).
	LEX_START = TOKEN_LEX_MAX + 1,         // start of all lexing paths
	LEX_LOOP_SYMBOL,                       // loop for capturing symbol
	LEX_IS_MINUS,                          // "-" confirmed, should be integer
	LEX_LOOP_INT,                          // loop for capturing integer
	LEX_IS_SLASH,                          // "/" confirmed, may be comment
	LEX_LOOP_COMMENT,                      // loop for capturing comment
	LEX_IS_EQUALS,                         // "=" confirmed
	LEX_IS_LCURLY,                         // "{" confirmed
	LEX_IS_RCURLY,                         // "}" confirmed
	LEX_IS_COMMA,                          // "," confirmed
	LEX_KW_D,                              // "d" keywords
	LEX_KW_DE,
	LEX_KW_DEC,
	LEX_KW_DEF,
	LEX_KW_G,                              // "g" keywords
	LEX_KW_GL,
	LEX_KW_GLO,
	LEX_KW_GLOB,
	LEX_KW_I,                              // "i" keywords
	LEX_KW_I1,
	LEX_KW_I8,
	LEX_KW_I16,

	// Special values.
	LEX_ERROR                              // end state for "bad" lexing
} LexState_e;

#endif /* DOMAIN_LEXSTATE_E_H_ */
