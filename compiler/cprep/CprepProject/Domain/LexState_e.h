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
	LEX_LOOP_TEXT,
	LEX_LOOP_FILENAME,
	LEX_LOOP_FILE_FIN,
	LEX_LOOP_DIRECTIVE,
	LEX_FOUND_TEXT,
	LEX_FOUND_FILE,

	// Special values.
	LEX_ERROR,                             // end state for "bad" lexing
	LEX_DIRECTIVE                          // "end state" for directives
} LexState_e;

#endif /* DOMAIN_LEXSTATE_E_H_ */
