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
	LEX_TKN_TEXT,                                // line of source file text
	LEX_TKN_FILE,                                // line matching file include
	LEX_TKN_DEF,                                 // line matching defined name
	LEX_TKN_INCLUDE,
	LEX_TKN_DEFINE,
	LEX_TKN_IFDEF,
	LEX_TKN_IFNDEF,
	LEX_TKN_ENDIF,
	LEX_TKN_EOF,                                 // End of file char/marker

	// "Internal path" values.
	LEX_START,                                   // start of all lex paths
	LEX_LOOP_PRE_WS,                             // whitespace prior to token
	LEX_LOOP_TEXT,                               // grab whole line of text
	LEX_LOOP_HASH,
	LEX_LOOP_INC_FILE,                           // grab file include itself
	LEX_LOOP_INC_END,                            // grab ending line of file
	LEX_LOOP_DEF_WORD,                           // grab word being defined
	LEX_LOOP_DEF_END,                            // grab ending line of define
	LEX_END_TEXT,
	LEX_END_FILE,
	LEX_END_DEF,

	// Special values.
	LEX_ERROR,                                   // terminal error state
	LEX_TKN_MAX = LEX_TKN_EOF                    // max value token value
} LexState_e;

#endif /* DOMAIN_LEXSTATE_E_H_ */
