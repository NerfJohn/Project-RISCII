/*
 * LexState_e.h: States (including terminal tokens) of program's lex algorithm.
 */

#ifndef DOMAIN_LEXSTATE_E_H_
#define DOMAIN_LEXSTATE_E_H_

#include "Common/Util/LexerDefs.h"

/*
 * @brief Different states for program's lex tree. Includes terminal values.
 *
 * Organized into terminal/non-terminal values (with LEX_TKN_MIN and
 * LEX_TKN_MAX acting as sentinel values). Intended to be used with
 * Common/Device/Lexer.
 */
typedef enum {
	// "Token" values.
	LEX_TKN_CMMNT,                               // Single line user comment
	LEX_TKN_ID,                                  // variable identifier
	LEX_TKN_INT,
	LEX_TKN_CHAR,
	LEX_TKN_STAR,
	LEX_TKN_LPAREN,
	LEX_TKN_RPAREN,
	LEX_TKN_LCURLY,
	LEX_TKN_RCURLY,
	LEX_TKN_EOF,                                 // End of file char/marker

	// "Internal path" values.
	LEX_LOOP_KWORD,                              // loop to grab "keyword"s
	LEX_LOOP_OP,                                 // loop to grab "operator"s

	// Special values.
	LEX_START   = LEXER_START,                   // start of all lex paths
	LEX_ERROR   = LEXER_ERROR,                   // terminal error state
	LEX_TKN_MIN = LEX_TKN_CMMNT,                 // min value token value
	LEX_TKN_MAX = LEX_TKN_EOF                    // max value token value
} LexState_e;

#endif /* DOMAIN_LEXSTATE_E_H_ */
