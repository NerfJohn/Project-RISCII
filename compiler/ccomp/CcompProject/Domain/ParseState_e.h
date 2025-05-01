/*
 * ParseState_e.h: States (including terminals) of program's parsing algorithm.
 */

#ifndef DOMAIN_PARSESTATE_E_H_
#define DOMAIN_PARSESTATE_E_H_

#include "Common/Util/ParserDefs.h"
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
	PARSE_ACT_FILE       = LEX_TKN_MAX + 1,      // Root of file's AST
	PARSE_ACT_FDEC,                              // Function decl/definition

	// "Non-terminal" values.
	// TODO

	// Special values.
	PARSE_FILE           = PARSER_START,         // start of parsing SM
	PARSE_INP_TKN        = PARSER_THIS,          // token used to select list
	PARSE_ERR_MISMATCH   = PARSER_MISMATCH,      // expected token didn't match
	PARSE_ERR_UNEXPECTED = PARSER_UNEXPECTED,    // token not in expected set
	PARSE_ACT_MIN        = PARSE_ACT_FILE,
	PARSE_ACT_MAX        = PARSE_ACT_FDEC,
} ParseState_e;

#endif /* DOMAIN_PARSESTATE_E_H_ */
