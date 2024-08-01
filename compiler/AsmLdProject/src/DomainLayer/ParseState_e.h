/*
 * ParseState_e.h
 *
 * "States of the assembly language parser- extends ParseAction_e type"
 */

#ifndef SRC_DOMAINLAYER_PARSESTATE_E_H_
#define SRC_DOMAINLAYER_PARSESTATE_E_H_

#include "ParseAction_e.h"

/*
 * States used to parse assembly language. Extends ParseAction_e type.
 *
 * For brevity, states that correspond to ParseAction_e values are not directly
 * given (but can be ParseAction_e values casted over). Enum focuses on defining
 * the sub-states used while parsing.
 *
 * (By derivation, this enum also extends LexToken_e as well.)
 */
typedef enum {
	// Cores values (extended from ParseAction_e).
	PARSE_START = ACTION_PARSE_MAX + 1, // Start of assembly file parsing
	PARSE_OPT_FLAG,                     // Instr w/ optional flag (vs reg)
	PARSE_3RD_OP,                       // Instr w/ imm vs reg for 3rd operand
	PARSE_DATA_VALUE,                   // Data value to init in data section
	PARSE_DATA_ELEMENT                  // Element in data init array
} ParseState_e;

#endif /* SRC_DOMAINLAYER_PARSESTATE_E_H_ */
