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
	LEX_LOOP_COMMENT,                      // loop used to capture comments
	LEX_HANDLE_PERCENT,                    // "%" sign found
	LEX_LOOP_FLAGS,                        // loop used to capture flags
	LEX_HANDLE_DOLLAR,                     // "$" sign found
	LEX_LOOP_REGISTER,                     // loop used to capture register #
	LEX_HANDLE_MINUS,                      // "-" sign found
	LEX_HANDLE_ZERO,                       // "0" found for decimal or hex
	LEX_HANDLE_HEX,                        // "0x" sign found
	LEX_HANDLE_ESCAPE,                     // "\" found for string escape char
	LEX_LOOP_DECIMAL,                      // loop used to capture decimal imm
	LEX_LOOP_HEX,                          // loop used to capture hex imm
	LEX_LOOP_NAME,                         // loop for label/keywords
	LEX_LOOP_DIRECTIVE,                    // loop for directive keywords
	LEX_LOOP_STR,                          // loop for string literals
	LEX_FOUND_COLON,                       // ':' char found
	LEX_FOUND_STRLIT,                      // string literal finished
	LEX_FOUND_LCURLY,                      // '{' char found
	LEX_FOUND_RCURLY,                      // '}' char found

	// Special values.
	LEX_ERROR,                             // end state for "bad" lexing
	LEX_NAME,                              // "end state" for labels/keywords
	LEX_DIRECTIVE                          // "end state" for directives
} LexState_e;

#endif /* DOMAIN_LEXSTATE_E_H_ */
