/*
 * LexState_e.h
 *
 * "States of an assembly language lexer- extends LexToken_e type"
 */

#ifndef SRC_DOMAINLAYER_LEXSTATE_E_H_
#define SRC_DOMAINLAYER_LEXSTATE_E_H_

#include "DomainLayer/LexToken_e.h"

/*
 * States of assembly language lexer. Extends LexToken_e in terms of values.
 *
 * For brevity, states that correspond to LexToken_e values are not directly
 * given (but instead can be LexToken_e values casted over). Enum focuses on
 * defining "sub states" of lexing (plus an error state).
 */
typedef enum {
	// Core values (extend LexToken_e values).
	LEX_START = TOKEN_LEX_MAX + 1, // Start of all lex/regex paths
	LEX_ERROR,                     // "Bad" end state for lexed value
	LEX_SLASH_HANDLE,              // '/' peeked- comment?
	LEX_COMMENT_LOOP,              // Comment valid- greedy
	LEX_FLAG_START,                // Prefix of flag peeked
	LEX_FLAG_LOOP,                 // Flag valid- greedy
	LEX_REG_START,                 // Prefix of register peeked
	LEX_REG_LOOP,                  // Register valid- greedy
	LEX_ZERO_HANDLE,               // Zero peeked- decimal or hex?
	LEX_MINUS_HANDLE,              // Minus peeked- decimal?
	LEX_HEX_START,                 // Prefix of hex peeked
	LEX_HEX_LOOP,                  // Hex valid- greedy
	LEX_DECIMAL_LOOP,              // Decimal valid- greedy
	LEX_S,                         // "S" keywords
	LEX_SH,
	LEX_SHR
} LexState_e;

#endif /* SRC_DOMAINLAYER_LEXSTATE_E_H_ */
