/*
 * ParseAction_e.h
 *
 * "Actions to take during assembly language parsing- extends LexToken_e type"
 */

#ifndef SRC_DOMAINLAYER_PARSEACTION_E_H_
#define SRC_DOMAINLAYER_PARSEACTION_E_H_

#include "DomainLayer/LexToken_e.h"

/*
 * Actions used to build items from parsed tokens. Extends LexToken_e type.
 *
 * For brevity, states that correspond to LexToken_e values are not directly
 * given (but can be LexToken_e values casted over). Enum focuses on defining
 * the actions themselves that are taken by the parsing algorithm.
 */
typedef enum {
	// Core values (extend LexToken_e values).
	ACTION_BUILD_INSTRUCTION = TOKEN_LEX_MAX + 1, // Build instruction item
	ACTION_BUILD_LABEL_DECL,                      // Build label decl item
	ACTION_BUILD_LOAD_ADDR,                       // Build load addr item

	// Special values.
	ACTION_INVALID,                               // Unknown/Invalid action
	ACTION_PARSE_MIN = ACTION_BUILD_INSTRUCTION,  // Lowest value action
	ACTION_PARSE_MAX = ACTION_INVALID             // Highest value action
} ParseAction_e;

#endif /* SRC_DOMAINLAYER_PARSEACTION_E_H_ */
