/*
 * ParseState_e.h: States (including terminals) of program's parsing algorithm.
 */

#ifndef DOMAIN_PARSESTATE_E_H_
#define DOMAIN_PARSESTATE_E_H_

#include "Domain/LexState_e.h"

/*
 * @brief Different states for program's parse stack. Includes terminal values.
 *
 * Organized into terminal/non-terminal values (with PARSE_ACT_MIN and
 * PARSE_ACT_MAX as sentinel values). Intended to be used with
 * Common/Device/Parser.
 */
typedef enum {
	// "Action" values.
	PARSE_ACT_FILE = LEX_TKN_MAX + 1,            // Root of file's AST
	PARSE_ACT_TEXT,                              // Source text from file
	PARSE_ACT_INCL,                              // file (content) inclusion
	PARSE_ACT_DEF,                               // preprocessor definition
	PARSE_ACT_COND,                              // conditional- contains nodes

	// "Non-terminal" values.
	PARSE_FILE,                                  // start of parsing SM
	PARSE_TEXT,                                  // list of source text
	PARSE_COND,                                  // list of item in conditional

	// Special values.
	PARSE_ERR_MISMATCH,                          // expected token didn't match
	PARSE_ERR_UNEXPECTED,                        // token not in expected set
	PARSE_ACT_MIN = PARSE_ACT_FILE,
	PARSE_ACT_MAX = PARSE_ACT_COND,
} ParseState_e;

#endif /* DOMAIN_PARSESTATE_E_H_ */
