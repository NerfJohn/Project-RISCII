/*
 * ParseOp_e.h: Result of parsing a nonterminal token into series of states.
 */

#ifndef COMMON_DOMAIN_PARSEOP_E_H_
#define COMMON_DOMAIN_PARSEOP_E_H_

/*
 * @brief Result of converting nonterminal parsing token.
 *
 * Intended to be returned by "m_parse" function in Common/Domain/ParseInit_t.
 * Values, beyond indicating success, also indicate how to handle adding the
 * new states (ie with or without the instigating token).
 */
typedef enum {
	// Real values.
	PARSE_OP_INCLUDE,  // Append token used to determine next state(s)
	PARSE_OP_EXCLUDE,  // Append determined next state(s) ONLY
	PARSE_OP_ERROR     // Bad state transition
} ParseOp_e;

#endif /* COMMON_DOMAIN_PARSEOP_E_H_ */
