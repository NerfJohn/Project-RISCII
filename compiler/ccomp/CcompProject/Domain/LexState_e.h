/*
 * LexState_e.h: States (including terminal tokens) of program's lex algorithm.
 */

#ifndef DOMAIN_LEXSTATE_E_H_
#define DOMAIN_LEXSTATE_E_H_

/*
 * @brief Different states for program's lex tree. Includes terminal values.
 *
 * Organized into terminal/non-terminal values (with LEX_TKN_MAX acting as a
 * sentinel value). Intended to be used with Common/Device/Lexer.
 */
typedef enum {
	// "Token" values.
	LEX_TKN_CHAR,
	LEX_TKN_INT,
	LEX_TKN_VOID,
	LEX_TKN_UNSIGNED,
	LEX_TKN_VOLATILE,
	LEX_TKN_ID,
	LEX_TKN_SEMICOLON,
	LEX_TKN_ASSIGN,
	LEX_TKN_MINUS,
	LEX_TKN_COMMA,
	LEX_TKN_STAR,
	LEX_TKN_LSQUARE,
	LEX_TKN_RSQUARE,
	LEX_TKN_LCURLY,
	LEX_TKN_RCURLY,
	LEX_TKN_CHR_LIT,
	LEX_TKN_INT_LIT,
	LEX_TKN_STR_LIT,
	LEX_TKN_EOF,                                 // End of file char/marker

	// "Internal path" values.
	LEX_START,                                   // start of all lex paths
	LEX_CHR_CHR,                                 // Char literal sub-states
	LEX_CHR_ESC,
	LEX_CHR_END,
	LEX_HEX_ZERO,                                // hex literal sub-states
	LEX_HEX_X,
	LEX_HEX_HEX,
	LEX_STR_STR,                                 // string literal sub-states
	LEX_STR_ESC,
	LEX_STR_END,
	LEX_LOOP_KWORD,                              // keyword or identifier
	LEX_LOOP_INT,                                // decimal integer
	LEX_IS_SEMICOLON,
	LEX_IS_ASSIGN,
	LEX_IS_MINUS,
	LEX_IS_COMMA,
	LEX_IS_STAR,
	LEX_IS_LSQUARE,
	LEX_IS_RSQUARE,
	LEX_IS_LCURLY,
	LEX_IS_RCURLY,
	LEX_IS_CHR_LIT,

	// Special values.
	LEX_ERROR,                                   // terminal error state
	LEX_TKN_MAX = LEX_TKN_EOF                    // max value token value
} LexState_e;

#endif /* DOMAIN_LEXSTATE_E_H_ */
