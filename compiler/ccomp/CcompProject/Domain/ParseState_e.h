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
	PARSE_ACT_LIT,                               // Literal/constant
	PARSE_ACT_VDEC,                              // Var decl/init

	// "Non-terminal" values.
	PARSE_FILE,                                  // Start of parsing SM
	PARSE_TYPE_STRT,                             // enclosed "type" states
	PARSE_TYPE_PRE,
	PARSE_TYPE_PST,
	PARSE_TYPE_PTR,
	PARSE_LIT_STRT,                              // enclosed "literal" states
	PARSE_LIT_HEAD,
	PARSE_LIT_ITEM,
	PARSE_LIT_NEXT,
	PARSE_PRED_STRT,                             // decl predicate states
	PARSE_PRED_VDEC,

	// Special values.
	PARSE_ERR_MISMATCH,                          // expected token didn't match
	PARSE_ERR_UNEXPECTED,                        // token not in expected set
	PARSE_ACT_MIN = PARSE_ACT_FILE,
	PARSE_ACT_MAX = PARSE_ACT_VDEC,
} ParseState_e;

#endif /* DOMAIN_PARSESTATE_E_H_ */
