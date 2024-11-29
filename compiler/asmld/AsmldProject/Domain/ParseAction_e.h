/*
 * ParseAction_e.h: Specific actions needed to generate fully parsed items.
 */

#ifndef DOMAIN_PARSEACTION_E_H_
#define DOMAIN_PARSEACTION_E_H_

#include "Domain/LexToken_e.h"

/*
 * Actions used to build items from parsed tokens. Extends LexToken_e type.
 *
 * For brevity, states that correspond to LexToken_e values are not directly
 * given (but can be LexToken_e values casted over). Enum focuses on defining
 * the actions themselves that are executed by the parsing algorithm.
 */
typedef enum {
	// Real values (LexToken_e extension).
	ACTION_INSTR = TOKEN_LEX_MAX + 1,            // instruction operation
	ACTION_DATA,                                 // data related directives
	ACTION_DECL,                                 // label declaration

	// Special values.
	ACTION_INVALID,                              // Unknown/Invalid action
	ACTION_PARSE_MIN = ACTION_INSTR,             // Lowest value action
	ACTION_PARSE_MAX = ACTION_INVALID            // Highest value action
} ParseAction_e;

#endif /* DOMAIN_PARSEACTION_E_H_ */
