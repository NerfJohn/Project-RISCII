/*
 * ParseState_e.h: Sub-states broken down into tokens and actions.
 */

#ifndef DOMAIN_PARSESTATE_E_H_
#define DOMAIN_PARSESTATE_E_H_

#include "Domain/ParseAction_e.h"

/*
 * States used to parse programming language. Extends ParseAction_e type.
 *
 * For brevity, states that correspond to ParseAction_e values are not directly
 * given (but can be ParseAction_e values casted over). Enum focuses on defining
 * the sub-states used while parsing/breaking down language syntax.
 */
typedef enum {
	// Real values (ParseAction_e extension).
	PARSE_FILE = ACTION_PARSE_MAX + 1,           // start of file to parse
	PARSE_GLOB_SYM                               // symbol w/ optional "glob"
} ParseState_e;

#endif /* DOMAIN_PARSESTATE_E_H_ */
